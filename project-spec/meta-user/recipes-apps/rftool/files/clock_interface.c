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

#include "clock_interface.h"

/************************** Constant Definitions *****************************/
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern int enTermMode;
extern XRFdc RFdcInst;
int DACCurrentFreq;
int ADC0CurrentFreq;
int ADC1CurrentFreq;
/************************** Function Definitions *****************************/

char *StrRepl(char *str, char x, char y){
    char *tmp=str;
    while(*tmp)
        if(*tmp == x)
            *tmp++ = y; /* assign first, then increment */
        else
            *tmp++;

    *tmp='\0';
    return str;
}

/*
 * The API is a wrapper function used as a bridge with the command interface. The function gets 
 *  available LMX frequencies.
 */
void GetExtPllFreqList(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN]={0};
	u32 board_id, Pll_Src;
	int i = 0;
	int end_i = 1;
	board_id = cmdVals[0].u;
	Pll_Src  = cmdVals[1].u;
	char pllfreq[16] ={};
	char *pllfreq_string[DAC_MAX] ={};

	switch (Pll_Src) {
	case PLL_C :
		memcpy(pllfreq_string, DACFREQ_STRING, sizeof(DACFREQ_STRING));
		end_i = sizeof(DACFREQ_STRING); break;
	case PLL_A :
		memcpy(pllfreq_string, ADC0FREQ_STRING, sizeof(ADC0FREQ_STRING));
		end_i = sizeof(ADC0FREQ_STRING);  break;
	case PLL_B :
		memcpy(pllfreq_string, ADC0FREQ_STRING, sizeof(ADC0FREQ_STRING));
		end_i = sizeof(ADC0FREQ_STRING);  break;
	default :
		printf("\n PLL selection not available %s \r\n", __func__);
		goto err;
	}

	for (i = 0; i < end_i/8-1; i++) {
		strncpy(pllfreq, &pllfreq_string[i][4], strlen(&pllfreq_string[i][4])-4);
		pllfreq[strlen(&pllfreq_string[i][4])-4] = '\0';
		StrRepl(pllfreq,'_','.');
		sprintf(Response," %s",pllfreq);
		strncat(txstrPtr, Response, BUF_MAX_LEN);
	}

	*status = SUCCESS;
	return;
err:
	if (enTermMode) {
		printf("err : cmd = GetExtPllFreqList\n"
				"board_id = %lu\n"
				"Pll_Src = %lu\n\n", board_id, Pll_Src);
	}

	*status = FAIL;

}

void GetExtPllConfig(convData_t *cmdVals, char *txstrPtr, int *status) {
	char Response[BUF_MAX_LEN]={0};
	u32 board_id;
	u32 Pll_Src;
	board_id = cmdVals[0].u;
	Pll_Src  = cmdVals[1].u;
	u32 freq;

	if((Pll_Src != PLL_A) && (Pll_Src != PLL_B) && (Pll_Src != PLL_C))
		goto err;

	if (Pll_Src == PLL_C)  {freq = DACCurrentFreq;}
	if (Pll_Src == PLL_A) {freq = ADC0CurrentFreq;}
	if (Pll_Src == PLL_B) {freq = ADC1CurrentFreq;}
	sprintf(Response," %d", freq);
    	strncat(txstrPtr, Response, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
err:
	if (enTermMode) {
		printf("err : cmd = GetExtPllConfig\n"
				"board_id = %lu\n"
				"Pll_Src = %lu\n\n", board_id, Pll_Src);
	}

	*status = FAIL;
}

/*
 * The API is a wrapper function used as a bridge with the command interface. The function configures
 *  and enable LMX2594(A/B/C) with given frequency.
 */
void SetExtPllClkRate(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 board_id, Pll_Src, freq;
	unsigned int ret;
	FILE *fp;
	char *path;
	char Response[BUF_MAX_LEN]={0}; 

	board_id = cmdVals[0].u;
	Pll_Src  = cmdVals[1].u;
	freq     = cmdVals[2].u;

	if((Pll_Src != PLL_A) && (Pll_Src != PLL_B) && (Pll_Src != PLL_C))
		goto err;

	if((board_id >= BOARD_ID_MAX))
		goto err;

	if (Pll_Src == PLL_A) {
		path = PLL_A_PATH;
		if(freq >= ADC_MAX) {
			printf("Wrong ADC0 freq: %d", freq);
			goto err;
		}
		ADC0CurrentFreq = freq;
	} else if (Pll_Src == PLL_B) {
		path = PLL_B_PATH;
		if(freq >= ADC_MAX) {
			printf("Wrong ADC1 freq: %d", freq);
			goto err;
		}
		ADC1CurrentFreq = freq;
	} else if (Pll_Src == PLL_C) {
		path = PLL_C_PATH;
		if(freq >= DAC_MAX) {
			printf("Wrong DAC freq: %d", freq);
			goto err;
		}
		DACCurrentFreq = freq;
	}

	/* configure LMX clock */
	ret = write_to_file(path, freq);
	if(ret != SUCCESS) {
		printf("Unable to set LMX %d clock with %d offset", Pll_Src, freq);
		goto err;
	}

	sprintf(Response," %d %d %d",board_id, Pll_Src, freq);

	strcat (txstrPtr, Response);

	*status = SUCCESS;
	return;

err:
	if (enTermMode) {
		printf("err : cmd = SetExtPllClkRate\n"
				"board_id = %lu\n"
				"Pll_Src = %lu\n"
				"freq = %lu\n\n", board_id, Pll_Src, freq);
	}

	*status = FAIL;

}

/*
 * The API is a wrapper function used as a bridge with the command interface. The function  configures
 * I2C0 and I2C1. It also configure and enables LMK04298 with given frequency.
 */
void SetExtParentclk(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 board_id, freq;
	unsigned int ret;
	FILE *fp;
	char *path;
	char Response[BUF_MAX_LEN]={0}; 

	board_id = cmdVals[0].u;
	freq     = cmdVals[1].u;

	if((board_id >= BOARD_ID_MAX))
		goto err;

	if((freq >= LMK04208_MAX) || (board_id >= BOARD_ID_MAX))
		goto err;
	
	path = PARENT_CLK_PATH;

	ret = write_to_file(path, freq);
	if(ret != SUCCESS) {
		printf("Unable to set LMK clock with %d offset", freq);
		goto err;
	}

	sprintf(Response," %d %d",board_id, freq);

	strcat (txstrPtr, Response);

	*status = SUCCESS;
	return;
err:
	if (enTermMode) {
		printf("cmd = SetExtParentclk\n"
				"board_id = %lu\n"
				"freq = %lu\n\n", board_id, freq);
	}

	*status = FAIL;
}

int initRFclock(u32 board_id, u32 lmk_freq, u32 lmx0_freq,u32 lmx1_freq,u32 lmx2_freq)
{
	unsigned int ret, i;
	FILE *fp;
	char *path, *pll_a, *pll_b, *pll_c;

	/* set LMK clock */	
	if((lmk_freq >= LMK04208_MAX) || (board_id >= BOARD_ID_MAX))
		goto err;

	if ((lmx0_freq >=  DAC_MAX) || (lmx1_freq >=  DAC_MAX) || (lmx2_freq >=  DAC_MAX))
		goto err;

	path = PARENT_CLK_PATH;
	pll_a = PLL_A_PATH;
	pll_b = PLL_B_PATH;
	pll_c = PLL_C_PATH;

	/* configure LMK clock */
	ret = write_to_file(path, lmk_freq);
	if(ret != SUCCESS) {
		printf("Unable to set LMK clock with %d offset", lmk_freq);
		goto err;
	}

	/* configure LMX 0 clock */
	ret = write_to_file(pll_a, lmx0_freq);
	if(ret != SUCCESS) {
		printf("Unable to set LMX 0 clock with %d offset", lmx0_freq);
		goto err;
	}

	/* configure LMX 1 clock */
	ret = write_to_file(pll_b, lmx1_freq);
	if(ret != SUCCESS) {
		printf("Unable to set LMX 1 clock with %d offset", lmx1_freq);
		goto err;
	}

	/* configure LMX 2 clock */
	ret = write_to_file(pll_c, lmx2_freq);
	if(ret != SUCCESS) {
		printf("Unable to set LMX 2 clock with %d offset", lmx2_freq);
		goto err;
	}

	DACCurrentFreq = lmx0_freq; 
	ADC0CurrentFreq = lmx1_freq;
	ADC1CurrentFreq = lmx0_freq;

	for(i = 0; i < 4; i++) {
    	ret = XRFdc_DynamicPLLConfig(&RFdcInst, 0, i, 1, 245.76, 4096);
	}

	for(i = 0; i < 2; i++) {
    	ret =  XRFdc_DynamicPLLConfig(&RFdcInst, 1, i, 1, 245.76, 6553.6);
	}

	return SUCCESS;

err: 
	return FAIL;

}
