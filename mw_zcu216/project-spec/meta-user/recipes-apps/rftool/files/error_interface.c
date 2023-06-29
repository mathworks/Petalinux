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

/***************************** Include Files *********************************/
#include "cmd_interface.h"
#include "tcp_interface.h"
#include <stdio.h>
#include <string.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/*******************************  Functions **********************************/
void errorIf(char *cmdPtr, int errId)
{
	char errMsg[BUF_MAX_LEN + 1]; /* error message buffer */

	strcpy(errMsg, "ERROR: "); /* set ERROR as first string */
	strncat(errMsg, cmdPtr, BUF_MAX_LEN); /* append command name */

	/* based on errId append the selected error message */
	switch (errId) {
	case SUCCESS:
		break;
	case ERROR_CMD_UNDEF:
		strcat(errMsg, " CMD_UNDEF");
		break;
	case ERROR_NUM_ARGS:
		strcat(errMsg, " NUM_ARGS");
		break;
	case ERROR_EXECUTE:
		strcat(errMsg, " EXECUTE");
		break;
	default:
		strcat(errMsg, " UNDEF");
		break;
	}
	/* make error message available outside of the scope of this function */
	strcpy(cmdPtr, errMsg);

	/* send error Message */
	if (0 > sendString(errMsg, strlen(errMsg)))
		printf("%s: Error in sendString %s\n", __func__, errMsg);
	printf("%s\n", errMsg);
	printf("\n*****************************\n\n");
	memset(errMsg, 0, sizeof(errMsg));
}

void errorIf_data(char *cmdPtr, int errId)
{
	char errMsg[BUF_MAX_LEN + 1]; /* error message buffer */

	strcpy(errMsg, "ERROR: "); /* set ERROR as first string */
	strncat(errMsg, cmdPtr, BUF_MAX_LEN); /* append command name */

	/* based on errId append the selected error message */
	switch (errId) {
	case SUCCESS:
		break;
	case ERROR_CMD_UNDEF:
		strcat(errMsg, " CMD_UNDEF");
		break;
	case ERROR_NUM_ARGS:
		strcat(errMsg, " NUM_ARGS");
		break;
	case ERROR_EXECUTE:
		strcat(errMsg, " EXECUTE");
		break;
	default:
		strcat(errMsg, " UNDEF");
		break;
	}
	/* make error message available outside of the scope of this function */
	strcpy(cmdPtr, errMsg);
	/* send error Message */
	if (0 > senddataString(errMsg, strlen(errMsg)))
		printf("%s: Error in senddataString %s\n", __func__, errMsg);
	printf("%s\n", errMsg);
	printf("\n*****************************\n\n");
	memset(errMsg, 0, sizeof(errMsg));
}
