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

/***************************** Include Files *********************************/
#include "rfdc_functions.h"

/************************** Constant Definitions *****************************/
extern XRFdc RFdcInst;      /* RFdc driver instance */

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

int getIntrStat(int Tile_Id, int Block_Id)
{
	int Type = RFDC_DAC;
	u32 IntrStatus;

	IntrStatus = XRFdc_GetIntrStatus(&RFdcInst, Type, Tile_Id, Block_Id);

	return IntrStatus;
}

void clearIntrStat(int Tile_Id, int Block_Id, unsigned int IntrMask)
{
	int Type = RFDC_DAC;
	XRFdc_IntrClr(&RFdcInst, Type, Tile_Id,Block_Id,  IntrMask);
}

int SetInterpolationFactor(int Tile_Id, int Block_Id)
{
	int ret, intStat;
	int type = RFDC_DAC;
	int InterpolationFactor = 2;
	int FabClkDiv = 3;

	/* Disable DAC FIFO */
	ret = XRFdc_SetupFIFO(&RFdcInst, type, Tile_Id,  DISABLE);
	if (ret != SUCCESS) {
		printf("\nXRFdc_SetupFIFO failed\r\n");
		return ret;
	}

	/* set clock devision factor */
	ret = XRFdc_SetFabClkOutDiv(&RFdcInst, type, Tile_Id,  FabClkDiv);
	if (ret != SUCCESS) {
		printf("\nXRFdc_SetFabClkOutDiv failed\r\n");
		return ret;
	}

	/* set interpolation factor */
	ret = XRFdc_SetInterpolationFactor(&RFdcInst, Tile_Id, Block_Id,
														  InterpolationFactor);
	if (ret != SUCCESS) {
		printf("\nXRFdc_SetInterpolationFactor failed\r\n");
		return ret;
	} 

	/* enable DAC FIFO */
	ret = XRFdc_SetupFIFO(&RFdcInst, type, Tile_Id,  ENABLE);
	if (ret != SUCCESS) {
		printf("\nXRFdc_SetupFIFO failed\r\n");
		return ret;
	}

	/* read interrupt status */
	intStat = getIntrStat(Tile_Id, Block_Id);

	/* clear interrupt status */
	clearIntrStat(Tile_Id, Block_Id, intStat);

	return ret;
}
