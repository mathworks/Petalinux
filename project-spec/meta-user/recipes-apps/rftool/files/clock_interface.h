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

#endif /* SRC_CLOCK_INTERFACE_ */
