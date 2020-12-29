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
#include "clock_interface.h"
#include "board.h"
#include "tcp_interface.h"
#include "xrfclk.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

/************************** Variable Definitions *****************************/
int RFCLK_present = 0; /* 0 - not OK, 1 - OK */
extern const u8 LMK_FREQ_LIST[LMK_FREQ_NUM][FREQ_LIST_STR_SIZE];
extern const u8 ADC_FREQ_LIST[LMX_ADC_NUM][FREQ_LIST_STR_SIZE];
extern const u8 DAC_FREQ_LIST[LMX_DAC_NUM][FREQ_LIST_STR_SIZE];
extern int enTermMode;
int LMKCurrentFreq;
int DACCurrentFreq;
int ADC0CurrentFreq;

struct lmk_freq LMKFreq = { 10000,  10000, 76800,  1,    245760, 7680,
			    245760, 7680,  245760, 7680, 245760, 7680,
			    122880, 7680,  245760, 7680, 245760, 7680 };
struct lmx_freq LMX1Freq = { 0, 0, 0, 0, 0 };
struct lmx_freq LMX2Freq = { 0, 0, 0, 0, 0 };

/* The config programm protection flag 
    programingConfigFlag = 0;..Outside the progarm config routine.
    programingConfigFlag = 1;..Inside the progarm config routine. */
u32 programingConfigFlag = 0;
/************************** Function Definitions *****************************/

void rfclkWriteReg(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 board_id = cmdVals[0].u;
	u32 chip_id = cmdVals[1].u;
	u32 reg_addr = cmdVals[2].u;
	u32 data = cmdVals[3].u;
	u32 tmp;

	if (RFCLK_present == 0) {
		printf("\nError: CLK104-not present %s \r\n", __func__);
		goto err;
	}

	if (board_id >= BOARD_ID_MAX)
		goto err;

	if (chip_id >= RFCLK_CHIP_NUM) {
		printf("\n Write register to chip %d failed %s \r\n", chip_id,
		       __func__);
		goto err;
	}

	if (chip_id == RFCLK_LMK)
		tmp = ((reg_addr & 0xffff) << 8) + (data & 0xff);
	else
		tmp = ((reg_addr & 0xff) << 16) + (data & 0xffff);

	if (programingConfigFlag == 0) {
		printf("New config cannot be programmed");
		goto err;
	}

	if (SUCCESS != XRFClk_WriteReg(chip_id, tmp)) {
		printf("\n Write register failed %s \r\n", __func__);
		goto err;
	}

	sprintf(Response, " %d %d %d %d", board_id, chip_id, reg_addr, data);
	strcat(txstrPtr, Response);
	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("err : cmd = XRFClk_WriteReg\n"
		       "board_id = %u\n"
		       "chip_id = %u\n"
		       "reg_addr = %u\n"
		       "data = %u\n\n",
		       board_id, chip_id, reg_addr, data);

	*status = FAIL;
}

void rfclkReadReg(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 board_id = cmdVals[0].u;
	u32 chip_id = cmdVals[1].u;
	u32 reg_addr = cmdVals[2].u;
	u32 data = 0;

	if (RFCLK_present == 0) {
		printf("\nError: CLK104-not present %s \r\n", __func__);
		goto err;
	}

	if (board_id >= BOARD_ID_MAX)
		goto err;

	if (chip_id >= RFCLK_CHIP_NUM) {
		printf("\n Read register from chip %d failed %s \r\n", chip_id,
		       __func__);
		goto err;
	}

	if (chip_id == RFCLK_LMK)
		data = (reg_addr & 0xffff) << 8;
	else
		data = (reg_addr & 0xff) << 16;

	if (SUCCESS != XRFClk_ReadReg(chip_id, &data)) {
		printf("\n Read register failed %s \r\n", __func__);
		goto err;
	}

	if (chip_id == RFCLK_LMK)
		data = data & 0xff;
	else
		data = data & 0xffff;

	sprintf(Response, " %d %d %d %d", board_id, chip_id, reg_addr, data);
	strcat(txstrPtr, Response);
	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("err : cmd = XRFClk_ReadReg\n"
		       "board_id = %u\n"
		       "chip_id = %u\n"
		       "reg_addr = %u\n"
		       "data = %u\n\n",
		       board_id, chip_id, reg_addr, data);

	*status = FAIL;
}

void GetExtParentClkList(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 board_id;
	int i = 0;
	board_id = cmdVals[0].u;

	if (RFCLK_present == 0) {
		printf("\nError: CLK104-not present %s \r\n", __func__);
		goto err;
	}

	if (board_id >= BOARD_ID_MAX)
		goto err;

	for (i = 0; i < LMK_FREQ_NUM; i++) {
		sprintf(Response, " %s", LMK_FREQ_LIST[i]);
		strncat(txstrPtr, Response, BUF_MAX_LEN);
	}

	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("err : cmd = GetExtParentClkList\nboard_id = %u\n",
		       board_id);

	*status = FAIL;
}

void GetExtParentClkConfig(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 board_id;
	board_id = cmdVals[0].u;

	if (RFCLK_present == 0) {
		printf("\nError: CLK104-not present %s \r\n", __func__);
		goto err;
	}

	if (board_id >= BOARD_ID_MAX)
		goto err;

	sprintf(Response, " %d", LMKCurrentFreq);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("err : cmd = GetExtParentClkConfig\nboard_id = %u\n",
		       board_id);

	*status = FAIL;
}

void GetExtPllFreqList(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 board_id, Pll_Src;
	int i = 0;
	board_id = cmdVals[0].u;
	Pll_Src = cmdVals[1].u;

	if (RFCLK_present == 0) {
		printf("\nError: CLK104-not present %s \r\n", __func__);
		goto err;
	}

	if (board_id >= BOARD_ID_MAX)
		goto err;

	switch (Pll_Src) {
	case RFCLK_LMX2594_1:
		for (i = 0; i < LMX_ADC_NUM; i++) {
			sprintf(Response, " %s", ADC_FREQ_LIST[i]);
			strncat(txstrPtr, Response, BUF_MAX_LEN);
		}
		break;
	case RFCLK_LMX2594_2:
		for (i = 0; i < LMX_DAC_NUM; i++) {
			sprintf(Response, " %s", DAC_FREQ_LIST[i]);
			strncat(txstrPtr, Response, BUF_MAX_LEN);
		}
		break;
	default:
		printf("\n PLL selection not available %s \r\n", __func__);
		goto err;
	}

	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("err : cmd = GetExtPllFreqList\n"
		       "board_id = %u\n"
		       "Pll_Src = %u\n\n",
		       board_id, Pll_Src);

	*status = FAIL;
}

void GetExtPllConfig(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 board_id;
	u32 Pll_Src;
	board_id = cmdVals[0].u;
	Pll_Src = cmdVals[1].u;
	u32 freq = 0;

	if (RFCLK_present == 0) {
		printf("\nError: CLK104-not present %s \r\n", __func__);
		goto err;
	}

	if (board_id >= BOARD_ID_MAX)
		goto err;

	if (Pll_Src > RFCLK_LMX2594_2)
		goto err;

	if (Pll_Src == RFCLK_LMX2594_1)
		freq = ADC0CurrentFreq;

	if (Pll_Src == RFCLK_LMX2594_2)
		freq = DACCurrentFreq;

	sprintf(Response, " %d", freq);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("err : cmd = GetExtPllConfig\n"
		       "board_id = %u\n"
		       "Pll_Src = %u\n\n",
		       board_id, Pll_Src);

	*status = FAIL;
}

void SetExtPllClkRate(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 board_id, Pll_Src, freq;
	unsigned int ret;
	char Response[BUF_MAX_LEN] = { 0 };

	board_id = cmdVals[0].u;
	Pll_Src = cmdVals[1].u;
	freq = cmdVals[2].u;

	if (RFCLK_present == 0) {
		printf("\nError: CLK104-not present %s \r\n", __func__);
		goto err;
	}

	if (board_id >= BOARD_ID_MAX)
		goto err;

	if (Pll_Src > RFCLK_LMX2594_2)
		goto err;

	if (Pll_Src == RFCLK_LMX2594_1) {
		if (freq >= LMX_ADC_NUM) {
			printf("Wrong ADC0 freq: %d", freq);
			goto err;
		}
		ADC0CurrentFreq = freq;
	} else {
		if (freq >= LMX_DAC_NUM) {
			printf("Wrong DAC freq: %d", freq);
			goto err;
		}
		DACCurrentFreq = freq;
	}

	/* configure LMX clock */
	ret = XRFClk_SetConfigOnOneChipFromConfigId(Pll_Src, freq);
	if (ret != SUCCESS) {
		printf("Unable to set LMX %d clock with %d offset", Pll_Src,
		       freq);
		goto err;
	}

	sprintf(Response, " %d %d %d", board_id, Pll_Src, freq);

	strcat(txstrPtr, Response);

	*status = SUCCESS;
	return;

err:
	if (enTermMode)
		printf("err : cmd = SetExtPllClkRate\n"
		       "board_id = %u\n"
		       "Pll_Src = %u\n"
		       "freq = %u\n\n",
		       board_id, Pll_Src, freq);

	*status = FAIL;
}

void SetExtParentclk(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 board_id, freq;
	unsigned int ret;
	char Response[BUF_MAX_LEN] = { 0 };

	board_id = cmdVals[0].u;
	freq = cmdVals[1].u;

	if (RFCLK_present == 0) {
		printf("\nError: CLK104-not present %s \r\n", __func__);
		goto err;
	}

	if (board_id >= BOARD_ID_MAX)
		goto err;

	if (freq >= LMK_FREQ_NUM)
		goto err;

	ret = XRFClk_SetConfigOnOneChipFromConfigId(RFCLK_LMK, freq);
	if (ret != SUCCESS) {
		printf("Unable to set LMK clock with %d offset", freq);
		goto err;
	}

	LMKCurrentFreq = freq;
	sprintf(Response, " %d %d", board_id, freq);

	strcat(txstrPtr, Response);

	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("cmd = SetExtParentclk\n"
		       "board_id = %u\n"
		       "freq = %u\n\n",
		       board_id, freq);

	*status = FAIL;
}

void SetExtPllFreq(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };

	if (programingConfigFlag == 0) {
		printf("New config cannot be programmed");
		goto err;
	}

	LMKFreq.CLKin0_FREQ = cmdVals[0].u;
	LMKFreq.CLKin1_FREQ = cmdVals[1].u;
	LMKFreq.CLKin2_FREQ = cmdVals[2].u;
	LMKFreq.CLKin_SEL_AUTOPINSMODE = cmdVals[3].u;
	LMKFreq.CLKout0_FREQ = cmdVals[4].u;
	LMKFreq.CLKout1_FREQ = cmdVals[5].u;
	LMKFreq.CLKout2_FREQ = cmdVals[6].u;
	LMKFreq.CLKout3_FREQ = cmdVals[7].u;
	LMKFreq.CLKout4_FREQ = cmdVals[8].u;
	LMKFreq.CLKout5_FREQ = cmdVals[9].u;
	LMKFreq.CLKout6_FREQ = cmdVals[10].u;
	LMKFreq.CLKout7_FREQ = cmdVals[11].u;
	LMKFreq.CLKout8_FREQ = cmdVals[12].u;
	LMKFreq.CLKout9_FREQ = cmdVals[13].u;
	LMKFreq.CLKout10_FREQ = cmdVals[14].u;
	LMKFreq.CLKout11_FREQ = cmdVals[15].u;
	LMKFreq.CLKout12_FREQ = cmdVals[16].u;
	LMKFreq.CLKout13_FREQ = cmdVals[17].u;
	LMX1Freq.Fosc_FREQ = cmdVals[18].u;
	LMX1Freq.FoutA_FREQ = cmdVals[19].u;
	LMX1Freq.FoutB_FREQ = cmdVals[20].u;
	LMX1Freq.Fpd_FREQ = cmdVals[21].u;
	LMX1Freq.Fvco_FREQ = cmdVals[22].u;
	LMX2Freq.Fosc_FREQ = cmdVals[23].u;
	LMX2Freq.FoutA_FREQ = cmdVals[24].u;
	LMX2Freq.FoutB_FREQ = cmdVals[25].u;
	LMX2Freq.Fpd_FREQ = cmdVals[26].u;
	LMX2Freq.Fvco_FREQ = cmdVals[27].u;

	sprintf(Response,
		" %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
		" %d %d %d %d %d %d %d %d",
		LMKFreq.CLKin0_FREQ, LMKFreq.CLKin1_FREQ, LMKFreq.CLKin2_FREQ,
		LMKFreq.CLKin_SEL_AUTOPINSMODE, LMKFreq.CLKout0_FREQ,
		LMKFreq.CLKout1_FREQ, LMKFreq.CLKout2_FREQ,
		LMKFreq.CLKout3_FREQ, LMKFreq.CLKout4_FREQ,
		LMKFreq.CLKout5_FREQ, LMKFreq.CLKout6_FREQ,
		LMKFreq.CLKout7_FREQ, LMKFreq.CLKout8_FREQ,
		LMKFreq.CLKout9_FREQ, LMKFreq.CLKout10_FREQ,
		LMKFreq.CLKout11_FREQ, LMKFreq.CLKout12_FREQ,
		LMKFreq.CLKout13_FREQ, LMX1Freq.Fosc_FREQ, LMX1Freq.FoutA_FREQ,
		LMX1Freq.FoutB_FREQ, LMX1Freq.Fpd_FREQ, LMX1Freq.Fvco_FREQ,
		LMX2Freq.Fosc_FREQ, LMX2Freq.FoutA_FREQ, LMX2Freq.FoutB_FREQ,
		LMX2Freq.Fpd_FREQ, LMX2Freq.Fvco_FREQ);

	strncat(txstrPtr, Response, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("cmd = SetExtPllFreq\n");
	*status = FAIL;
}

void GetExtPllFreq(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };

	if (programingConfigFlag != 0) {
		printf("Config is not valid");
		goto err;
	}

	sprintf(Response,
		" %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
		" %d %d %d %d %d %d %d %d",
		LMKFreq.CLKin0_FREQ, LMKFreq.CLKin1_FREQ, LMKFreq.CLKin2_FREQ,
		LMKFreq.CLKin_SEL_AUTOPINSMODE, LMKFreq.CLKout0_FREQ,
		LMKFreq.CLKout1_FREQ, LMKFreq.CLKout2_FREQ,
		LMKFreq.CLKout3_FREQ, LMKFreq.CLKout4_FREQ,
		LMKFreq.CLKout5_FREQ, LMKFreq.CLKout6_FREQ,
		LMKFreq.CLKout7_FREQ, LMKFreq.CLKout8_FREQ,
		LMKFreq.CLKout9_FREQ, LMKFreq.CLKout10_FREQ,
		LMKFreq.CLKout11_FREQ, LMKFreq.CLKout12_FREQ,
		LMKFreq.CLKout13_FREQ, LMX1Freq.Fosc_FREQ, LMX1Freq.FoutA_FREQ,
		LMX1Freq.FoutB_FREQ, LMX1Freq.Fpd_FREQ, LMX1Freq.Fvco_FREQ,
		LMX2Freq.Fosc_FREQ, LMX2Freq.FoutA_FREQ, LMX2Freq.FoutB_FREQ,
		LMX2Freq.Fpd_FREQ, LMX2Freq.Fvco_FREQ);

	strncat(txstrPtr, Response, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
err:
	if (enTermMode)
		printf("cmd = GetExtPllFreq\n");
	*status = FAIL;
}

void SetStartEndClkConfig(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };

	programingConfigFlag = cmdVals[0].u;
	sprintf(Response, " %u", programingConfigFlag);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
}

void GetStartEndClkConfig(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };

	sprintf(Response, " %u", programingConfigFlag);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
}

void ClkBoardPresent(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };

	sprintf(Response, " %u", RFCLK_present);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
}
