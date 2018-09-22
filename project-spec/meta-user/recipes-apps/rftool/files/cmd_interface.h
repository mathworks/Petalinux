/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sub license, and/or sell
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

#ifndef SRC_CMD_INTERFACE_H_
#define SRC_CMD_INTERFACE_H_

/***************************** Include Files *********************************/
#include <string.h>
#include <stdlib.h>
#include <metal/log.h>
#include "rfdc_functions_w.h"
#include "clock_interface.h"
#include "power_interface.h"
#include "data_interface.h"
#include "iic_interface.h"
#include "local_mem.h"

/************************** Constant Definitions *****************************/

/*maximum number of values for a command */
#define MAX_CMD_VALS 20
#define CMD_STRING_LEN 30
#define CMD_HELP_LEN   1400


/* Internal Error Status */
/* command success */
#define SUCCESS 0
/* command received is undefined */
#define ERROR_CMD_UNDEF    1
/* command received has the wrong number of arguments */
#define ERROR_NUM_ARGS     2
/* command executed returned an ERROR */
#define ERROR_EXECUTE      3

/* enable printing for debug within the command interface */
#define CMD_IF_PRINT 0

/**************************** Type Definitions *******************************/

/* cmd interface */
typedef struct cmdStruct {
	char cmdString[CMD_STRING_LEN];
	char cmdHelp[CMD_HELP_LEN];
	char argType [MAX_CMD_VALS];
	void (*cmdFunc)();
} CMDSTRUCT;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

void TermMode(convData_t *cmdVals,char *txstrPtr, int *status);
void DoHelp(convData_t *cmdVals,char *txstrPtr, int *status);
void RfdcVersion (convData_t *cmdVals,char *txstrPtr, int *status);
void Version (convData_t *cmdVals,char *txstrPtr, int *status);
void GUI_Title (convData_t *cmdVals,char *txstrPtr, int *status);
int cmdParseArg (char *strPtr, int cmdIdx );
int cmdParse( char *strPtr, char *txstrPtr);
int data_cmdParseArg (char *strPtr, int cmdIdx );
int data_cmdParse( char *strPtr, char *txstrPtr);
void MetalLoghandler(enum metal_log_level level, const char *format, ...);
void GetLog (convData_t *cmdVals,char *txstrPtr, int *status);

#endif /* SRC_CMD_INTERFACE_H_ */
