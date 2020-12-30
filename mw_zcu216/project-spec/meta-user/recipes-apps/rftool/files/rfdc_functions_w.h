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
#ifndef SRC_RFDC_FUNCTIONS_W_H_
#define SRC_RFDC_FUNCTIONS_W_H_

/***************************** Include Files *********************************/

#include "board.h"
#include "common.h"

/**************************** Type Definitions *******************************/
/***************** Macros (Inline Functions) Definitions *********************/
static inline u32 XRFdc_IsBlockEnabled(XRFdc *InstancePtr, u32 Type,
				       u32 Tile_Id, u32 Block_Id)
{
	u32 IsBlockAvail;
	if (Type == XRFDC_DAC_TILE) {
		IsBlockAvail =
			XRFdc_IsDACBlockEnabled(InstancePtr, Tile_Id, Block_Id);
	} else {
		IsBlockAvail =
			XRFdc_IsADCBlockEnabled(InstancePtr, Tile_Id, Block_Id);
	}
	return IsBlockAvail;
}

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
/****************************************************************************/
/**
*
* This function restarts the requested tile. It can restart a single tile and
* alternatively can restart all the tiles. Existing register settings are not
* lost or altered in the process. It just starts the requested tile(s).
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void StartUp(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function stops the tile as requested. It can also stop all the tiles if
* requested for. It does not clear any of the existing register settings. It
*just
* stops the requested tile(s)
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void Shutdown(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets complete status of IP (DAC/ADC) like block status, tile
*status,
* power up state and PLL state.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetIPStatus(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function updates Mixer and NCO settings to the corresponding
* hardware registers.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetMixerSettings(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets Mixer and NCO settings from the corresponding
* hardware registers.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetMixerSettings(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets current QMC settings.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetQMCSettings(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function resets the requested tile. It can reset all the tiles as well.
* In the process, all existing register settings are cleared and are replaced
* with the settings initially configured.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void Reset(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set DAC/ADC QMC Gain, Phase, Offset settings.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetQMCSettings(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get coarse delay settings for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetCoarseDelaySettings(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set coarse delay settings for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetCoarseDelaySettings(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get interpolation factor applied for DAC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetInterpolationFactor(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set interpolation factor for DAC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetInterpolationFactor(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get decimation factor applied for ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetDecimationFactor(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set decimation factor for ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetDecimationFactor(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set Nyquist factor for DAC and ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetNyquistZone(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Nyquist factor for DAC and ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetNyquistZone(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get DAC output current.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetOutputCurr(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get PLL lock status.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetPLLLockStatus(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get clock source for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetClockSource(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set dynamic PLL configuration for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void DynamicPLLConfig(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set fabric clock divider values for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetFabClkOutDiv(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get fabric clock divider values for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetFabClkOutDiv(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to start/stop RFDC FIFO for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetupFIFO(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get RFDC FIFO status for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetFIFOStatus(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Fabric Write Valid dWords for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetFabWrVldWords(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Fabric Read Valid dWords for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetFabRdVldWords(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set Fabric Read Valid dWords for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetFabRdVldWords(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set Fabric Write Valid dWords for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetFabWrVldWords(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set Decode mode for DAC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetDecoderMode(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Decode mode for DAC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetDecoderMode(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to reset NCO phase for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void ResetNCOPhase(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to dump DAC/ADC register values.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void DumpRegs(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function will trigger the update event for an event for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void UpdateEvent(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set calibration mode for ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetCalibrationMode(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get calibration mode for ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetCalibrationMode(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get a data converter block status for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetBlockStatus(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Threshold settings for ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/

void GetThresholdSettings(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set Threshold settings for ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetThresholdSettings(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Lookup Config of the RfDC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void LookupConfig(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to read RfDC 32 bit register.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RF_ReadReg32(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to write RfDC 32 bit register.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RF_WriteReg32(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to read RfDC 16 bit register.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RF_ReadReg16(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to write RfDC 16 bit register.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RF_WriteReg16(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to read RfDC 8 bit register.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RF_ReadReg8(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to write RfDC 8 bit register.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RF_WriteReg8(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to setup multiband configuration for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void MultiBand(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function return XRFDC version.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RfdcVersion(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Data Converter connected for digital data path I and
*Q.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetConnectedData(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set Inverse Sync Filter.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetInvSincFIR(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Inverse Sync Filter.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetInvSincFIR(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to clear interrupt status.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void IntrClr(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get interrupt status.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetIntrStatus(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function return xrfdc version.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void JtagIdcode(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get PLL configuration for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetPLLConfig(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get VCM configuration.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetLinkCoupling(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to initialise Multi tile synchronisation for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void MultiConverter_Init(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to synchronize data between tiles for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void MultiConverter_Sync(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to configure sysref clock for Multi tile synchronisation
* for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void MTS_Sysref_Config(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to setup resources for Multi tile synchronisation
* for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void MTS_Setup(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to know whether Multi tile synchronisation is enabled
* for DAC/ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetMTS_Setup(convData_t *cmdVals, char *txstrPtr, int *status);
/****************************************************************************/
/**
*
* This function is to set calibration freeze feature for ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetCalFreeze(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get calibration freeze status for ADC.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetCalFreeze(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get Dither status.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetDither(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set Dither status.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetDither(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to get MTS enable status.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetMTSEnable(convData_t *cmdVals, char *txstrPtr, int *status);
/****************************************************************************/
/**
*
* This function is to check whether the image is appropriate for the part
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void CheckImage(convData_t *cmdVals, char *txstrPtr, int *status);

void SetDataPathMode(convData_t *cmdVals, char *txstrPtr, int *status);
void GetDataPathMode(convData_t *cmdVals, char *txstrPtr, int *status);

void SetIMRPassMode(convData_t *cmdVals, char *txstrPtr, int *status);
void GetIMRPassMode(convData_t *cmdVals, char *txstrPtr, int *status);
void SetCalCoefficients(convData_t *cmdVals, char *txstrPtr, int *status);
void GetCalCoefficients(convData_t *cmdVals, char *txstrPtr, int *status);
void DisableCoefficientsOverride(convData_t *cmdVals, char *txstrPtr,
				 int *status);

void SetClkDistribution(convData_t *cmdVals, char *txstrPtr, int *status);

void GetClkDistribution(convData_t *cmdVals, char *txstrPtr, int *status);

void SetDACVOP(convData_t *cmdVals, char *txstrPtr, int *status);
void SetDSA(convData_t *cmdVals, char *txstrPtr, int *status);
void GetDSA(convData_t *cmdVals, char *txstrPtr, int *status);
void IntrEnable(convData_t *cmdVals, char *txstrPtr, int *status);
void IntrDisable(convData_t *cmdVals, char *txstrPtr, int *status);

void SetDACCompMode(convData_t *cmdVals, char *txstrPtr, int *status);
void GetDACCompMode(convData_t *cmdVals, char *txstrPtr, int *status);
void GetEnabledInterrupts(convData_t *cmdVals, char *txstrPtr, int *status);
void CheckDigitalPathEnabled(convData_t *cmdVals, char *txstrPtr, int *status);
void SetPwrMode(convData_t *cmdVals, char *txstrPtr, int *status);
void GetPwrMode(convData_t *cmdVals, char *txstrPtr, int *status);

#endif /* SRC_RFDC_FUNCTIONS_W_H_ */
