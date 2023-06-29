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
#include "power_interface.h"
#include "rfdc_functions_w.h"
#include "tcp_interface.h"
#include "common.h"
#include <string.h>
#include <stdio.h>

/************************** Constant Definitions *****************************/
extern XRFdc RFdcInst;
extern int enTermMode;
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

void SetDACcurrent_hw(u32 current)
{
	u32 DAC_MC_CFG2_BLOCK_ADDR;
	u32 DAC_MC_CFG3_BLOCK_ADDR;
	u16 rdVal;
	u16 wrVal;

	/* Constant value used as mask for the read modified write */
	/* The bit filed set to 1 on the mask below are the one that holds the
	 currents settings. */
	/* This will be used for the write modified read to
	 guaranty the other bit of the register remain set to their original
	 value */

	u16 DAC_MC_CFG2_OPCSCAS_MASK = 0xF8F8;

	/* value that set the current to 32 MA */
	u16 DAC_MC_CFG2_OPCSCAS_32MA = 0xA0D8;
	u16 DAC_MC_CFG3_OPCSCAS_32MA = 0xFFC0;

	/* value that set current to 20 MA */
	u32 DAC_MC_CFG2_OPCSCAS_20MA = 0x4858;
	u32 DAC_MC_CFG3_OPCSCAS_20MA = 0x87C0;

	for (int Tile_id = 0; Tile_id < 2; Tile_id++) {
		/* Loop around all DAC block within a tile */
		for (int Block_id = 0; Block_id < 4; Block_id++) {
			if (XRFdc_IsDACBlockEnabled(&RFdcInst, Tile_id,
						    Block_id)) {
				DAC_MC_CFG2_BLOCK_ADDR =
					0x6000 + (Tile_id * 0x4000) +
					(Block_id * 0x400) + 0x1CC;
				DAC_MC_CFG3_BLOCK_ADDR =
					0x6000 + (Tile_id * 0x4000) +
					(Block_id * 0x400) + 0x1D0;

				rdVal = XRFdc_ReadReg16((&RFdcInst), 0,
							DAC_MC_CFG2_BLOCK_ADDR);
				rdVal = rdVal & (~DAC_MC_CFG2_OPCSCAS_MASK);

				if (current ==
				    current_20mA) { /* set current to 20 mA */
					/* A read modified write access : a LV
					 subvi could be implemented to perform
					  the read modified write */
					wrVal = rdVal |
						DAC_MC_CFG2_OPCSCAS_20MA;
					XRFdc_WriteReg16((&RFdcInst), 0,
							 DAC_MC_CFG2_BLOCK_ADDR,
							 wrVal);
					/* A write access */
					XRFdc_WriteReg16(
						(&RFdcInst), 0,
						DAC_MC_CFG3_BLOCK_ADDR,
						DAC_MC_CFG3_OPCSCAS_20MA);
				} else if (current ==
					   current_32mA) { /* Set current to 32
					    mA */
					wrVal = rdVal |
						DAC_MC_CFG2_OPCSCAS_32MA;
					XRFdc_WriteReg16((&RFdcInst), 0,
							 DAC_MC_CFG2_BLOCK_ADDR,
							 wrVal);
					XRFdc_WriteReg16(
						(&RFdcInst), 0,
						DAC_MC_CFG3_BLOCK_ADDR,
						DAC_MC_CFG3_OPCSCAS_32MA);
				}
			}
		}
	}
}

/*
 * The API is a wrapper function used as a bridge with the command interface.
 * The function
 * configure DAC output current.
 */
void SetDACPowerMode(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 board_id, block_id, op_current;
	int tile_id, tid;
	unsigned int cval = 0x19;
	char *path = PMIC_PATH;
	FILE *fp;
	char Response[BUF_MAX_LEN] = { 0 };
	u32 OutputCurr;

	board_id = cmdVals[0].u;
	tile_id = cmdVals[1].i;
	block_id = cmdVals[2].u;
	op_current = cmdVals[3].u;

	if ((board_id >= BOARD_ID_MAX) || (op_current >= CURRENT_MAX))
		goto err;

	if (op_current == current_20mA)
		cval = 0x19;
	else if (op_current == current_32mA)
		cval = 0x1e;

	SetDACcurrent_hw(op_current);

	for (tid = 0; tid < 2; tid++) {
		/* Loop around all DAC block within a tile */
		for (block_id = 0; block_id < 4; block_id++) {
			if (XRFdc_IsDACBlockEnabled(&RFdcInst, tid, block_id)) {
				*status = XRFdc_GetOutputCurr(
					&RFdcInst, tid, block_id, &OutputCurr);
				if (*status != SUCCESS) {
					printf("Output current is not set"
					       " properly for block ID: %d\n",
					       block_id);
					goto err;
					;
				}
			}
		}
	}

	fp = fopen(path, "wb");
	if (fp == NULL) {
		printf("Error opening file : %s\n", path);
		goto err;
	}

	(void)fprintf(fp, "%x\n", cval);

	fclose(fp);

	sprintf(Response, " %d %d %d %d", board_id, tile_id, block_id,
		op_current);

	strcat(txstrPtr, Response);

	*status = SUCCESS;
	return;

err:
	if (enTermMode) {
		printf("cmd = SetDACPowerMode\n"
		       "board_id = %u\n"
		       "tile_id = %d\n"
		       "block_id = %u\n"
		       "op_current = %u\n\n",
		       board_id, tile_id, block_id, op_current);
	}

	*status = FAIL;
}

int get_data_from_file(char *path)
{
	FILE *fp;
	int val = 0, ret = 0;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		printf("Error opening file : %s\n", path);
		return FAIL;
	}

	ret = fscanf(fp, "%d", &val);
	if (ret < 0) {
		printf("error in reading file : %s\n", path);
		val = FAIL;
	}

	fclose(fp);

	return val;
}

/*
 * The API is a wrapper function used as a bridge with the command interface.
 * The function
 * get power values from driver.
 */

void GetDACPower(convData_t *cmdVals, char *txstrPtr, int *status)
{
	int tile_id;
	u32 board_id;
	int value_1, value_2, value_3, value_4, value_5;
	char Response[BUF_MAX_LEN] = { 0 };

	board_id = cmdVals[0].u;
	tile_id = cmdVals[1].i;

	if (board_id >= BOARD_ID_MAX)
		goto err;

	value_1 = get_data_from_file(DAC_AVTT_SLV_PATH);
	if (value_1 == FAIL)
		goto err;

	value_2 = get_data_from_file(DAC_AVCCAUX_SLV_PATH);
	if (value_2 == FAIL)
		goto err;

	value_3 = get_data_from_file(DAC_AVCC_SLV_PATH);
	if (value_3 == FAIL)
		goto err;

	value_4 = get_data_from_file(ADC_AVCCAUX_SLV_PATH);
	if (value_4 == FAIL)
		goto err;

	value_5 = get_data_from_file(ADC_AVCC_SLV_PATH);
	if (value_5 == FAIL)
		goto err;

	sprintf(Response, " %d %d %8d %8d %8d %8d %8d", board_id, tile_id,
		value_1, value_2, value_3, value_4, value_5);

	strcat(txstrPtr, Response);
	*status = SUCCESS;
	return;

err:
	if (enTermMode) {
		printf("cmd = GetDACPower\n"
		       "board_id = %u\n"
		       "tile_id = %d",
		       board_id, tile_id);
	}
	*status = FAIL;
}
