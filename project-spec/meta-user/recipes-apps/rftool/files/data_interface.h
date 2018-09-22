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

#ifndef SRC_DATA_INTERFACE_
#define SRC_DATA_INTERFACE_

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfdc_functions_w.h"
#include "tcp_interface.h"
#include "xrfdc.h"
#include "board.h"
#include "common.h"
#include "cmd_interface.h"

/************************** Constant Definitions *****************************/
/***************** Macros (Inline Functions) Definitions *********************/
#define RFDC_DATA_ADC_ALIGN (256 * 1024)
#define FIFO_SIZE (128 * 1024)

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_REL    "/sys/class/gpio/unexport"

#define DAC_ADC_MAP_SZ (1024 * 1024 * 1024)
#define DAC_MAP_SZ 	(DAC_ADC_MAP_SZ / 8)
#define ADC_MAP_SZ 	(DAC_ADC_MAP_SZ / 8)
#define ADC_DAC_DMA_SZ_ALIGNMENT 32

#define INVAL_ARGS	 			(-1)
#define MEM_DIS_FAIL 			(-2)
#define MEM_EN_FAIL 			(-3)
#define MEM_INIT_FAIL 			(-4)
#define EN_FIFO_FAIL 			(-5)
#define RST_FIFO_FAIL 			(-6)
#define DMA_RX_TRIGGER_FAIL 	(-7)
#define DMA_TX_TRIGGER_FAIL 	(-8)
#define DIS_FIFO_FAIL 			(-9)
#define EN_IL_CONFIG_FAIL 		(-10)
#define SND_SAMPLE_FAIL 		(-11)
#define RCV_SAMPLE_FAIL 		(-12)
#define GPIO_SET_FAIL 			(-13)

#define FIFO_EN			1
#define FIFO_DIS		0

enum MEM_TYPE {
	NO_MEM,
	PL_MEM,
	PS_MEM
};

enum DEV_TYPE {
	ADC = 0,
	DAC = 1
};

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
void WriteDataToMemory(convData_t *cmdVals, char *txstrPtr, int *status);
void ReadDataFromMemory(convData_t *cmdVals, char *txstrPtr, int *status);
void disconnect(convData_t *cmdVals, char *txstrPtr, int *status);	
int initDataPath(void);
int change_fifo_stat(int fifo_id, int tile_id, int stat);
int disable_all_fifo(int fifo_id);
void deinitDataPath(void);
void *datapath_t(void *args);

#endif /* SRC_DATA_INTERFACE_ */
