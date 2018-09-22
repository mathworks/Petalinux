/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.	All rights reserved.
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
extern XRFdc RFdcInst;

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
/************************** Function Definitions *****************************/
int initRFclock (unsigned int lmk_freq, unsigned int lmx0_freq,
								unsigned int lmx1_freq, unsigned int lmx2_freq)
{
	unsigned int ret, i;
	char *path, *pll_a, *pll_b, *pll_c;

	 /* set LMK clock */
	if(lmk_freq >= LMK04208_MAX)
		goto err;

	if ((lmx0_freq >=  DAC_MAX) || (lmx1_freq >=	DAC_MAX) ||
												(lmx2_freq >= DAC_MAX))
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

	/* Configure internal PLL */
	for(i= 0; i < DAC_MAX_TILE; i++) {
		ret =  XRFdc_DynamicPLLConfig(&RFdcInst, 1, i, 1, 245.76, 3932.16);
	}

	return SUCCESS;

err:
	return FAIL;
}
