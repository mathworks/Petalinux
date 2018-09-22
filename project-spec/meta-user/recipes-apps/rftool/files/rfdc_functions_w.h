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

#ifndef SRC_RFDC_FUNCTIONS_W_H_
#define SRC_RFDC_FUNCTIONS_W_H_

/***************************** Include Files *********************************/

#include "stdio.h"
#include "xrfdc.h"
#include "tcp_interface.h"
#include "board.h"
#include "xparameters.h"

/**************************** Type Definitions *******************************/
typedef union {
	int    i;
	u32    u;
	u64    l;
	double d;
	char   b[8];
} convData_t;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
void StartUp (convData_t *cmdVals, char *txstrPtr, int *status);
void Shutdown (convData_t *cmdVals, char *txstrPtr, int *status);
void GetIPStatus (convData_t *cmdVals, char *txstrPtr, int *status);
void SetMixerSettings(convData_t *cmdVals, char *txstrPtr, int *status);
void GetMixerSettings(convData_t *cmdVals, char *txstrPtr, int *status);
void GetQMCSettings(convData_t *cmdVals, char *txstrPtr, int *status);

void Reset(convData_t *cmdVals, char *txstrPtr, int *status);
void SetQMCSettings(convData_t *cmdVals, char *txstrPtr, int *status);
void GetCoarseDelaySettings(convData_t *cmdVals, char *txstrPtr, int *status);
void SetCoarseDelaySettings(convData_t *cmdVals, char *txstrPtr, int *status);
void GetInterpolationFactor(convData_t *cmdVals, char *txstrPtr, int *status);
void SetInterpolationFactor(convData_t *cmdVals, char *txstrPtr, int *status);
void GetDecimationFactor(convData_t *cmdVals, char *txstrPtr, int *status);
void SetDecimationFactor(convData_t *cmdVals, char *txstrPtr, int *status);
void SetNyquistZone(convData_t *cmdVals, char *txstrPtr, int *status);
void GetNyquistZone(convData_t *cmdVals, char *txstrPtr, int *status);
void GetOutputCurr(convData_t *cmdVals, char *txstrPtr, int *status);
void GetPLLLockStatus(convData_t *cmdVals, char *txstrPtr, int *status);
void GetClockSource(convData_t *cmdVals, char *txstrPtr, int *status);
void DynamicPLLConfig(convData_t *cmdVals, char *txstrPtr, int *status);
void SetFabClkOutDiv(convData_t *cmdVals, char *txstrPtr, int *status);
void SetupFIFO(convData_t *cmdVals, char *txstrPtr, int *status);
void GetFIFOStatus(convData_t *cmdVals, char *txstrPtr, int *status);
void GetFabWrVldWords(convData_t *cmdVals, char *txstrPtr, int *status);
void GetFabRdVldWords(convData_t *cmdVals, char *txstrPtr, int *status);
void SetFabRdVldWords(convData_t *cmdVals, char *txstrPtr, int *status);
void SetFabWrVldWords(convData_t *cmdVals, char *txstrPtr, int *status);
void SetDecoderMode(convData_t *cmdVals, char *txstrPtr, int *status);
void GetDecoderMode(convData_t *cmdVals, char *txstrPtr, int *status);
void SetSignalFlow(convData_t *cmdVals, char *txstrPtr, int *status);
void GetSignalFlow(convData_t *cmdVals, char *txstrPtr, int *status);
void ResetNCOPhase(convData_t *cmdVals, char *txstrPtr, int *status);
void DumpRegs(convData_t *cmdVals, char *txstrPtr, int *status);
void UpdateEvent(convData_t *cmdVals, char *txstrPtr, int *status);
void SetCalibrationMode(convData_t *cmdVals, char *txstrPtr, int *status);
void GetCalibrationMode(convData_t *cmdVals, char *txstrPtr, int *status);
void GetBlockStatus(convData_t *cmdVals, char *txstrPtr, int *status);
void GetThresholdSettings(convData_t *cmdVals, char *txstrPtr, int *status);
void SetThresholdSettings(convData_t *cmdVals, char *txstrPtr, int *status);
void LookupConfig(convData_t *cmdVals, char *txstrPtr, int *status);
void RF_ReadReg32(convData_t *cmdVals, char *txstrPtr, int *status);
void RF_WriteReg32(convData_t *cmdVals, char *txstrPtr, int *status);
void RF_ReadReg16(convData_t *cmdVals, char *txstrPtr, int *status);
void RF_WriteReg16(convData_t *cmdVals, char *txstrPtr, int *status);
void RF_ReadReg8(convData_t *cmdVals, char *txstrPtr, int *status);
void RF_WriteReg8(convData_t *cmdVals, char *txstrPtr, int *status);
void MultiBand(convData_t *cmdVals, char *txstrPtr, int *status);
void RfdcVersion (convData_t *cmdVals,char *txstrPtr, int *status);
void GetConnectedData(convData_t *cmdVals, char *txstrPtr, int *status);
void SetInvSincFIR(convData_t *cmdVals, char *txstrPtr, int *status);
void GetInvSincFIR(convData_t *cmdVals, char *txstrPtr, int *status);
void IntrClr(convData_t *cmdVals, char *txstrPtr, int *status);
void GetIntrStatus(convData_t *cmdVals, char *txstrPtr, int *status);
void JtagIdcode (convData_t *cmdVals,char *txstrPtr, int *status);
void GetPLLConfig (convData_t *cmdVals, char *txstrPtr, int *status);
void GetLinkCoupling(convData_t *cmdVals, char *txstrPtr, int *status);
void MultiConverter_Init (convData_t *cmdVals, char *txstrPtr, int *status);
void MultiConverter_Sync (convData_t *cmdVals, char *txstrPtr, int *status);
void MTS_Sysref_Config (convData_t *cmdVals, char *txstrPtr, int *status);
void MTS_Setup  (convData_t *cmdVals, char *txstrPtr, int *status);
void SetCalFreeze (convData_t *cmdVals, char *txstrPtr, int *status);
void GetCalFreeze (convData_t *cmdVals, char *txstrPtr, int *status);

int ResetMMCM();
#endif /* SRC_RFDC_FUNCTIONS_W_H_ */

