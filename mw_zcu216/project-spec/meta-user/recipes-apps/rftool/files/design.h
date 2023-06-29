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
#ifndef SRC_DESIGN_H_
#define SRC_DESIGN_H_

#include "common.h"

void GetMMCMReg(convData_t *cmdVals, char *txstrPtr, int *status);
u32 GetMMCMRegHW(u32 Type, int Tile_Id, u32 *Lock, u32 *Mult, u32 *MultFrac,
		 u32 *Div, u32 *Clk0Div, u32 *Clk0DivFrac, u32 *Clk1Div);
void MMCM_Rst(convData_t *cmdVals, char *txstrPtr, int *status);
u32 MMCM_Rst_HW(u32 Type, int Tile_Id);
u32 MMCM_Reprog_HW(u32 Type, int Tile_Id, u32 Mult, u32 Mult_frac, u32 Div,
		   u32 clkout0_div, u32 clkout0_frac);
void SetMMCMReg(convData_t *cmdVals, char *txstrPtr, int *status);
void SetMMCM(convData_t *cmdVals, char *txstrPtr, int *status);
void SetmmcmFin(convData_t *cmdVals, char *txstrPtr, int *status);
void GetmmcmFin(convData_t *cmdVals, char *txstrPtr, int *status);

#endif
