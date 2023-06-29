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
#ifndef SRC_GPIO_INTERFACE_
#define SRC_GPIO_INTERFACE_
/***************************** Include Files *********************************/
#include "board.h"
/************************** Extern Definitions *****************************/
extern int dac_userselect_gpio[(MAX_DAC_PER_TILE * MAX_DAC_TILE)];
extern int dac_mts_clk_en[MAX_DAC_TILE];
extern int adc_mts_clk_en[MAX_ADC_TILE];
/****************************************************************************/
/**
*
* This function initializes the gpios
*
* @param	None
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int init_gpio();
/****************************************************************************/
/**
*
* This function deinitializes the gpios
*
* @param	None
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int deinit_gpio();

#endif /* SRC_GPIO_INTERFACE_ */
