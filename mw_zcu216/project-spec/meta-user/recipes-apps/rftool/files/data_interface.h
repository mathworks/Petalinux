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

#ifndef SRC_DATA_INTERFACE_
#define SRC_DATA_INTERFACE_

/***************************** Include Files *********************************/
#include "board.h"
#include "common.h"

/************************** Constant Definitions *****************************/
/***************** Macros (Inline Functions) Definitions *********************/
#define DAC_ADC_MAP_SZ (1024 * 1024 * 1024)
#define DAC_MAP_SZ                                                             \
	(DAC_ADC_MAP_SZ / 8) /* For DAC each partition size is 128MB */
#define ADC_MAP_SZ                                                             \
	(DAC_ADC_MAP_SZ / 8) /* For ADC each partition size is 128MB */
#define ADC_DAC_SZ_ALIGNMENT 32
/* ADC_DMABUF_SKIP_AREA and ADC_DDR_LATENCY_SKIP are hardware design (xsa)
 * specific values */
#ifdef XPS_BOARD_ZCU208
#define ADC_DMABUF_SKIP_AREA (30720)
#define SAMPLE_DMA_ALIGN 192
#define SAMPLE_ALIGN 48
#else
#define ADC_DMABUF_SKIP_AREA (32 * 1024)
#endif
#define FIFO_SIZE (16 * 1024 * 2)

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
#define DMA_RESET_FAIL (-16)

#define FIFO_EN 1
#define FIFO_DIS 0

#define MAX_ADC 16
#define MAX_DAC 16
#define MAX_STRLEN 512

#define MAX_TILE_ID (4)
#ifdef XPS_BOARD_ZCU208
#define MAX_BLOCK_ID (2)
#else
#define MAX_BLOCK_ID (4)
#endif

#define DAC_TILE_AXI_SWITCH_BASE_ADDR 0xA0201000
#define DAC_TILE0_CHANNEL_AXI_SWITCH_BASE_ADDR 0xA0202000
#define DAC_TILE1_CHANNEL_AXI_SWITCH_BASE_ADDR 0xA0203000
#define DAC_SOURCE_I_BASEADDR 0xB0000000
#define ADC_SINK_I_BASEADDR 0xB0400000
#define CLK_WIZ_ADC0_BASEADDR 0xB4C40000
#define CLK_WIZ_ADC1_BASEADDR 0xB4C50000
#define CLK_WIZ_ADC2_BASEADDR 0xB4C60000
#define CLK_WIZ_ADC3_BASEADDR 0xB4C70000
#define CLK_WIZ_DAC0_BASEADDR 0xB4C00000
#define CLK_WIZ_DAC1_BASEADDR 0xB4C10000
#define CLK_WIZ_DAC2_BASEADDR 0xB4C20000
#define CLK_WIZ_DAC3_BASEADDR 0xB4C30000
#define ADC_CHANNEL_SELECT_AXI_SWITCH_BASE_ADDR 0xA0204000
#define RD_WR_BUF 32768

enum PL_STATUS { PL_NOT_READY, PL_READY };
enum MEM_TYPE { NO_MEM, PL_MEM, PS_MEM };

enum BRAM_DDR_TYPE { DDR, BRAM, MAX };

enum DEV_TYPE { ADC = 0, DAC = 1, MAX_DEV_TYPE = 2 };

/************************** Variable Definitions *****************************/

struct rfsoc_info {
	signed char *map_dac[MAX_DAC];
	signed char *map_adc[MAX_ADC];
	int fd_dac[MAX_DAC];
	int fd_adc[MAX_ADC];
	unsigned int channel_size[MAX_DAC];
	int adc_status, dac_status;
	unsigned int mem_type_dac;
	unsigned int mem_type_adc;
	u32 adc_channels;
	u32 dac_channels;
	int fd;
	u32 paddr_adc;
	u32 paddr_dac;
	void *vaddr_adc;
	void *vaddr_dac;
	void *base_adc;
	void *base_dac;
	void *clk_wiz_dac0;
	void *clk_wiz_dac1;
	void *clk_wiz_dac2;
	void *clk_wiz_dac3;
	void *clk_wiz_adc0;
	void *clk_wiz_adc1;
	void *clk_wiz_adc2;
	void *clk_wiz_adc3;
	u32 mts_enable_dac_mask;
	u32 mts_enable_adc_mask;
};
/************************** Extern Definitions *****************************/
extern struct rfsoc_info info;
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
* This function will enable all the interrupts in DAC and ADC Tiles
*
* @param	None
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void EnableAllInterrupts();

int init_mem();
int deinit_mem();

#endif /* SRC_DATA_INTERFACE_ */
