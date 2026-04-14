/******************************************************************************
 *
 * Copyright (C) 2018-2021 Xilinx, Inc.  All rights reserved.
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

#ifndef SRC_LOCAL_MEM_H_

/***************************** Include Files *********************************/

#include "cmd_interface.h"
#include "rfdc_functions_w.h"
#include "xrfdc.h"

/************************** Constant Definitions *****************************/
#define lmem_rd32 XRFdc_In32
#define lmem_wr32 XRFdc_Out32

/* LMEM_INFO register address */
#define LMEM_INFO 0x00

/* LMEM_TRIGGER register address */
#define LMEM_TRIGGER 0x04
#define LMEM_ENABLE 0x08
#define LMEM_CLKSEL 0x0C
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
