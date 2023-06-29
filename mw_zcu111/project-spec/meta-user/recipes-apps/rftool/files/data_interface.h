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
#include "board.h"
#include "cmd_interface.h"
#include "common.h"
#include "gpio.h"
#include "rfdc_functions_w.h"
#include "tcp_interface.h"
#include "xrfdc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************** Constant Definitions *****************************/
/***************** Macros (Inline Functions) Definitions *********************/
#define RFDC_DATA_ADC_ALIGN (256 * 1024)
#define RFDC_DATA_ADC_BETA_ALIGN (1024 * 1024)
#define FIFO_SIZE (64 * 1024)

#define DAC_ADC_MAP_SZ (1024 * 1024 * 1024)
#define DAC_MAP_SZ (DAC_ADC_MAP_SZ / 8)
#define ADC_MAP_SZ (DAC_ADC_MAP_SZ / 8)
#define ADC_DAC_DMA_SZ_ALIGNMENT 32

#define INVAL_ARGS (-2)
#define MEM_DIS_FAIL (-3)
#define MEM_EN_FAIL (-4)
#define MEM_INIT_FAIL (-5)
#define EN_FIFO_FAIL (-6)
#define RST_FIFO_FAIL (-7)
#define DMA_RX_TRIGGER_FAIL (-8)
#define DMA_TX_TRIGGER_FAIL (-9)
#define DIS_FIFO_FAIL (-10)
#define EN_IL_CONFIG_FAIL (-11)
#define SND_SAMPLE_FAIL (-12)
#define RCV_SAMPLE_FAIL (-13)
#define GPIO_SET_FAIL (-14)
#define MTS_FAIL (-15)

#define FIFO_EN 1
#define FIFO_DIS 0

#define MAX_ADC 8
#define MAX_DAC 8
#define DAC_MUX_GPIOS 4
#define ADC_MUX_GPIOS 3
#define MAX_STRLEN 512
#define RFDC_DATA_ALIGN (32 * 1024 * 1024)

#define DAC_MULTITILE_CTL_GPIO 484
#define DAC_MULTITILE_CTL_GPIO_STR "/sys/class/gpio/gpio484/"

#define ADC_MULTITILE_CTL_GPIO 500
#define ADC_MULTITILE_CTL_GPIO_STR "/sys/class/gpio/gpio500/"

#define DAC_SSR_CTL_GPIO 507
#define ADC_SSR_CTL_GPIO 506

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

enum MEM_TYPE { NO_MEM, PL_MEM, PS_MEM };

enum BRAM_DDR_TYPE { DDR, BRAM, MAX };

enum DEV_TYPE { ADC = 0, DAC = 1 };

enum DESIGN_TYPE { NON_MTS = 1, MTS = 2, DAC1_ADC1 = 3 };

/************************** Variable Definitions *****************************/

struct rfsoc_info {
  signed char *map_dac[MAX_DAC];
  signed char *map_adc[MAX_ADC];
  void *map_scratchreg_dac;
  void *map_scratchreg_adc;
  void *map_design_type_reg;
  int fd_dac[MAX_DAC];
  int fd_adc[MAX_ADC];
  int fd_scratchpad;
  int adc_status, dac_status;
  int mts_enable_dac;
  int mts_enable_adc;
  unsigned int mem_type;
  u32 scratch_value_dac;
  u32 scratch_value_adc;
  u32 channel_size;
  int invalid_size;
  int first_read;
  u32 dac_ssrgpio_val;
  u32 adc_ssrgpio_val;
  u32 adc_channels;
  u32 dac_channels;
  u32 design_type;
};

struct rfsoc_channel_info {
  u32 num_channels;
  u32 channel[8];
  u32 channel_size[8];
};

/************************** Extern Definitions *****************************/
extern struct rfsoc_info info;
extern struct rfsoc_channel_info dac_chaninfo;
extern int adc_localstart_gpio[MAX_ADC];
extern int adc_reset_gpio[MAX_ADC];
extern int dac_localstart_gpio[MAX_DAC];
extern int dac_loopback_gpio[MAX_DAC];
extern int dac_reset_gpio[MAX_DAC];
extern int dac_select_gpio[DAC_MUX_GPIOS];
/************************** Function Prototypes ******************************/
/****************************************************************************/
/**
*
* This function recieves data from client, updates DAC memory content
* and triggers DMA.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void WriteDataToMemory(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function triggers DMA, updates ADC memory content
* and send the data to client.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void ReadDataFromMemory(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function cleans-up all the resources used by rftool
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void disconnect(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets the current memory type
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetMemtype(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function sets the specified memory type
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetMemtype(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function initialises the resources required by rftool for data path
*
* @param	None
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int initDataPath(void);

/****************************************************************************/
/**
*
* This function enables/disables RFDC FIFO
*
* @param	fifo id(DAC/ADC)
* @param	tile id
* @param	enable/disable
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int change_fifo_stat(int fifo_id, int tile_id, int stat);

/****************************************************************************/
/**
*
* This function enables/disables RFDC FIFO for all tiles
*
* @param	fifo id(DAC/ADC)
* @param	enable/disable
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int config_all_fifo(int fifo_id, int enable);

/****************************************************************************/
/**
*
* This function de-initialises the resources required by rftool for data path
*
* @param	None
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void deinitDataPath(void);

/****************************************************************************/
/**
*
* This function is a thread callback function, starts data path and waits for
* commands from client.
*
* @param	None
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void *datapath_t(void *args);

/****************************************************************************/
/**
*
* This function is intended to reset ADC or DAC pipe
*
* @param	Reset GPIO array pointer
* @param	localstart GPIO array pointer
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/

int reset_pipe(int *reset_gpio, int *localstart_gpio);
#endif /* SRC_DATA_INTERFACE_ */
