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
#include "gpio.h"
#include "gpio_interface.h"
#include "data_interface.h"
#include "cmd_interface.h"
#include "tcp_interface.h"
#include "error_interface.h"
#include "rfdc_interface.h"
#include "axi_switch.h"
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************** Constant Definitions *****************************/

#ifdef XPS_BOARD_ZCU208
int dac_userselect_gpio[(MAX_DAC_PER_TILE * MAX_DAC_TILE)] = {
	503, 504, 505, 506, 507, 508, 509, 510,
};
int dac_mts_clk_en[MAX_DAC_TILE] = {
	495,
	496,
	497,
	498,
};
int adc_mts_clk_en[MAX_ADC_TILE] = {
	499,
	500,
	501,
	502,
};
int adc_axiswitchrst[1] = {
	511,
};
#else
int dac_userselect_gpio[(MAX_DAC_PER_TILE * MAX_DAC_TILE)] = {
	495, 496, 497, 498, 499, 500, 501, 502,
	503, 504, 505, 506, 507, 508, 509, 510,
};
int dac_mts_clk_en[MAX_DAC_TILE] = {
	487,
	488,
	489,
	490,
};
int adc_mts_clk_en[MAX_ADC_TILE] = {
	491,
	492,
	493,
	494,
};
int adc_axiswitchrst[1] = {
	511,
};
#endif

int init_gpio()
{
	u32 i, ret = 0;
	u32 max_dac;

	max_dac = MAX_DAC_PER_TILE * MAX_DAC_TILE;
	for (i = 0; i < max_dac; i++) {
		/* Enable/Export reset GPIO */
		ret = enable_gpio(dac_userselect_gpio[i]);
		if (ret) {
			printf("Unable to enable reset GPIO\n");
			return ret;
		}
		ret = config_gpio_op(dac_userselect_gpio[i]);
		if (ret) {
			printf("Unable to set direction for reset gpio of dac"
			       " %d\n",
			       i);
			return ret;
		}
	}
	for (i = 0; i < MAX_DAC_TILE; i++) {
		/* Enable/Export reset GPIO */
		ret = enable_gpio(dac_mts_clk_en[i]);
		if (ret) {
			printf("Unable to enable DAC clk GPIO\n");
			return ret;
		}
		ret = config_gpio_op(dac_mts_clk_en[i]);
		if (ret) {
			printf("Unable to set direction for clk gpio of dac"
			       " %d\n",
			       i);
			return ret;
		}
		ret = set_gpio(dac_mts_clk_en[i], 1);
		if (ret) {
			printf("Unable to set value for clk gpio of dac %d\n",
			       i);
			return ret;
			;
		}
	}

	for (i = 0; i < MAX_ADC_TILE; i++) {
		/* Enable/Export reset GPIO */
		ret = enable_gpio(adc_mts_clk_en[i]);
		if (ret) {
			printf("Unable to enable ADC clk GPIO\n");
			return ret;
		}
		ret = config_gpio_op(adc_mts_clk_en[i]);
		if (ret) {
			printf("Unable to set direction for clk gpio of adc"
			       " %d\n",
			       i);
			return ret;
		}
		ret = set_gpio(adc_mts_clk_en[i], 1);
		if (ret) {
			printf("Unable to set value for clk gpio of adc %d\n",
			       i);
			return ret;
		}
	}
	ret = enable_gpio(adc_axiswitchrst[0]);
	if (ret) {
		printf("Unable to enable adc axi switch reset GPIO\n");
		return ret;
	}
	ret = config_gpio_op(adc_axiswitchrst[0]);
	if (ret) {
		printf("Unable to set direction of adc axi swtich reset GPIO"
		       " %d\n",
		       i);
		return ret;
	}
	ret = set_gpio(adc_axiswitchrst[0], 1);
	if (ret) {
		printf("Unable to setup adc axi swtich reset GPIO\n");
		return ret;
	}
	return ret;
}

int deinit_gpio()
{
	u32 i, ret = 0;
	u32 max_dac;

	max_dac = MAX_DAC_PER_TILE * MAX_DAC_TILE;
	for (i = 0; i < max_dac; i++) {
		/* Release reset GPIO */
		ret = disable_gpio(dac_userselect_gpio[i]);
		if (ret) {
			MetalLoghandler_firmware(
				ret, "Unable to release reset GPIO of dac\n");
			return ret;
		}
	}
	for (i = 0; i < MAX_DAC_TILE; i++) {
		/* Release reset GPIO */
		ret = disable_gpio(dac_mts_clk_en[i]);
		if (ret) {
			MetalLoghandler_firmware(
				ret, "Unable to release reset GPIO of dac\n");
			return ret;
		}
	}
	for (i = 0; i < MAX_ADC_TILE; i++) {
		/* Release reset GPIO */
		ret = disable_gpio(adc_mts_clk_en[i]);
		if (ret) {
			MetalLoghandler_firmware(
				ret, "Unable to release reset GPIO of dac\n");
			return ret;
		}
	}
	ret = disable_gpio(adc_axiswitchrst[0]);
	if (ret) {
		MetalLoghandler_firmware(
			ret, "Unable to release adc axi switch reset GPIO\n");
		return ret;
	}
	return ret;
}
