/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/
/*****************************************************************************/
/**
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who    Date     Changes
 * ----- ---    -------- -----------------------------------------------
 * 1.0
 *
 * </pre>
 *
 ******************************************************************************/
/***************************** Include Files *********************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "cmd_interface.h"
#include "tcp_interface.h"
#include "xrfdc.h"
#include "rfdc_interface.h"
#include "data_interface.h"
#include "error_interface.h"

/************************** Constant Definitions *****************************/
#define RFDC_DEVICE_ID 	XPAR_XRFDC_0_DEVICE_ID
/**************************** Variable Definitions *******************************/

static char rcvBuf[BUF_MAX_LEN]={0}; /* receive buffer of BUF_MAX_LEN character */
static char txBuf[BUF_MAX_LEN]={0}; /* tx buffer of BUF_MAX_LEN character */

int thread_stat = 0;
/*********************************** Main ************************************/
int main(void) 
{
	int bufLen = BUF_MAX_LEN-1; /* buffer len must be set to max buffer minus 1 */
	int numCharacters; /* number of character received per command line */
	int cmdStatus; /* status of the command: XST_SUCCES - ERROR_CMD_UNDEF - ERROR_NUM_ARGS - ERROR_EXECUTE */
	int ret; 
	pthread_t thread_id;

	/* initialize RFDC instance */
	ret = rfdc_inst_init(RFDC_DEVICE_ID);
	if(ret != XRFDC_SUCCESS) {
		printf("Failed to initialize RFDC\n");
		return -1;
	}

	/* disable ADC and DAC fifo 
	ret = disable_all_fifo(DAC);
	if(ret != SUCCESS) {
		printf("Failed to disable DAC FIFO\n");
		return -1;;
	}
	

	ret = disable_all_fifo(ADC);
	if(ret != SUCCESS) {
		printf("Failed to disable ADC FIFO\n");
		return -1;
	}
	*/

	ret = initRFclock(ZCU111, LMK04208_12M8_3072M_122M88_REVAB, DAC_245_76_MHZ, DAC_245_76_MHZ, ADC_245_76_MHZ);
	if(ret != SUCCESS){
		printf("Unable to enable RF clocks\n");
		return ret;
	}

	tcpServerInitialize();
	DataServerInitialize();

	printf("Server Init Done\n");

newConn: acceptdataConnection();
	 printf("Accepted data connection\n");
	 acceptConnection();
	 printf("Accepted command connection\n");

	 /* clear rcvBuf each time anew command is received and processed */
	 memset(rcvBuf, 0, sizeof(rcvBuf));
	 /* clear txBuf each time anew command is received and a response returned */
	 memset(txBuf, 0, sizeof(txBuf));

	 /* mark this thread as active */
	 thread_stat = 1;

	 pthread_create(&thread_id, NULL, datapath_t, NULL);

	 while(1) {
		 /* get string from io interface (Non blocking) */
		 numCharacters = getString(rcvBuf, bufLen);
		 /* a string with N character is available */
		 if(numCharacters > 0) {
			 /* parse and run with error check */
			 cmdStatus = cmdParse(rcvBuf,txBuf);
			 /* check cmParse status - return an error message or the response */
			 if (cmdStatus != SUCCESS) {
				 /* command returned an errors */
				 errorIf(txBuf, cmdStatus);
			 } else {
				 /* send response */
				 sendString(txBuf, strlen(txBuf));
			 }

			 if(strcmp(txBuf, "disconnect") == 0) {

				 thread_stat = 0;
				 shutdown_sock(COMMAND);
				 shutdown_sock(DATA);
				 printf("Closed data and command sockets\n");
				 pthread_join(thread_id, NULL);
				 break;
			 }
			 /* clear rcvBuf each time anew command is received and processed */
			 memset(rcvBuf, 0, sizeof(rcvBuf));
			 /* clear txBuf each time anew command is received and a response returned */
			 memset(txBuf, 0, sizeof(txBuf));
		 } else {
			 if(pthread_kill(thread_id, 0))
				 printf("not able to kill data processing thread\n");

			 thread_stat = 0;
			 break;
		 }
	 }
	 goto newConn;
	 return 0;
}
