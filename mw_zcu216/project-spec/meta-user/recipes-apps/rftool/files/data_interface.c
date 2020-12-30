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

extern XRFdc RFdcInst; /* RFdc driver instance */
extern int enTermMode;
extern int thread_stat;
extern int adc_axiswitchrst[1];

char mem_path_dac[MAX_DAC][MAX_STRLEN] = {
	"/dev/plmem0",  "/dev/plmem1",  "/dev/plmem2",  "/dev/plmem3",
	"/dev/plmem4",  "/dev/plmem5",  "/dev/plmem6",  "/dev/plmem7",
	"/dev/plmem8",  "/dev/plmem9",  "/dev/plmem10", "/dev/plmem11",
	"/dev/plmem12", "/dev/plmem13", "/dev/plmem14", "/dev/plmem15",
};
char mem_path_adc[MAX_ADC][MAX_STRLEN] = {
	"/dev/plmem16", "/dev/plmem17", "/dev/plmem18", "/dev/plmem19",
	"/dev/plmem20", "/dev/plmem21", "/dev/plmem22", "/dev/plmem23",
	"/dev/plmem24", "/dev/plmem25", "/dev/plmem26", "/dev/plmem27",
	"/dev/plmem28", "/dev/plmem29", "/dev/plmem30", "/dev/plmem31",
};
char mem_type_path_dac[MAX_DAC][MAX_STRLEN] = {
	"/sys/class/plmem/plmem0/device/select_mem",
	"/sys/class/plmem/plmem1/device/select_mem",
	"/sys/class/plmem/plmem2/device/select_mem",
	"/sys/class/plmem/plmem3/device/select_mem",
	"/sys/class/plmem/plmem4/device/select_mem",
	"/sys/class/plmem/plmem5/device/select_mem",
	"/sys/class/plmem/plmem6/device/select_mem",
	"/sys/class/plmem/plmem7/device/select_mem",
	"/sys/class/plmem/plmem8/device/select_mem",
	"/sys/class/plmem/plmem9/device/select_mem",
	"/sys/class/plmem/plmem10/device/select_mem",
	"/sys/class/plmem/plmem11/device/select_mem",
	"/sys/class/plmem/plmem12/device/select_mem",
	"/sys/class/plmem/plmem13/device/select_mem",
	"/sys/class/plmem/plmem14/device/select_mem",
	"/sys/class/plmem/plmem15/device/select_mem",
};
char mem_type_path_adc[MAX_ADC][MAX_STRLEN] = {
	"/sys/class/plmem/plmem16/device/select_mem",
	"/sys/class/plmem/plmem17/device/select_mem",
	"/sys/class/plmem/plmem18/device/select_mem",
	"/sys/class/plmem/plmem19/device/select_mem",
	"/sys/class/plmem/plmem20/device/select_mem",
	"/sys/class/plmem/plmem21/device/select_mem",
	"/sys/class/plmem/plmem22/device/select_mem",
	"/sys/class/plmem/plmem23/device/select_mem",
	"/sys/class/plmem/plmem24/device/select_mem",
	"/sys/class/plmem/plmem25/device/select_mem",
	"/sys/class/plmem/plmem26/device/select_mem",
	"/sys/class/plmem/plmem27/device/select_mem",
	"/sys/class/plmem/plmem28/device/select_mem",
	"/sys/class/plmem/plmem29/device/select_mem",
	"/sys/class/plmem/plmem30/device/select_mem",
	"/sys/class/plmem/plmem31/device/select_mem",
};
char bram_ddr_path_dac[MAX_DAC][MAX_STRLEN] = {
	"/sys/class/plmem/plmem0/device/mem_type",
	"/sys/class/plmem/plmem1/device/mem_type",
	"/sys/class/plmem/plmem2/device/mem_type",
	"/sys/class/plmem/plmem3/device/mem_type",
	"/sys/class/plmem/plmem4/device/mem_type",
	"/sys/class/plmem/plmem5/device/mem_type",
	"/sys/class/plmem/plmem6/device/mem_type",
	"/sys/class/plmem/plmem7/device/mem_type",
	"/sys/class/plmem/plmem8/device/mem_type",
	"/sys/class/plmem/plmem9/device/mem_type",
	"/sys/class/plmem/plmem10/device/mem_type",
	"/sys/class/plmem/plmem11/device/mem_type",
	"/sys/class/plmem/plmem12/device/mem_type",
	"/sys/class/plmem/plmem13/device/mem_type",
	"/sys/class/plmem/plmem14/device/mem_type",
	"/sys/class/plmem/plmem15/device/mem_type",
};
char bram_ddr_path_adc[MAX_ADC][MAX_STRLEN] = {
	"/sys/class/plmem/plmem16/device/mem_type",
	"/sys/class/plmem/plmem17/device/mem_type",
	"/sys/class/plmem/plmem18/device/mem_type",
	"/sys/class/plmem/plmem19/device/mem_type",
	"/sys/class/plmem/plmem20/device/mem_type",
	"/sys/class/plmem/plmem21/device/mem_type",
	"/sys/class/plmem/plmem22/device/mem_type",
	"/sys/class/plmem/plmem23/device/mem_type",
	"/sys/class/plmem/plmem24/device/mem_type",
	"/sys/class/plmem/plmem25/device/mem_type",
	"/sys/class/plmem/plmem26/device/mem_type",
	"/sys/class/plmem/plmem27/device/mem_type",
	"/sys/class/plmem/plmem28/device/mem_type",
	"/sys/class/plmem/plmem29/device/mem_type",
	"/sys/class/plmem/plmem30/device/mem_type",
	"/sys/class/plmem/plmem31/device/mem_type",
};

extern int dac_userselect_gpio[(MAX_DAC_PER_TILE * MAX_DAC_TILE)];

struct rfsoc_info info;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

void EnableAllInterrupts()
{
	int Tile_Id = 0;
	int Block_Id = 0;

	for (Tile_Id = 0; Tile_Id < MAX_TILE_ID; Tile_Id++) {
		/* Disable FIFOs */
		XRFdc_SetupFIFO(&RFdcInst, XRFDC_DAC_TILE, Tile_Id, 0);
		/* Enable DAC Interrupts */
		for (Block_Id = 0; Block_Id < MAX_BLOCK_ID; Block_Id++) {
			if (XRFdc_IsDACBlockEnabled(&RFdcInst, Tile_Id,
						    Block_Id)) {
				XRFdc_IntrDisable(&RFdcInst, XRFDC_DAC_TILE,
						  Tile_Id, Block_Id,
						  0xffffffff);
				XRFdc_IntrEnable(&RFdcInst, XRFDC_DAC_TILE,
						 Tile_Id, Block_Id, 0xffffffff);
				XRFdc_IntrClr(&RFdcInst, XRFDC_DAC_TILE,
					      Tile_Id, Block_Id, 0xffffffff);
			}
		}
		/* Enable FIFOs */
		XRFdc_SetupFIFO(&RFdcInst, XRFDC_DAC_TILE, Tile_Id, 1);
		/* Disable FIFOs */
		XRFdc_SetupFIFO(&RFdcInst, XRFDC_ADC_TILE, Tile_Id, 0);
		/* Enable ADC Interrupts */
		for (Block_Id = 0; Block_Id < MAX_BLOCK_ID; Block_Id++) {
			if (XRFdc_IsADCBlockEnabled(&RFdcInst, Tile_Id,
						    Block_Id)) {
				XRFdc_IntrDisable(&RFdcInst, XRFDC_ADC_TILE,
						  Tile_Id, Block_Id,
						  0xffffffff);
				XRFdc_IntrEnable(&RFdcInst, XRFDC_ADC_TILE,
						 Tile_Id, Block_Id, 0xffffffff);
				XRFdc_IntrClr(&RFdcInst, XRFDC_ADC_TILE,
					      Tile_Id, Block_Id, 0xffffffff);
			}
		}
		/* Enable FIFOs */
		XRFdc_SetupFIFO(&RFdcInst, XRFDC_ADC_TILE, Tile_Id, 1);
	}
}

int init_mem()
{
	int i, ret;
	for (i = 0; i < MAX_DAC; i++) {
		/* Set memory type */
		ret = write_to_file(mem_type_path_dac[i], PL_MEM);
		if (ret != SUCCESS) {
			printf("Error configuring memory\n");
			return FAIL;
		}

		/* open memory file */
		info.fd_dac[i] = open(mem_path_dac[i], O_RDWR);
		if ((info.fd_dac[i]) < 0) {
			printf("file %s open failed\n", mem_path_dac[i]);
			return FAIL;
		}
		/* map size of memory */
		info.map_dac[i] =
			(signed char *)mmap(0, DAC_MAP_SZ,
					    PROT_READ | PROT_WRITE, MAP_SHARED,
					    info.fd_dac[i], 0);
		if ((info.map_dac[i]) == MAP_FAILED) {
			printf("Error mmapping the file %d\n", i);
			return FAIL;
		}
	}
	for (i = 0; i < MAX_ADC; i++) {
		/* Set memory type */
		ret = write_to_file(mem_type_path_adc[i], PL_MEM);
		if (ret != SUCCESS) {
			printf("Error configuring memory\n");
			return FAIL;
		}

		/* open memory file */
		info.fd_adc[i] = open(mem_path_adc[i], O_RDWR);
		if ((info.fd_adc[i]) < 0) {
			printf("file %s open failed\n", mem_path_adc[i]);
			return FAIL;
		}
		/* map size of memory */
		info.map_adc[i] =
			(signed char *)mmap(0, ADC_MAP_SZ,
					    PROT_READ | PROT_WRITE, MAP_SHARED,
					    info.fd_adc[i], 0);
		if ((info.map_adc[i]) == MAP_FAILED) {
			printf("Error mmapping the file %d\n", i);
			return FAIL;
		}
	}
	info.mem_type_dac = 0xF; /* Set all 4 tiles in BRAM mode */
	info.mem_type_adc = 0xF; /* Set all 4 tiles in BRAM mode */

	return SUCCESS;
}

int deinit_path(int *fd, signed char *map, unsigned int sz)
{
	if ((*fd) != 0) {
		fsync(*fd);
	}

	if (map != NULL) {
		if (munmap(map, sz) == -1)
			MetalLoghandler_firmware(FAIL, "unmap failed\n");
	}

	if ((*fd) != 0) {
		close(*fd);
		*fd = 0;
	}
	return SUCCESS;
}

int deinit_mem(void)
{
	int i, ret;
	for (i = 0; i < MAX_DAC; i++) {
		ret = write_to_file(mem_type_path_dac[i], NO_MEM);
		if (ret != SUCCESS) {
			MetalLoghandler_firmware(
				ret,
				"Error configuring dac memory: DAC mem index:"
				" %d\n",
				i);
		}
		ret = write_to_file(bram_ddr_path_dac[i], BRAM);
		if (ret != SUCCESS) {
			MetalLoghandler_firmware(
				ret,
				"Error configuring dac memory: DAC mem index:"
				" %d\n",
				i);
		}
		deinit_path(&info.fd_dac[i], info.map_dac[i], DAC_MAP_SZ);
	}
	for (i = 0; i < MAX_ADC; i++) {
		ret = write_to_file(mem_type_path_adc[i], NO_MEM);
		if (ret != SUCCESS) {
			MetalLoghandler_firmware(
				ret,
				"Error configuring ADC memory: ADC mem index:"
				" %d\n",
				i);
		}
		ret = write_to_file(bram_ddr_path_adc[i], BRAM);
		if (ret != SUCCESS) {
			MetalLoghandler_firmware(
				ret,
				"Error configuring dac mem type:  mem index:"
				" %d\n",
				i);
		}
		deinit_path(&info.fd_adc[i], info.map_adc[i], ADC_MAP_SZ);
	}
	return SUCCESS;
}

int change_fifo_stat(int fifo_id, int tile_id, int stat)
{
	int ret;

	/*FIFO disable*/
	ret = XRFdc_SetupFIFO(&RFdcInst, fifo_id, tile_id, stat);
	if (ret != SUCCESS) {
		MetalLoghandler_firmware(ret, "Unable to disable FIFO\n");
		return ret;
	}
	return SUCCESS;
}

void SetMemtype(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 i;
	int ret;
	char buff[BUF_MAX_LEN] = { 0 };
	unsigned int mem_type;
	unsigned int type;
	u32 tile_id;
	u32 max_dac;
	u32 max_blocks;
	u32 max_adc;

	type = cmdVals[0].u;
	tile_id = cmdVals[1].u;
	mem_type = cmdVals[2].u;

	if (type == DAC) {
		fsync((info.fd_dac[0]));
		if (mem_type == BRAM)
			info.mem_type_dac |= (1 << tile_id);
		else
			info.mem_type_dac &= ~(1 << tile_id);

		/* bram_ddr_path_dac is set with mem_type for all the blocks
		 * in the tile */
		i = ((tile_id * MAX_DAC_TILE));
		max_blocks = (tile_id + 1) * MAX_DAC_TILE;
		for (; i < max_blocks; i++) {
			/* Set memory type for DAC */
			ret = write_to_file(bram_ddr_path_dac[i], mem_type);
			if (ret != SUCCESS) {
				MetalLoghandler_firmware(
					ret, "Error configuring memory\n");
				goto err;
			}
		}
		/* Gpios in the tile are set for BRAM mode */
		i = ((tile_id * MAX_DAC_PER_TILE));
		max_dac = (((tile_id + 1) * MAX_DAC_PER_TILE));
		for (; i < max_dac; i++) {
			if (mem_type == BRAM) {
				ret = set_gpio(dac_userselect_gpio[i], 0);
				if (ret) {
					ret = GPIO_SET_FAIL;
					MetalLoghandler_firmware(
						ret,
						"Unable to set DAC userselect"
						" GPIO value\n");
					goto err;
				}
			}
		}
	} else {
		if (mem_type == BRAM)
			info.mem_type_adc |= (1 << tile_id);
		else
			info.mem_type_adc &= ~(1 << tile_id);

		/* bram_ddr_path_adc is set with mem_type for all the blocks
		 * in the tile */
		i = ((tile_id * MAX_ADC_PER_TILE));
		max_adc = (((tile_id + 1) * MAX_ADC_PER_TILE));
		for (; i < max_adc; i++) {
			/* Set memory type for ADC */
			ret = write_to_file(bram_ddr_path_adc[i], mem_type);
			if (ret != SUCCESS) {
				MetalLoghandler_firmware(
					ret, "Error configuring memory\n");
				goto err;
			}
			if (mem_type == BRAM)
				config_axi_switch(
					ADC_CHANNEL_SELECT_AXI_SWITCH_BASE_ADDR,
					0x40, 0, i);
		}
	}
	*status = SUCCESS;
	return;
err:
	if (enTermMode) {
		MetalLoghandler_firmware(ret,
					 "cmd = SetMemtype\n"
					 "mem_type = %d\n",
					 mem_type);
	}
	/* append error code */
	sprintf(buff, " %d", ret);
	strcat(txstrPtr, buff);
	*status = FAIL;
}

void GetMemtype(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char buff[BUF_MAX_LEN] = { 0 };
	unsigned int type = cmdVals[0].u;
	unsigned int tile_id = cmdVals[1].u;

	if (type == DAC)
		sprintf(buff, " %d ",
			((info.mem_type_dac & (1 << tile_id)) != 0));
	else
		sprintf(buff, " %d ",
			((info.mem_type_adc & (1 << tile_id)) != 0));
	strncat(txstrPtr, buff, BUF_MAX_LEN);
	*status = SUCCESS;
	return;
}

int WriteDataToMemory_bram(u32 block_id, int tile_id, u32 size, u32 il_pair)
{
	(void)il_pair;
	u32 ret = 0;
	u32 paddr_dac;
	signed int *vaddr_dac;
	void *bram_base_dac;
	unsigned char *base_dac;
	u32 i, j;

	if (size > FIFO_SIZE) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"Requested size is bigger than available"
			" size(%d samples), tile %d block %u\n",
			FIFO_SIZE / 2, tile_id, block_id);
		goto err;
	}
	if ((size == 0) || (size % ADC_DAC_SZ_ALIGNMENT) != 0) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(ret,
					 "size should be multiples of 16"
					 " samples, tile %d block %u\n",
					 tile_id, block_id);
		goto err;
	}
	base_dac = (unsigned char *)malloc(sizeof(char) * size);
	if (base_dac == NULL) {
		printf("failed to allocate memory, tile %d block %u\n", tile_id,
		       block_id);
		goto err;
	}
	memset(base_dac, 0, size);
	paddr_dac = DacMap[tile_id * 4 + block_id].addr_I;
	bram_base_dac = mmap(NULL, (size), PROT_READ | PROT_WRITE, MAP_SHARED,
			     info.fd, (paddr_dac));
	if (bram_base_dac == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the file, tile %d block %u\n",
		       __func__, tile_id, block_id);
		free(base_dac);
		goto err;
	}
	vaddr_dac = (signed int *)bram_base_dac;

	for (i = 0; i < size / 4; i++)
		vaddr_dac[i] = 0;

	/* get data from socket */
	ret = getSamples((base_dac), size);
	if (ret != size) {
		munmap(bram_base_dac, size);
		free(base_dac);
		ret = RCV_SAMPLE_FAIL;
		MetalLoghandler_firmware(ret,
					 "Unable to read %d bytes of data, "
					 "received : %d, tile %d block %u\n",
					 size, ret, tile_id, block_id);
		goto err;
	}

	j = 0;
	for (i = 0; i < (size / 4); i++) {
		vaddr_dac[i] =
			(base_dac[j] | (base_dac[j + 1] << 8) |
			 (base_dac[j + 2] << 16) | (base_dac[j + 3] << 24));
		j += 4;
	}

	munmap(bram_base_dac, size);
	free(base_dac);
	/* Enable RFDC FIFO */
	ret = change_fifo_stat(DAC, tile_id, FIFO_EN);
	if (ret != SUCCESS) {
		ret = EN_FIFO_FAIL;
		MetalLoghandler_firmware(ret,
					 "Unable to enable FIFO, "
					 "tile %d block %u\n",
					 tile_id, block_id);
		goto err;
	}
	return 0;
err:
	return ret;
}

int WriteDataToMemory_ddr(u32 block_id, int tile_id, u32 size, u32 il_pair)
{
	(void)il_pair;
	int in_val_len = 0;
	int dac;
	int ret;

	if (size > DAC_MAP_SZ) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"Requested samples %d must be lower than"
			" %d in tile %d block %u\n",
			size >> 1, DAC_MAP_SZ >> 1, tile_id, block_id);
		in_val_len = 1;
	}
	if ((size == 0) || ((size % ADC_DAC_SZ_ALIGNMENT) != 0)) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"size %d must be a multiple of 16 samples"
			" tile %d block %u\n",
			size >> 1, tile_id, block_id);
		in_val_len = 1;
	}

	/* extract DAC number from tile_id and block_id */
	dac = (((tile_id & 0x3) << 2) | (block_id & 0x3));

	/* get data from socket */
	ret = getSamples(((u8 *)info.map_dac[dac]), size);
	if ((u32)ret != size) {
		ret = RCV_SAMPLE_FAIL;
		MetalLoghandler_firmware(
			ret,
			"Unable to read %d bytes of data, received : %d"
			" tile %d block %u\n",
			size, ret, tile_id, block_id);
		goto err;
	}

	if (in_val_len) {
		ret = INVAL_ARGS;
		goto err;
	}
	info.channel_size[dac] = size;
	return 0;
err:
	return ret;
}

void WriteDataToMemory(convData_t *cmdVals, char *txstrPtr, int *status)
{
	(void)txstrPtr;
	u32 block_id;
	int tile_id;
	u32 size, il_pair;
	int ret;

	tile_id = cmdVals[0].i;
	block_id = cmdVals[1].u;
	size = cmdVals[2].u;
	il_pair = cmdVals[3].u;

	if ((info.mem_type_dac & (1 << tile_id)) != DDR) {
		ret = WriteDataToMemory_bram(block_id, tile_id, size, il_pair);
		if (ret < 0) {
			printf("Error in executing WriteDataToMemory_bram"
			       " :%d tile %d block %u\n",
			       ret, tile_id, block_id);
			goto err;
		}
	} else {
		ret = WriteDataToMemory_ddr(block_id, tile_id, size, il_pair);
		if (ret < 0) {
			printf("Error in executing WriteDataToMemory_ddr :%d"
			       " tile %d block %u\n",
			       ret, tile_id, block_id);
			goto err;
		}
	}
	*status = SUCCESS;
	return;
err:
	if (enTermMode) {
		printf("cmd = WriteDataToMemory\n"
		       "tile_id = %d\n"
		       "block_id = %u\n"
		       "size = %u\n"
		       "interleaved_pair = %u\n\n",
		       tile_id, block_id, size, il_pair);
	}

	*status = FAIL;
}

int ReadDataFromMemory_bram(u32 block_id, int tile_id, u32 size, u32 il_pair)
{
	int ret;
	u32 paddr_adc_I, paddr_adc_Q;
	u16 numblockpertile =
		RFdcInst.RFdc_Config.ADCTile_Config[tile_id].NumSlices;
	signed int *vaddr_adc_I, *vaddr_adc_Q;
	void *bram_base_adc_I, *bram_base_adc_Q;
	void *buffer_qi;
	XRFdc_Mixer_Settings Mixer_Settings;

	if (il_pair > 1) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"interleaved pair variable should be 0 or 1, "
			" tile %d block %u\n",
			tile_id, block_id);
		goto err;
	}
	if ((il_pair) ? (size > FIFO_SIZE * 2) : (size > FIFO_SIZE)) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"Requested size is bigger than available"
			" (%d samples), tile %d block %u\n",
			FIFO_SIZE / 2, tile_id, block_id);
		goto err;
	}
	if ((size == 0) || (size % ADC_DAC_SZ_ALIGNMENT) != 0) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"size should be multiples of 16 samples, "
			"tile %d block %u\n",
			tile_id, block_id);
		goto err;
	}

	/* Enable RFDC FIFO */
	ret = change_fifo_stat(ADC, tile_id, FIFO_EN);
	if (ret != SUCCESS) {
		ret = EN_FIFO_FAIL;
		MetalLoghandler_firmware(ret,
					 "Unable to enable FIFO, "
					 " tile %d block %u\n",
					 tile_id, block_id);
		goto err;
	}
	ret = XRFdc_GetMixerSettings(&RFdcInst, XRFDC_ADC_TILE, tile_id,
				     block_id, &Mixer_Settings);
	if (FAIL == ret) {
		MetalLoghandler_firmware(ret,
					 "Error from XRFdc_GetMixerSettings, "
					 " ADC Tile_Id = %d Block_Id = %u\n",
					 tile_id, block_id);
		goto err;
	}
	if ((Mixer_Settings.MixerMode == XRFDC_MIXER_MODE_R2R) ||
	    (0 == XRFdc_IsHighSpeedADC(&RFdcInst, tile_id)) ||
	    ((Mixer_Settings.MixerType == XRFDC_MIXER_TYPE_COARSE) &&
	     (Mixer_Settings.CoarseMixFreq == XRFDC_COARSE_MIX_BYPASS))) {
		/* extract DAC number from tile_id and block_id */
		paddr_adc_I =
			AdcMap[tile_id * numblockpertile + block_id].addr_I;
		bram_base_adc_I = mmap(NULL, (size), PROT_READ | PROT_WRITE,
				       MAP_SHARED, info.fd, (paddr_adc_I));
		if (bram_base_adc_I == MAP_FAILED) {
			perror("map");
			printf("%s: Error mmapping from addr_I, "
			       "tile %d block %u\n",
			       __func__, tile_id, block_id);
			goto err;
		}
		vaddr_adc_I = (signed int *)bram_base_adc_I;

		ret = sendSamples((signed char *)vaddr_adc_I,
				  (size * sizeof(signed char)));
		if ((u32)ret != (size * sizeof(signed char))) {
			munmap(bram_base_adc_I, size);
			ret = SND_SAMPLE_FAIL;
			MetalLoghandler_firmware(
				ret,
				"Unable to send %d bytes, sent %d bytes, "
				"tile %d block %u\n",
				size, ret, tile_id, block_id);
			goto err;
		}
		munmap(bram_base_adc_I, size);
	} else {
		buffer_qi = (unsigned char *)malloc(sizeof(char) * size);
		if (buffer_qi == NULL) {
			printf("%s: Failed to allocate qi buffer memory, "
			       "tile %d block %u\n",
			       __func__, tile_id, block_id);
			goto err;
		}

		paddr_adc_I =
			AdcMap[tile_id * numblockpertile + block_id].addr_I;
		bram_base_adc_I = mmap(NULL, (size / 2), PROT_READ | PROT_WRITE,
				       MAP_SHARED, info.fd, (paddr_adc_I));
		if (bram_base_adc_I == MAP_FAILED) {
			perror("map");
			printf("%s: Error mmapping for addr_I, "
			       " tile %d block %u\n",
			       __func__, tile_id, block_id);
			free(buffer_qi);
			goto err;
		}
		vaddr_adc_I = (signed int *)bram_base_adc_I;
		memcpy(buffer_qi, vaddr_adc_I, (size / 2));
		paddr_adc_Q =
			AdcMap[tile_id * numblockpertile + block_id].addr_Q;
		bram_base_adc_Q = mmap(NULL, (size / 2), PROT_READ | PROT_WRITE,
				       MAP_SHARED, info.fd, (paddr_adc_Q));
		if (bram_base_adc_Q == MAP_FAILED) {
			perror("map");
			munmap(bram_base_adc_I, size / 2);
			free(buffer_qi);
			printf("%s: Error mmapping the for addr_Q, "
			       "tile %d block %u\n",
			       __func__, tile_id, block_id);
			goto err;
		}
		vaddr_adc_Q = (signed int *)bram_base_adc_Q;
		memcpy(buffer_qi + (size / 2), vaddr_adc_Q, (size / 2));

		ret = sendSamples(buffer_qi, (size * sizeof(signed char)));
		if ((u32)ret != (size * sizeof(signed char))) {
			ret = SND_SAMPLE_FAIL;
			MetalLoghandler_firmware(
				ret,
				"Unable to send %d bytes, sent %d bytes, "
				"tile %d block %u\n",
				size, ret, tile_id, block_id);
			munmap(bram_base_adc_I, size / 2);
			munmap(bram_base_adc_Q, size / 2);
			free(buffer_qi);
			goto err;
		}
		munmap(bram_base_adc_I, size / 2);
		munmap(bram_base_adc_Q, size / 2);
		free(buffer_qi);
	}

	return 0;
err:
	return ret;
}

int ReadDataFromMemory_ddr(u32 block_id, int tile_id, u32 size, u32 il_pair)
{
	int ret;
	int adc;
	int dma_req_size;
	signed char *map_adc;
	char buffer[1000];
#ifdef XPS_BOARD_ZCU208
	u32 size_integer;
	u32 size_mod;
	void *buffer_iq;
#endif

	if (size > ADC_MAP_SZ) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"Requested samples %d must be lower than"
			" %d , tile %d block %u\n",
			size >> 1, ADC_MAP_SZ >> 1, tile_id, block_id);
		goto err;
	}
	if ((size == 0) || ((size % ADC_DAC_SZ_ALIGNMENT) != 0)) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"size %d must be a multiple of 16 samples, "
			"tile %d block %u\n",
			size >> 1, tile_id, block_id);
		goto err;
	}

	/* ADC_DMABUF_SKIP_AREA and SAMPLE_DMA_ALIGN are hardware design(xsa)
	 * specific values */
#ifdef XPS_BOARD_ZCU208
	dma_req_size = (((size / SAMPLE_DMA_ALIGN) + 1) * SAMPLE_DMA_ALIGN) +
		       ADC_DMABUF_SKIP_AREA * 2;
#else
	dma_req_size = size + ADC_DMABUF_SKIP_AREA * 2;
#endif
	printf("ADC DMA request for size %u \n", dma_req_size);
	if (dma_req_size > ADC_MAP_SZ) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(
			ret,
			"Requested samples must be lower than"
			" (%d samples), tile %d block %u\n",
			(ADC_MAP_SZ - (ADC_DMABUF_SKIP_AREA * 2)) >> 1, tile_id,
			block_id);
		goto err;
	}

	if (il_pair > 1) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(ret,
					 "il_pair > 1, invalid arguments"
					 ", tile %d block %u\n",
					 tile_id, block_id);
		goto err;
	}

	/* extract ADC number from tile_id and block_id */
	adc = tile_id * MAX_ADC_PER_TILE + block_id;

	/* Enable RFDC FIFO */
	ret = change_fifo_stat(ADC, tile_id, FIFO_EN);
	if (ret != SUCCESS) {
		ret = EN_FIFO_FAIL;
		MetalLoghandler_firmware(ret,
					 "Unable to enable FIFO, "
					 "tile %d block %u\n",
					 tile_id, block_id);
		goto err;
	}
	config_axi_switch(ADC_CHANNEL_SELECT_AXI_SWITCH_BASE_ADDR, 0x40, 1,
			  adc);
	/* 0xA0310000 is address of tlast register, this address depends
	 on Design. */
	sprintf(buffer, "devmem 0xA0310000 32 0x%x", dma_req_size);
	system(buffer);
	/* reset the AXI stream switch and DMA */
	ret = set_gpio(adc_axiswitchrst[0], 0);
	if (ret) {
		ret = GPIO_SET_FAIL;
		printf("Unable to set adc axi switch reset gpio\n");
		MetalLoghandler_firmware(ret, "Unable to reset adc axi switch "
					      "reset GPIO \n");
		goto err;
	}

	ret = fsync((info.fd_adc[0]));
	if (SUCCESS != ret) {
		ret = DMA_RESET_FAIL;
		printf("DMA reset failed\n");
		MetalLoghandler_firmware(ret, "DMA reset failed\n");
		goto err;
	}

	ret = set_gpio(adc_axiswitchrst[0], 1);
	if (ret) {
		ret = GPIO_SET_FAIL;
		printf("Unable to set adc axi switch reset gpio\n");
		MetalLoghandler_firmware(ret, "Unable to set adc axi switch "
					      "reset GPIO \n");
		goto err;
	}

	/* Trigger DMA */
	ret = read((info.fd_adc[adc]), info.map_adc[adc],
		   ((dma_req_size) * sizeof(signed char)));
	if (ret < 0) {
		ret = DMA_TX_TRIGGER_FAIL;
		printf("Error in reading data\n");
		MetalLoghandler_firmware(ret,
					 "Error in reading data, "
					 "tile %d block %u\n",
					 tile_id, block_id);
		goto err;
	}

	map_adc = info.map_adc[adc];
#ifdef XPS_BOARD_ZCU208
	/* SAMPLE_ALIGN is a hardware design(xsa) specific value */
	size_integer = (size / SAMPLE_ALIGN) * SAMPLE_ALIGN;
	size_mod = size % SAMPLE_ALIGN;
	buffer_iq = (signed char *)malloc(sizeof(char) * size);

	memcpy(buffer_iq, map_adc + ADC_DMABUF_SKIP_AREA, size_integer);
	memcpy(buffer_iq + size_integer,
	       map_adc + ADC_DMABUF_SKIP_AREA + size_integer, size_mod / 2);
	memcpy(buffer_iq + size_integer + size_mod / 2,
	       map_adc + ADC_DMABUF_SKIP_AREA + size_integer +
		       (SAMPLE_DMA_ALIGN / 2),
	       size_mod / 2);
	ret = sendSamples(buffer_iq, (size * sizeof(signed char)));

	free(buffer_iq);
#else
	ret = sendSamples((map_adc + ADC_DMABUF_SKIP_AREA),
			  (size * sizeof(signed char)));
#endif
	if ((u32)ret != (size * sizeof(signed char))) {
		ret = SND_SAMPLE_FAIL;
		printf("sendSamples failed");
		MetalLoghandler_firmware(
			ret,
			"Unable to send %d bytes, sent %d bytes, "
			"tile %d block %u\n",
			size, ret, tile_id, block_id);
		goto err;
	}
	return 0;
err:
	return ret;
}

void ReadDataFromMemory(convData_t *cmdVals, char *txstrPtr, int *status)
{
	(void)txstrPtr;
	u32 block_id;
	int tile_id;
	u32 size, il_pair;
	int ret;

	tile_id = cmdVals[0].i;
	block_id = cmdVals[1].u;
	size = cmdVals[2].u;
	il_pair = cmdVals[3].u;

	if ((info.mem_type_adc & (1 << tile_id)) != DDR) {
		ret = ReadDataFromMemory_bram(block_id, tile_id, size, il_pair);
		if (ret < 0) {
			printf("Error in executing ReadDataFromMemory_bram"
			       " :%d, tile %d block %u\n",
			       ret, tile_id, block_id);
			goto err;
		}
	} else {
		ret = ReadDataFromMemory_ddr(block_id, tile_id, size, il_pair);
		if (ret < 0) {
			printf("Error in executing ReadDataFromMemory_ddr"
			       " :%d, tile %d block %u\n",
			       ret, tile_id, block_id);
			goto err;
		}
	}
	*status = SUCCESS;
	return;
err:
	if (enTermMode) {
		printf("cmd = ReadDataFromMemory\n"
		       "tile_id = %d\n"
		       "block_id = %u\n"
		       "size = %u\n"
		       "interleaved_pair = %u\n\n",
		       tile_id, block_id, size, il_pair);
	}

	*status = FAIL;
}

/*
 * The API is a wrapper function used as a bridge with the command interface.
 * The function disable hardware fifo and disable datapath memory.
 */
void disconnect(convData_t *cmdVals, char *txstrPtr, int *status)
{
	(void)cmdVals;
	(void)txstrPtr;

	*status = SUCCESS;
	return;
}

/* Thread function for data path. It receives command/data from data socket
 */
void *datapath_t(void *args)
{
	(void)args;
	/* receive buffer of BUF_MAX_LEN character */
	char rcvBuf[BUF_MAX_LEN] = { 0 };
	/* tx buffer of BUF_MAX_LEN character */
	char txBuf[BUF_MAX_LEN] = { 0 };
	/* buffer len must be set to max buffer minus 1 */
	int bufLen = BUF_MAX_LEN - 1;
	/* number of character received per command line */
	int numCharacters;
	/* status of the command: XST_SUCCES - ERROR_CMD_UNDEF
	 * - ERROR_NUM_ARGS - ERROR_EXECUTE */
	int cmdStatus;

	info.mts_enable_dac_mask = 0;
	info.mts_enable_adc_mask = 0;
whileloop:
	while (thread_stat) {
		/* get string from io interface (Non blocking) */
		numCharacters = getdataString(rcvBuf, bufLen);
		if (numCharacters > 0) {
			/* parse and run with error check */
			cmdStatus = data_cmdParse(rcvBuf, txBuf);
			/* check cmParse status - return an error message or the
			 response */
			if (cmdStatus != SUCCESS) {
				/* command returned an errors */
				errorIf_data(txBuf, cmdStatus);
			} else {
				/* send response */
				if (0 > senddataString(txBuf, strlen(txBuf)))
					printf("%s: Error in senddataString %s\n",
					       __func__, txBuf);
			}

			/* clear rcvBuf each time anew command is received
			 and processed */
			memset(rcvBuf, 0, sizeof(rcvBuf));
			/* clear txBuf each time anew command is received and
			 a response returned */
			memset(txBuf, 0, sizeof(txBuf));

		} else if (numCharacters == 0) {
			goto whileloop;
		}
	}
	return NULL;
}
