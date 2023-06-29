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
#ifndef RFDC_INTERFACE_
#define RFDC_INTERFACE_

#include "xrfdc.h"

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define BUS_NAME "platform"
#define RFDC_DEVICE_ID 0

struct Channel_Map {
	u32 addr_I;
	u32 addr_Q;
	u32 Channel_I;
	u32 Channel_Q;
};

struct MMCMReg_struct {
	double Fin;
	double Fratio;
	u16 EnableClkout2;
};

extern struct Channel_Map DacMap[16];
extern struct Channel_Map AdcMap[16];
extern u16 AdcMemMap[16];
extern u16 DacMemMap[16];

/****************************************************************************/
/**
*
* This function is used to initiliase RFDC instance and related resources.
*
* @param	RFDC ID
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int rfdc_inst_init(u16 rfdc_id);

/****************************************************************************/
/**
*
* This function is used to initiliase RFDC.
*
* @param	void
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
* ***************************************************************************/
int rfdc_init(void);

/****************************************************************************/
/**
*
* This function is used to build memory map for DAC and ADC in BRAM mode
*
* @param	void
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
* ***************************************************************************/
int RFInitBuildMemoryMap(void);

int InitMMCM_ADC(void);
int InitMMCM_DAC(void);
u32 lmem_rd32(void *vaddr);
void lmem_wr32(void *vaddr, u32 val);

#endif /* RFDC_INTERFACE_ */
