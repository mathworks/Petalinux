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
#ifndef SRC_CLOCK_INTERFACE_
#define SRC_CLOCK_INTERFACE_

/***************************** Include Files *********************************/
#include "common.h"

/************************** Variable Definitions *****************************/
struct lmk_freq {
	u32 CLKin0_FREQ;
	u32 CLKin1_FREQ;
	u32 CLKin2_FREQ;
	u32 CLKin_SEL_AUTOPINSMODE;
	u32 CLKout0_FREQ;
	u32 CLKout1_FREQ;
	u32 CLKout2_FREQ;
	u32 CLKout3_FREQ;
	u32 CLKout4_FREQ;
	u32 CLKout5_FREQ;
	u32 CLKout6_FREQ;
	u32 CLKout7_FREQ;
	u32 CLKout8_FREQ;
	u32 CLKout9_FREQ;
	u32 CLKout10_FREQ;
	u32 CLKout11_FREQ;
	u32 CLKout12_FREQ;
	u32 CLKout13_FREQ;
};
struct lmx_freq {
	u32 Fosc_FREQ;
	u32 FoutA_FREQ;
	u32 FoutB_FREQ;
	u32 Fpd_FREQ;
	u32 Fvco_FREQ;
};
/************************** Function Prototypes ******************************/
/****************************************************************************/
/**
*
* This function configure and enables LMK with given frequency.
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
void SetExtParentclk(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function configures and enable LMX2594(A/B) with a given frequency.
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
void SetExtPllClkRate(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets available LMK frequencies.
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
void GetExtParentClkList(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets currently configured LMK frequency.
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
void GetExtParentClkConfig(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets available LMX frequencies.
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
void GetExtPllFreqList(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets currently configured LMX frequency, for requested source.
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
void GetExtPllConfig(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function writes data to register in ADC LMX, DAC LMX or LMK.
* This command will return failure if Start/Stop flag is set to 0.
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
void rfclkWriteReg(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function reads register in ADC LMX, DAC LMX or LMK.
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
void rfclkReadReg(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function sets ADC LMX, DAC LMX and LMK frequency data. This command
* will return failure if Start/Stop flag is set to 0.
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
void SetExtPllFreq(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function reads ADC LMX, DAC LMX and LMK frequency data.This command
* will return failure if Start/Stop flag is set to 1.
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
void GetExtPllFreq(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function sets clock config protection flag. Setting new config will
* have the following steps:
*  1. GUI sets this flag to 1
*  2. GUI writes a new config data to LMK and LMX
*  3. GUI sets a frequenzy data in rftool
*  4. Set this flag to 0
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
void SetStartEndClkConfig(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* Read a clock config protection flag.
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
void GetStartEndClkConfig(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* CLK104 present and functioning OK
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
void ClkBoardPresent(convData_t *cmdVals, char *txstrPtr, int *status);

#endif /* SRC_CLOCK_INTERFACE_ */
