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
/************************** Constant Definitions *****************************/
#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

/**************************** Type Definitions *******************************/
#ifdef XPS_BOARD_ZCU208
#define MAX_DAC_PER_TILE 2
#define MAX_ADC_PER_TILE 2
#else
#define MAX_DAC_PER_TILE 4
#define MAX_ADC_PER_TILE 4
#endif
#ifdef XPS_BOARD_ES1
#define PART_IDENTIFIER "-es1"
#else
#define PART_IDENTIFIER ""
#endif

#define MAX_DAC_TILE 4
#define MAX_ADC_TILE 4
/***************** Macros (Inline Functions) Definitions *********************/
/* TODO: not used for now */
enum board_id { ZCU111, ZC1254, ZC1275, ZCU216, ZCU208, BOARD_ID_MAX };

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

#endif /* SRC_BOARD_H_ */
