/******************************************************************************
*
* Copyright (C) 2017-2020 Xilinx, Inc.  All rights reserved.
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
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
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

#include "clock_interface.h"
#include "cmd_interface.h"
#include "data_interface.h"
#include "gpio_interface.h"
#include "design.h"
#include "error_interface.h"
#include "rfdc_interface.h"
#include "tcp_interface.h"
#include "xrfdc.h"
#include "xrfclk.h"
#include "version.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************** Constant Definitions *****************************/
#define DEFAULT_RFCLK_LMK_CONFIG 0
#define DEFAULT_DECIMATION_FACTOR XRFDC_INTERP_DECIM_1X
#define DEFAULT_INTERPOLATION_FACTOR XRFDC_INTERP_DECIM_1X
#define DEFAULT_DATA_PATH_MODE XRFDC_DATAPATH_MODE_NODUC_0_FSDIVTWO

extern int LMKCurrentFreq;
extern struct lmk_freq LMKFreq;
extern u32 programingConfigFlag;

/**************************** Variable Definitions ***************************/

static char rcvBuf[BUF_MAX_LEN] = {
	0
}; /* receive buffer of BUF_MAX_LEN character */
static char txBuf[BUF_MAX_LEN] = { 0 }; /* tx buffer of BUF_MAX_LEN character */

int thread_stat = 0;
extern XRFdc RFdcInst;
extern int RFCLK_present;

void StartUpConfig()
{
	u32 Tile_Id = 0;
	u32 Block_Id = 0;
	convData_t cmdVals[2];
	char txstrPtr[BUF_MAX_LEN + 1];
	int status;
	u32 ret;
	u32 adc_blocks;
	u32 dac_blocks = 4;
	XRFdc_Mixer_Settings Mixer_Settings;

	/* ADC default settings */
	for (Tile_Id = 0; Tile_Id < MAX_TILE_ID; Tile_Id++) {
		adc_blocks = RFdcInst.ADC_Tile[Tile_Id].NumOfADCBlocks;
		for (Block_Id = 0; Block_Id < adc_blocks; Block_Id++) {
			ret = XRFdc_GetMixerSettings(&RFdcInst, XRFDC_ADC_TILE,
						     Tile_Id, Block_Id,
						     &Mixer_Settings);
			if (FAIL == ret) {
				printf("%s: Error from XRFdc_GetMixerSettings"
				       " for ADC Tile_Id = %u Block_Id = %u\n",
				       __func__, Tile_Id, Block_Id);
			}
			Mixer_Settings.MixerType = XRFDC_MIXER_TYPE_COARSE;
			Mixer_Settings.CoarseMixFreq = XRFDC_COARSE_MIX_BYPASS;
			Mixer_Settings.MixerMode = XRFDC_MIXER_MODE_R2R;
			Mixer_Settings.EventSource = XRFDC_EVNT_SRC_TILE;
			ret = XRFdc_SetMixerSettings(&RFdcInst, XRFDC_ADC_TILE,
						     Tile_Id, Block_Id,
						     &Mixer_Settings);
			if (FAIL == ret) {
				printf("%s: Error from XRFdc_SetMixerSettings"
				       " for ADC Tile_Id = %u Block_Id = %u\n",
				       __func__, Tile_Id, Block_Id);
			}
			ret = XRFdc_SetDecimationFactor(
				&RFdcInst, Tile_Id, Block_Id,
				DEFAULT_DECIMATION_FACTOR);
			if (FAIL == ret) {
				printf("%s: Error from XRFdc_SetDecimationFactor"
				       " for ADC Tile_Id = %u Block_Id = %u\n",
				       __func__, Tile_Id, Block_Id);
			}
			cmdVals[0].u = ADC;
			cmdVals[1].u = Tile_Id;
			SetMMCM(cmdVals, txstrPtr, &status);
		}
	}

	for (Tile_Id = 0; Tile_Id < MAX_TILE_ID; Tile_Id++) {
#ifdef XPS_BOARD_ZCU208
		printf("Changing Multiband of tile %u\n", Tile_Id);
		ret = XRFdc_MultiBand(&RFdcInst, XRFDC_DAC_TILE, Tile_Id, 0x1,
				      0x4, 0x1);
		ret |= XRFdc_MultiBand(&RFdcInst, XRFDC_DAC_TILE, Tile_Id, 0x4,
				       0x4, 0x4);
		if (FAIL == ret) {
			printf("%s: Error from XRFdc_MultiBand"
			       " for DAC Tile_Id = %u Block_Id = %u\n",
			       __func__, Tile_Id, Block_Id);
		}
#endif
		for (Block_Id = 0; Block_Id < dac_blocks; Block_Id++) {
			ret = XRFdc_GetMixerSettings(&RFdcInst, XRFDC_DAC_TILE,
						     Tile_Id, Block_Id,
						     &Mixer_Settings);
			if (FAIL == ret) {
				printf("%s: Error from XRFdc_GetMixerSettings"
				       " for DAC Tile_Id = %u Block_Id = %u\n",
				       __func__, Tile_Id, Block_Id);
			}

			Mixer_Settings.MixerType = XRFDC_MIXER_TYPE_COARSE;
			Mixer_Settings.CoarseMixFreq = XRFDC_COARSE_MIX_BYPASS;
			Mixer_Settings.MixerMode = XRFDC_MIXER_MODE_R2R;
			Mixer_Settings.EventSource = XRFDC_EVNT_SRC_TILE;

			ret = XRFdc_SetMixerSettings(&RFdcInst, XRFDC_DAC_TILE,
						     Tile_Id, Block_Id,
						     &Mixer_Settings);
			ret |= XRFdc_ResetNCOPhase(&RFdcInst, XRFDC_DAC_TILE,
						   Tile_Id, Block_Id);
			if (FAIL == ret) {
				printf("%s: Error from XRFdc_SetMixerSettings or XRFdc_ResetNCOPhase"
				       " for DAC Tile_Id = %u Block_Id = %u\n",
				       __func__, Tile_Id, Block_Id);
			}

			ret = XRFdc_UpdateEvent(&RFdcInst, XRFDC_DAC_TILE,
						Tile_Id, Block_Id,
						XRFDC_EVENT_MIXER);
			if (FAIL == ret) {
				printf("%s: Error from XRFdc_UpdateEvent"
				       " for DAC Tile_Id = %u Block_Id = %u\n",
				       __func__, Tile_Id, Block_Id);
			}
#ifdef XPS_BOARD_ZCU208
			if (Block_Id == 1 || Block_Id == 3) {
				printf("ZCU208 Interpolation for block_id %u\n",
				       Block_Id);
				ret = XRFdc_SetInterpolationFactor(
					&RFdcInst, Tile_Id, Block_Id,
					XRFDC_INTERP_DECIM_OFF);
			} else {
				printf("ZCU208 Interpolation for block_id %u\n",
				       Block_Id);
				ret = XRFdc_SetInterpolationFactor(
					&RFdcInst, Tile_Id, Block_Id,
					DEFAULT_INTERPOLATION_FACTOR);
			}
#else
			printf("ZCU216 Interpolation for block_id %u\n",
			       Block_Id);
			ret = XRFdc_SetInterpolationFactor(
				&RFdcInst, Tile_Id, Block_Id,
				DEFAULT_INTERPOLATION_FACTOR);
#endif
			if (FAIL == ret) {
				printf("%s: Error from XRFdc_SetInterpolationFactor"
				       " for DAC Tile_Id = %u Block_Id = %u\n",
				       __func__, Tile_Id, Block_Id);
			}
#ifdef XPS_BOARD_ZCU208
			if (Block_Id == 0 || Block_Id == 2) {
				printf("ZCU208 datapathmode for block_id %u\n",
				       Block_Id);
				ret = XRFdc_SetDataPathMode(
					&RFdcInst, Tile_Id, Block_Id,
					DEFAULT_DATA_PATH_MODE);
			}
#else
			printf("ZCU216 datapathmode for block_id %u\n",
			       Block_Id);
			ret = XRFdc_SetDataPathMode(&RFdcInst, Tile_Id,
						    Block_Id,
						    DEFAULT_DATA_PATH_MODE);
#endif
			if (FAIL == ret) {
				printf("%s: Error from XRFdc_SetDataPathMode"
				       " for DAC Tile_Id = %u Block_Id = %u\n",
				       __func__, Tile_Id, Block_Id);
			}
		}
		cmdVals[0].u = DAC;
		cmdVals[1].u = Tile_Id;
		SetMMCM(cmdVals, txstrPtr, &status);
	}
	for (Tile_Id = 0; Tile_Id < 4; Tile_Id++) {
		for (Block_Id = 0;
		     Block_Id < RFdcInst.ADC_Tile[Tile_Id].NumOfADCBlocks;
		     Block_Id++) {
			if (XRFdc_IsADCBlockEnabled(&RFdcInst, Tile_Id,
						    Block_Id)) {
				XRFdc_IntrDisable(&RFdcInst, XRFDC_ADC_TILE,
						  Tile_Id, Block_Id,
						  0xffffffff);
				XRFdc_IntrEnable(&RFdcInst, XRFDC_ADC_TILE,
						 Tile_Id, Block_Id, 0xffffffff);
				XRFdc_IntrClr(&RFdcInst, XRFDC_ADC_TILE,
					      Tile_Id, Block_Id, 0xffffffff);
			}
		}
		for (Block_Id = 0; Block_Id < 4; Block_Id++) {
			if (XRFdc_IsDACBlockEnabled(&RFdcInst, Tile_Id,
						    Block_Id)) {
				XRFdc_IntrDisable(&RFdcInst, XRFDC_DAC_TILE,
						  Tile_Id, Block_Id,
						  0xffffffff);
				XRFdc_IntrEnable(&RFdcInst, XRFDC_DAC_TILE,
						 Tile_Id, Block_Id, 0xffffffff);
				XRFdc_IntrClr(&RFdcInst, XRFDC_DAC_TILE,
					      Tile_Id, Block_Id, 0xffffffff);
			}
		}
	}
}
/*********************************** Main ************************************/
int main(void)
{
	int bufLen = BUF_MAX_LEN -
		     1; /* buffer len must be set to max buffer minus 1 */
	int numCharacters; /* number of character received per command line */
	int cmdStatus; /* status of the command: XST_SUCCES - ERROR_CMD_UNDEF -
			  ERROR_NUM_ARGS - ERROR_EXECUTE */
	int ret;
	pthread_t thread_id;

	printf("\nVersion number; %s\n\n", RFTOOL_INTERNAL_VERSION);
#ifdef XPS_BOARD_ZCU208
	printf("\nBoard version ZCU208\n\n");
#else
	printf("\nBoard version ZCU216\n\n");
#endif

	printf("\nRFCLK v%s\n", RFCLK_VERSION);
#ifdef XPS_BOARD_ZCU208
	ret = XRFClk_Init(493);
#else
	ret = XRFClk_Init(485);
#endif
	if (ret != SUCCESS)
		printf("\nCLK104 is broken or not present\r\n");
	else {
		programingConfigFlag = 1;
		/* Configure LMK with a default config */
		ret = XRFClk_SetConfigOnOneChipFromConfigId(
			RFCLK_LMK, DEFAULT_RFCLK_LMK_CONFIG);
		if (ret != SUCCESS)
			printf("\nError: Config LMK in CLK-104\r\n");
		else {
			RFCLK_present = 1;
			LMKCurrentFreq = DEFAULT_RFCLK_LMK_CONFIG;
			printf("\nRFCLK v%s Init Done\n", RFCLK_VERSION);

			/* Power down LMX1 (DAC) */
			if (SUCCESS != XRFClk_WriteReg(RFCLK_LMX2594_1, 0x1)) {
				printf("\nError: Power down LMX1\r\n");
			}

			/* Power down LMX2 (ADC0) */
			if (SUCCESS != XRFClk_WriteReg(RFCLK_LMX2594_2, 0x1)) {
				printf("\nError: Power down LMX2\r\n");
			}
		}
		programingConfigFlag = 0;
	}

	if (RFCLK_present != 1)
		memset(&LMKFreq, 0, sizeof(LMKFreq));

	ret = rfdc_init();
	if (ret != SUCCESS) {
		printf("Failed to initialize RFDC\n");
		return -1;
	}
	/*ret = RFInitBuildMemoryMap();
	if (ret != SUCCESS) {
		printf("Failed to build memory map\n");
		return -1;
	}

	InitMMCM_ADC();
	InitMMCM_DAC();
        // In the TRD, a memory map is performed at the beginning between the PS and a few clocks on the PL to get clock divder information
        // This appears to be used to report back to the user clock speed information
        */
	tcpServerInitialize();
	DataServerInitialize();
	/*
        ret = init_mem();
	if (ret) {
		deinit_mem();
		printf("Unable to initialise memory\n");
		return -1;
	}
	printf("going to init gpio\n");
	ret = init_gpio();
	if (ret) {
		printf("Unable to initialise gpio's\n");
		deinit_gpio();
		deinit_mem();
		return -1;
	}
        */
	EnableAllInterrupts();
	//StartUpConfig();
	DisplayIpAddress();
	printf("Server Init Done\n");

newConn:
	acceptdataConnection();
	printf("Accepted data connection\n");
	acceptConnection();
	printf("Accepted command connection\n");

	/* clear rcvBuf each time anew command is received and processed */
	memset(rcvBuf, 0, sizeof(rcvBuf));
	/* clear txBuf each time anew command is received and a response
	 returned */
	memset(txBuf, 0, sizeof(txBuf));

	/* mark this thread as active */
	thread_stat = 1;

//	printf("Start data processing thread\n");
//	pthread_create(&thread_id, NULL, datapath_t, NULL);
	while (1) {
		/* get string from io interface (Non blocking) */
		numCharacters = getString(rcvBuf, bufLen);
		/* a string with N character is available */
		if (numCharacters > 0) {
			/* parse and run with error check */
			cmdStatus = cmdParse(rcvBuf, txBuf);
			/* check cmParse status - return an error message or
			 the response */
			if (cmdStatus != SUCCESS) {
				/* command returned an errors */
				errorIf(txBuf, cmdStatus);
			} else {
				/* send response */
				if (0 > sendString(txBuf, strlen(txBuf)))
					printf("%s: Error in sendString %s\n",
					       __func__, txBuf);
			}

			if (strcmp(txBuf, "disconnect") == 0) {
				thread_stat = 0;
				shutdown_sock(COMMAND);
				shutdown_sock(DATA);
				printf("Closed data and command sockets\n");
				pthread_join(thread_id, NULL);
				break;
			}
			/* clear rcvBuf each time anew command is received and
			 processed */
			memset(rcvBuf, 0, sizeof(rcvBuf));
			/* clear txBuf each time anew command is received and
			 a response returned */
			memset(txBuf, 0, sizeof(txBuf));
		} else {
		/*	printf("Kill data processing thread\n");
			if (pthread_kill(thread_id, 0))
				printf("not able to kill data processing"
				       " thread\n");
                    */
			thread_stat = 0;
			break;
		}
	}
	goto newConn;
}
