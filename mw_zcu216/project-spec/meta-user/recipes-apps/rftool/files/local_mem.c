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
#include "local_mem.h"
#include "cmd_interface.h"
#include "tcp_interface.h"
#include "data_interface.h"
#include "rfdc_interface.h"
#include "gpio.h"
#include "gpio_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/
#define MEM_SIZE_MASK (0x00FFFFFF)
#define MEM_SIZE_SHIFT (0)
#define MEM_NUM_MASK (0x7f)
#define MEM_NUM_SHIFT (24)
#define EFFECTIVE_FS_LIMIT (4500.0)

/***************** Macros (Inline Functions) Definitions *********************/
int LocalMemTriggerHw(u32 type, void *memBaseAddr, u8 clk_sel, u32 numsamples,
		      u32 rfdc_ch);
void GetMemInfoHw(u32 type, void *mem, u32 *num_tiles, u32 *num_mem,
		  u32 *mem_size, int *num_words, int *mem_enable,
		  int *mem_clksel);

/************************** Variable Definitions *****************************/
extern int enTermMode; /* Enable printing in terminal mode */
extern XRFdc RFdcInst;

/* Get local Mem Info bridge */

void LocalMemInfo(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 numTiles; /* value of parameter tiles set on the stimulus gen
	 or data capture IPI */
	u32 numMem; /* number of stim gen or data capture memory channels */
	u32 memSize; /* stim gen or data capture memory channel size */
	int numWords; /* streaming interface number of words per clk cycle */
	void *memBaseAddr; /* AXI Base address */
	int mem_enable;
	int mem_clksel;
	u32 type = cmdVals[0].u;
	unsigned int mem_type;
	char Response[BUF_MAX_LEN] = {
		0
	}; /* hold command response to return to the host */
	u32 pmemBaseAddr;

	/* Need to add tile_id for localmeminfo */
	if (type == DAC)
		mem_type = info.mem_type_dac;
	else
		mem_type = info.mem_type_adc;

	if (mem_type != DDR) {
		/* open memory file */

		/* select either stim gen or data capture base address based
		 on cmd argument from the host */
		if (type == ADC) {
			memBaseAddr = info.vaddr_adc;
			pmemBaseAddr = info.paddr_adc;
		} else {
			memBaseAddr = info.vaddr_dac;
			pmemBaseAddr = info.paddr_dac;
		}
		/* get stimulus or data capture mem info */
		GetMemInfoHw(type, memBaseAddr, &numTiles, &numMem, &memSize,
			     &numWords, &mem_enable, &mem_clksel);
		*status = SUCCESS;
		if (*status == SUCCESS) {
			sprintf(Response, " %d %x %u %u %u %d %d %d",
				cmdVals[0].i, pmemBaseAddr, numTiles,
				numMem * 2, memSize, numWords, mem_enable,
				mem_clksel);
			strncat(txstrPtr, Response, BUF_MAX_LEN);
		}
	} else {
		if (cmdVals[0].i == 0) {
			numTiles = 4;
			memBaseAddr = (void *)82000000;
			mem_enable = 0;
			mem_clksel = 0;
		} else {
			numTiles = 4;
			memBaseAddr = (void *)86000000;
			mem_enable = 0;
			mem_clksel = 1;
		}

		numMem = 8;
		memSize = (128 * 1024);
		numWords = 16;

		*status = SUCCESS;
		if (*status == SUCCESS) {
			sprintf(Response, " %d %lx %u %u %u %d %d %d",
				cmdVals[0].i, (uintptr_t)memBaseAddr, numTiles,
				numMem * 2, memSize, numWords, mem_enable,
				mem_clksel);
			strncat(txstrPtr, Response, BUF_MAX_LEN);
		}
	}
}

/* Function is returning the address for each channel */
void LocalMemAddr(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 type;
	int tile_id;
	u32 block_id;
	u32 Addr_I;
	u32 Addr_Q;
	char Response[BUF_MAX_LEN] = { 0 };
	u16 numblockpertile = 4;
	unsigned int mem_type;

	type = cmdVals[0].u;
	tile_id = cmdVals[1].i;
	block_id = cmdVals[2].u;

	if (type == DAC)
		mem_type = ((info.mem_type_dac & (1 << tile_id)) != 0);
	else
		mem_type = ((info.mem_type_adc & (1 << tile_id)) != 0);
	/* In DDR mode this is a dummy function, so don't do anything */
	if (mem_type != BRAM) {
		*status = SUCCESS;
		return;
	}

	if (type == DAC) {
		Addr_I = DacMap[tile_id * 4 + block_id].addr_I;
		Addr_Q = DacMap[tile_id * 4 + block_id].addr_Q;
	} else {
		if (XRFdc_IsADC4GSPS(&RFdcInst)) {
			numblockpertile = 2;
		}
		Addr_I = AdcMap[tile_id * numblockpertile + block_id].addr_I;
		Addr_Q = AdcMap[tile_id * numblockpertile + block_id].addr_Q;
	}

	if (Addr_I == 0xffffffff) {
		*status = FAIL;
		metal_log(METAL_LOG_ERROR, "\nChannel does not exist \r\n");
	} else {
		*status = SUCCESS;
		sprintf(Response, " %u %d %u %8X %8X ", type, tile_id, block_id,
			Addr_I, Addr_Q);
		strncat(txstrPtr, Response, BUF_MAX_LEN);
	}
}
/*
 *  Get local Memory info the Returns the Stim_Cap AXI base address memory,
 *  the number of tile connected to the Stim or Cap , the number of channels,
 *  memory channel size and axi-stream interface width the data converter
 */

void GetMemInfoHw(u32 type, void *mem, u32 *num_tiles, u32 *num_mem,
		  u32 *mem_size, int *num_words, int *mem_enable,
		  int *mem_clksel)
{
	int i = 0;
	u32 data;
	data = lmem_rd32(mem + LMEM_INFO);
	u32 DcChEnabled = 0x0;
	u16 *MemChMap;
	u32 FabWords;

	if (type == ADC) {
		MemChMap = AdcMemMap;
		XRFdc_GetFabRdVldWords(&RFdcInst, ADC, 0, 0, &FabWords);
	} else {
		MemChMap = DacMemMap;
		XRFdc_GetFabWrVldWords(&RFdcInst, DAC, 0, 0, &FabWords);
	}

	*num_words = FabWords;
	*num_mem = (data >> MEM_NUM_SHIFT) & MEM_NUM_MASK; /* (31:24) */
	if (type == ADC)
		*num_tiles = 4;
	else
		*num_tiles = !XRFdc_IsADC4GSPS(&RFdcInst) * 2 + 2;

	*mem_size = data & MEM_SIZE_MASK; /* (23:0) */
	*mem_size = *mem_size / (*num_mem + 1) / 16;

	data = lmem_rd32(mem + LMEM_ENABLE);
	for (i = 0; i < 16; i++)
		if ((data >> i) & 0x1)
			DcChEnabled |= 0x01 << MemChMap[i];

	*mem_enable =
		DcChEnabled; /* 16 LSB (16 memories) indicate which one are enabled. */
	*mem_clksel = 0;
}

void LocalMemTrigger_bram(u32 type, u32 tile_id, u32 block_id, u32 clksel,
			  u32 numsamples, u32 rfdc_ch, int *status)
{
	void *memBaseAddr; /* AXI Base address */
	u32 interrupt[16];
	int i = 0;

	if (type == ADC) {
		memBaseAddr = info.vaddr_adc;
	} else {
		memBaseAddr = info.vaddr_dac;
	}

	XRFdc_IntrClr(&RFdcInst, type, tile_id, block_id, 0xffffffff);
	usleep(1);
	XRFdc_GetIntrStatus(&RFdcInst, type, tile_id, block_id, &interrupt[i]);
	if ((0x80000003 & interrupt[i]) != 0x00000000) {
		if (type == ADC) {
			metal_log(METAL_LOG_WARNING,
				  "FIFO under/overflow on ADC Tile"
				  " %d Block %d, Fabric frequency"
				  " probably incorrect: check I/Q, decimation"
				  " and Fabric width\r\n",
				  tile_id, block_id);
		} else {
			metal_log(METAL_LOG_WARNING,
				  "FIFO under/overflow on DAC Tile"
				  " %d Block %d, Fabric frequency"
				  " probably incorrect: check I/Q, "
				  "interpolation and Fabric width\r\n",
				  tile_id, block_id);
		}
		*status |= WARN_EXECUTE;
		/* removing marginal FIFO over/underflow */
	} else if ((0xfffffff2 & interrupt[i]) != 0x00000000) {
		metal_log(METAL_LOG_WARNING,
			  "interrupt 0x%x on Tile %d Block %d, please check "
			  "interrupt screen for details\r\n",
			  interrupt[i], tile_id, block_id);
		*status |= WARN_EXECUTE;
	}
	*status |= LocalMemTriggerHw(type, memBaseAddr, clksel, numsamples,
				     rfdc_ch);
}

/* Function is a bridge that calls localMemTriggerHw */

void LocalMemTrigger(convData_t *cmdVals, char *txstrPtr, int *status)
{
	(void)txstrPtr;
	u32 type = cmdVals[0].u;
	u32 clksel = cmdVals[1].u;
	u32 numsamples = cmdVals[2].u;
	u32 rfdc_ch = cmdVals[3].u;

	u32 tile_id;
	u32 block_id;
	u32 i;
	int ret;
	int gpio_index;
	unsigned int mem_type;
	u32 start_dma = 1;

	*status = SUCCESS;

	if (type == DAC) {
		/*
		 * channel mask is 0, so it is stop command.
		 * Reset the pipies and clean-up DAC/ADC.
		 */
		if (rfdc_ch == 0 && (info.mem_type_dac != 0xF)) {
			/* Reset DMA */
			fsync((info.fd_dac[0]));
			*status = SUCCESS;
			return;
		}
		start_dma = 1;
		/* Enable dac select gpio */
		for (i = 0; i < MAX_DAC; i++) {
			if ((rfdc_ch >> i) & 0x01) {
				tile_id = (i / MAX_DAC_TILE);
				block_id = (i % MAX_DAC_TILE);
				mem_type = ((info.mem_type_dac &
					     (1 << tile_id)) != 0);

				if (mem_type == DDR) {
					gpio_index = i / (MAX_DAC_TILE /
							  MAX_DAC_PER_TILE);
					ret = set_gpio(
						dac_userselect_gpio[gpio_index],
						1);
					if (ret) {
						ret = GPIO_SET_FAIL;
						MetalLoghandler_firmware(
							ret, "Unable to set DAC"
							     " userselect GPIO"
							     " value\n");
						goto err;
					}
					/* Enable RFDC FIFO */
					ret = change_fifo_stat(DAC, tile_id,
							       FIFO_EN);
					if (ret != SUCCESS) {
						ret = EN_FIFO_FAIL;
						MetalLoghandler_firmware(
							ret, "Unable to enable"
							     " FIFO\n");
						goto err;
					}
					if (start_dma) {
						/* Trigger DMA */
						ret = write(
							(info.fd_dac[i]),
							info.map_dac[i],
							((info.channel_size[i]) *
							 sizeof(signed char)));
						if (ret < 0) {
							ret = DMA_TX_TRIGGER_FAIL;
							MetalLoghandler_firmware(
								ret,
								"Error in "
								"writing data"
								"\n");
							goto err;
						}
						start_dma = 0;
					}
				} else {
					LocalMemTrigger_bram(type, tile_id,
							     block_id, clksel,
							     numsamples,
							     rfdc_ch, status);
				}
			}
		}
	} else {
		for (i = 0; i < MAX_ADC; i++) {
			if ((rfdc_ch >> i) & 0x01) {
				tile_id = (i / MAX_ADC_PER_TILE);
				block_id = (i % MAX_ADC_PER_TILE);
				mem_type = ((info.mem_type_adc &
					     (1 << tile_id)) != 0);
				if (mem_type == DDR) {
					continue;
				} else {
					LocalMemTrigger_bram(type, tile_id,
							     block_id, clksel,
							     numsamples,
							     rfdc_ch, status);
				}
			}
		}
	}

	if (enTermMode) {
		if (*status != SUCCESS) {
			printf("localMemTrigger() failed\n\r");
		} else {
			printf("    **********localMemTrigger()"
			       "***********\r\n");
			printf("    Local mem type (input from host):   %d\r\n",
			       cmdVals[0].i);
			printf("    Local mem clk sel:   %d\r\n", cmdVals[1].u);
			printf("    Local mem num samples:  %d\r\n",
			       cmdVals[2].u);
			printf("    Local mem rfdc channels trigger:   %d\r\n",
			       rfdc_ch);
		}
	}
	return;
err:
	*status = FAIL;
}

/*
 * Sets the end-address (size) of the data to run,
 * then enables  the list of channels (bitmask/one hot encoded) and runs
 * for either the Stim/Capture memories
 */
int LocalMemTriggerHw(u32 type, void *memBaseAddr, u8 clk_sel, u32 numsamples,
		      u32 rfdc_ch)
{
	(void)clk_sel;
	(void)numsamples;
	u32 i, j = 0;
	struct Channel_Map *chMap;
	u32 mem_ids = 0x00000000;
	int status = SUCCESS;
	u8 Tile_Id = 0;
	u8 num_block;
	void *reg_addr;

	if (type == XRFDC_ADC_TILE) {
		chMap = AdcMap;
	} else {
		chMap = DacMap;
	}

	/* disable all channels*/
	lmem_wr32(memBaseAddr + LMEM_ENABLE_TILE, 0);
	lmem_wr32(memBaseAddr + LMEM_ENABLE, 0);
	for (i = 0; i < 16; i++) {
		if (type == XRFDC_ADC_TILE) {
			num_block = RFdcInst.RFdc_Config.ADCTile_Config[Tile_Id]
					    .NumSlices;
		} else {
			num_block = 4;
		}
		if (j == num_block) {
			Tile_Id++;
			j = 0;
		}
		if ((rfdc_ch >> i) & 0x1) {
			if ((type == XRFDC_ADC_TILE) &&
			    (XRFdc_IsHighSpeedADC(&RFdcInst, Tile_Id))) {
				/* enable correct mem channel */
				mem_ids |=
					lmem_rd32(memBaseAddr + LMEM_ENABLE) |
					(0x01 << (chMap[i].Channel_Q));
			}
			mem_ids |= lmem_rd32(memBaseAddr + LMEM_ENABLE) |
				   (0x01 << (chMap[i].Channel_I));
		}
		j++;
	}
	/* Enable memories based on mem_ids bit-mask (clear then set) */
	reg_addr = memBaseAddr + LMEM_ENABLE;
	lmem_wr32(reg_addr, mem_ids);
	if (enTermMode) {
		printf("Mem Enables: %d -> %8lx\n", mem_ids,
		       (uintptr_t)reg_addr);
	}
	/* Issue the trigger */
	reg_addr = memBaseAddr + LMEM_TRIGGER;
	lmem_wr32(reg_addr, 0x1);
	if (enTermMode) {
		printf("Mem Trigger: %d -> %8lx\n", 1, (uintptr_t)reg_addr);
	}

	reg_addr = memBaseAddr + LMEM_ENABLE_TILE;
	lmem_wr32(reg_addr, 0xF);
	if (enTermMode) {
		printf("Mem Trigger: %d -> %8lx\n", 1, (uintptr_t)reg_addr);
	}

	return status;
}

/* Function is setting the number of sample on a channel */
void SetLocalMemSample(convData_t *cmdVals, char *txstrPtr, int *status)
{
	(void)txstrPtr;
	u32 type = cmdVals[0].u;
	int tile_id = cmdVals[1].i;
	u32 block_id = cmdVals[2].u;
	u32 numsamples = cmdVals[3].u;
	u32 num_mem = 0;
	u32 ret = 0;
	int num_words = 0;
	int mem_enable = 0;
	int mem_clksel = 0;
	u32 mem_size = 0;
	void *reg_addr;
	u32 num_tiles = 0;
	struct Channel_Map *chMap;
	u32 numblockpertile;
	u32 FifoWidth = 0;
	u32 numsamples_channel = numsamples;
	XRFdc_Mixer_Settings Mixer_Settings;
	void *memBaseAddr;
	unsigned int mem_type;
	u32 DataPathMode = 0;
	u32 InterDecim = 0;
	double SampleRate;
	double Effective_FS = 0;
	XRFdc_PLL_Settings PLLSettings;
	double DataIQ = 1.0;
	double Max_Effective_Fs_Limit = EFFECTIVE_FS_LIMIT;

	*status = SUCCESS;
	if (type == DAC)
		mem_type = ((info.mem_type_dac & (1 << tile_id)) != 0);
	else
		mem_type = ((info.mem_type_adc & (1 << tile_id)) != 0);

	if (DDR == mem_type) {
		if (type == DAC) {
			ret = XRFdc_GetDataPathMode(&RFdcInst, tile_id,
						    block_id, &DataPathMode);
			if (FAIL == ret) {
				metal_log(METAL_LOG_ERROR,
					  "XRFdc_GetDataPathMode"
					  " failed with ret = %u\n",
					  ret);
				*status |= FAIL;
			}
			ret = XRFdc_GetInterpolationFactor(
				&RFdcInst, tile_id, block_id, &InterDecim);
			if (FAIL == ret) {
				metal_log(METAL_LOG_ERROR,
					  "XRFdc_GetInterpolationFactor"
					  " failed with ret = %u\n",
					  ret);
				*status |= FAIL;
			}
		} else {
			DataPathMode = 1;
			ret = XRFdc_GetDecimationFactor(&RFdcInst, tile_id,
							block_id, &InterDecim);
			if (FAIL == ret) {
				metal_log(METAL_LOG_ERROR,
					  "XRFdc_GetDecimationFactor"
					  " failed with ret = %u\n",
					  ret);
				*status |= FAIL;
			}
		}
		if (2 == DataPathMode || 3 == DataPathMode) {
			InterDecim = 2 * InterDecim;
		}
		ret = XRFdc_GetPLLConfig(&RFdcInst, type, tile_id,
					 &PLLSettings);
		if (FAIL == ret) {
			metal_log(METAL_LOG_ERROR,
				  "XRFdc_GetDecimationFactor "
				  " failed with ret = %u\n",
				  ret);
			*status |= FAIL;
		}
		SampleRate = 1000 * PLLSettings.SampleRate;
		Effective_FS = SampleRate / (double)InterDecim;
		*status |= XRFdc_GetMixerSettings(&RFdcInst, type, tile_id,
						  block_id, &Mixer_Settings);
		if (type == XRFDC_DAC_TILE) {
			if ((4 == DataPathMode) ||
			    (Mixer_Settings.MixerMode ==
			     XRFDC_MIXER_MODE_R2R) ||
			    (Mixer_Settings.MixerMode ==
			     XRFDC_MIXER_MODE_R2C) ||
			    (Mixer_Settings.MixerType ==
				     XRFDC_MIXER_TYPE_COARSE &&
			     Mixer_Settings.CoarseMixFreq ==
				     XRFDC_COARSE_MIX_BYPASS)) {
				DataIQ = 1.0;
			} else {
				DataIQ = 2.0;
			}
		} else {
			if (!XRFdc_IsHighSpeedADC(&RFdcInst, tile_id) &&
			    ((Mixer_Settings.MixerMode ==
			      XRFDC_MIXER_MODE_R2R) ||
			     ((Mixer_Settings.MixerType ==
			       XRFDC_MIXER_TYPE_COARSE) &&
			      (Mixer_Settings.CoarseMixFreq ==
			       XRFDC_COARSE_MIX_BYPASS)))) {
				DataIQ = 1.0;
			} else {
				DataIQ = 2.0;
			}
		}
		Effective_FS = SampleRate / (double)InterDecim;
		Max_Effective_Fs_Limit /= DataIQ;
		if (Effective_FS > Max_Effective_Fs_Limit) {
			metal_log(METAL_LOG_ERROR,
				  " Effective FS is greater than %0.2lf GHz"
				  " for Tile %d Block %d\r\n",
				  Max_Effective_Fs_Limit / 1000, tile_id,
				  block_id);
			*status |= FAIL;
		}
		return;
	}
	if (mem_type != BRAM) {
		return;
	}
	num_tiles = 4;
	if (type == ADC) {
		memBaseAddr = info.vaddr_adc;
		chMap = AdcMap;
		numblockpertile =
			(!XRFdc_IsHighSpeedADC(&RFdcInst, tile_id) << 1) + 2;
		XRFdc_GetFabRdVldWords(&RFdcInst, type, tile_id, block_id,
				       &FifoWidth);
	} else {
		memBaseAddr = info.vaddr_dac;

		chMap = DacMap;
		numblockpertile = 4;
		XRFdc_GetFabWrVldWords(&RFdcInst, type, tile_id, block_id,
				       &FifoWidth);
	}

	GetMemInfoHw(type, memBaseAddr, &num_tiles, &num_mem, &mem_size,
		     &num_words, &mem_enable, &mem_clksel);
	/* Calculate numsamples according to IQ mode for 2Gsps */
	if (type == ADC && !XRFdc_IsHighSpeedADC(&RFdcInst, tile_id)) {
		XRFdc_GetMixerSettings(&RFdcInst, type, tile_id, block_id,
				       &Mixer_Settings);
		if (!(Mixer_Settings.CoarseMixFreq == XRFDC_COARSE_MIX_BYPASS &&
		      Mixer_Settings.MixerType == XRFDC_MIXER_TYPE_COARSE)) {
			numsamples_channel = numsamples * 2;
		}
	}
	/* Check we are not trying to overflow the memory */
	if (numsamples_channel > mem_size) {
		metal_log(
			METAL_LOG_ERROR,
			"Total sample size is limited to %d in Real or %d in"
			" IQ for Tile Id %d Block Id %d, try changing to real "
			"mode or increase the memory size in the bitstream\r\n",
			mem_size, mem_size >> 1, tile_id, block_id);
		*status |= FAIL;
	}
	if (numsamples_channel != ((u32)(numsamples_channel / 16)) * 16) {
		metal_log(METAL_LOG_ERROR,
			  "Sample size must be a multiple of 16 for Tile Id %d"
			  " Block Id %d\r\n",
			  tile_id, block_id);
		*status |= FAIL;
	}

	if (numsamples_channel !=
	    ((u32)(numsamples_channel / FifoWidth)) * FifoWidth) {
		metal_log(METAL_LOG_WARNING,
			  "Sample size must be a multiple of the fifo width %d"
			  " for Tile Id %d Block Id %d\r\n",
			  FifoWidth, tile_id, block_id);
		*status |= WARN_EXECUTE;
	}

	/* Write the registers */
	reg_addr =
		memBaseAddr + LMEM0_ENDADDR +
		(chMap[(tile_id * numblockpertile) + block_id].Channel_I * 4);
	lmem_wr32(reg_addr, numsamples_channel);
	if (type == ADC && XRFdc_IsHighSpeedADC(&RFdcInst, tile_id)) {
		reg_addr =
			memBaseAddr + LMEM0_ENDADDR +
			(chMap[tile_id * numblockpertile + block_id].Channel_Q *
			 4);
		lmem_wr32(reg_addr, numsamples_channel);
	}
	return;
}
