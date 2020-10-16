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
#ifndef SRC_CLOCK_INTERFACE_
#define SRC_CLOCK_INTERFACE_

/***************************** Include Files *********************************/
#include "board.h"
#include "common.h"
#include "xrfdc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************** Constant Definitions *****************************/
#define PLL_A_PATH "/sys/bus/i2c/devices/12-002f/frequency2"
#define PLL_B_PATH "/sys/bus/i2c/devices/12-002f/frequency1"
#define PLL_C_PATH "/sys/bus/i2c/devices/12-002f/frequency"
#define PARENT_CLK_PATH "/sys/bus/i2c/devices/12-002f/parent_frequency"
/***************** Macros (Inline Functions) Definitions *********************/
enum parent_clock_freqs {
  LMK04208_12M8_3072M_122M88_REVA,
  LMK04208_12M8_3072M_122M88_REVAB,
  LMK04208_12M8_3072M_122M88_REVB,
  LMK04208_MAX
};

#define DAC_MAX_TILE    2
#define DAC_MAX_BLOCK   4

#define FOREACH_DACFREQ(DACFREQ)                                               \
  DACFREQ(DAC_102_40625_MHZ)                                                   \
  DACFREQ(DAC_204_8_MHZ)                                                       \
  DACFREQ(DAC_245_76_MHZ)                                                      \
  DACFREQ(DAC_409_6_MHZ)                                                       \
  DACFREQ(DAC_491_52_MHZ)                                                      \
  DACFREQ(DAC_737_28_MHZ)                                                      \
  DACFREQ(DAC_1474_56_MHZ)                                                     \
  DACFREQ(DAC_1966_08_MHZ)                                                     \
  DACFREQ(DAC_2048_MHZ)                                                        \
  DACFREQ(DAC_2457_6_MHZ)                                                      \
  DACFREQ(DAC_2949_12_MHZ)                                                     \
  DACFREQ(DAC_3072_MHZ)                                                        \
  DACFREQ(DAC_3194_88_MHZ)                                                     \
  DACFREQ(DAC_3276_8_MHZ)                                                      \
  DACFREQ(DAC_3686_4_MHZ)                                                      \
  DACFREQ(DAC_3932_16_MHZ)                                                     \
  DACFREQ(DAC_4096_MHZ)                                                        \
  DACFREQ(DAC_4423_68_MHZ)                                                     \
  DACFREQ(DAC_4669_44_MHZ)                                                     \
  DACFREQ(DAC_4915_2_MHZ)                                                      \
  DACFREQ(DAC_5734_4_MHZ)                                                      \
  DACFREQ(DAC_5898_24_MHZ)                                                     \
  DACFREQ(DAC_6144_MHZ)                                                        \
  DACFREQ(DAC_6389_76_MHZ)                                                     \
  DACFREQ(DAC_6400_MHZ)                                                        \
  DACFREQ(DAC_6553_6_MHZ)                                                      \
  DACFREQ(DAC_MAX)

#define FOREACH_ADC0FREQ(ADC0FREQ)                                             \
  ADC0FREQ(ADC_102_40625_MHZ)                                                  \
  ADC0FREQ(ADC_204_8_MHZ)                                                      \
  ADC0FREQ(ADC_245_76_MHZ)                                                     \
  ADC0FREQ(ADC_409_6_MHZ)                                                      \
  ADC0FREQ(ADC_491_52_MHZ)                                                     \
  ADC0FREQ(ADC_737_28_MHZ)                                                     \
  ADC0FREQ(ADC_1474_56_MHZ)                                                    \
  ADC0FREQ(ADC_1966_08_MHZ)                                                    \
  ADC0FREQ(ADC_2048_MHZ)                                                       \
  ADC0FREQ(ADC_2457_6_MHZ)                                                     \
  ADC0FREQ(ADC_2949_12_MHZ)                                                    \
  ADC0FREQ(ADC_3072_MHZ)                                                       \
  ADC0FREQ(ADC_3194_88_MHZ)                                                    \
  ADC0FREQ(ADC_3276_8_MHZ)                                                     \
  ADC0FREQ(ADC_3686_4_MHZ)                                                     \
  ADC0FREQ(ADC_3932_16_MHZ)                                                    \
  ADC0FREQ(ADC_4096_MHZ)                                                       \
  ADC0FREQ(ADC_MAX)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum DACFREQ_ENUM { FOREACH_DACFREQ(GENERATE_ENUM) };

enum ADC0FREQ_ENUM { FOREACH_ADC0FREQ(GENERATE_ENUM) };

static const char *DACFREQ_STRING[] = {FOREACH_DACFREQ(GENERATE_STRING)};

static const char *ADC0FREQ_STRING[] = {FOREACH_ADC0FREQ(GENERATE_STRING)};



enum pll_src { PLL_A = 0x8, PLL_B = 0x4, PLL_C = 0x1 };

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
int initRFclock(u32 board_id, u32 lmk_freq, u32 lmx0_freq, u32 lmx1_freq,
                u32 lmx2_freq);
int iic_device_write(u32 inst, u32 slave, u32 pll_src, u32 size, u32 data);

#endif /* SRC_CLOCK_INTERFACE_ */
