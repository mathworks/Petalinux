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
#ifndef SRC_LOCAL_MEM_H_
#define SRC_LOCAL_MEM_H_

/***************************** Include Files *********************************/

#include "common.h"

/************************** Constant Definitions *****************************/

/* LMEM_INFO register address */
#define LMEM_INFO 0x00

/* LMEM_TRIGGER register address */
#define LMEM_TRIGGER 0x04
#define LMEM_ENABLE 0x08
#define LMEM_ENABLE_TILE 0x0C
#define LMEM0_ENDADDR 0x10

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
/****************************************************************************/
/**
*
* This function sends available memory details and enabled devices.
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
void LocalMemInfo(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is used as a trigger to start DAC or ADC operations.
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
void LocalMemTrigger(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function returns address of each channel.
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
void LocalMemAddr(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function is to set number of samples in a channel.
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
void SetLocalMemSample(convData_t *cmdVals, char *txstrPtr, int *status);

#endif /* SRC_LOCAL_MEM_H_ */
