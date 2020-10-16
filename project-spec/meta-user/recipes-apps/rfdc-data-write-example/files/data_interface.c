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
#include "data_interface.h"
#include "gpio.h"
#include <unistd.h>

/************************** Constant Definitions *****************************/
extern XRFdc RFdcInst;      /* RFdc driver instance */

#define MAX_ADC 8
#define MAX_DAC 8
#define MAX_STRLEN 512
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)
#define SCRATCHPAD_REG 0xB0005050
#define DESIGN_TYPE_REG 0xB0005054

struct rfsoc_info {
	signed char *map_dac[MAX_DAC];
	signed char *map_adc[MAX_ADC];
	void *map_scratchreg;
	void *map_design_type_reg;
  	int fd_scratchpad;
	int fd_dac[MAX_DAC];
	int fd_adc[MAX_ADC];
	int adc_status, dac_status;
  	u32 scratch_value;
	u32 design_type;
};

char mem_path_dac[MAX_DAC][MAX_STRLEN] = {
	"/dev/plmem0",
	"/dev/plmem1",
	"/dev/plmem2",
	"/dev/plmem3",
	"/dev/plmem4",
	"/dev/plmem5",
	"/dev/plmem6",
	"/dev/plmem7",
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
};

/* GPIO0,4,8,12,16,20,24,28 */
int dac_reset_gpio[MAX_DAC] = {
	416,
	420,
	424,
	428,
	432,
	436,
	440,
	444,
};
/* GPIO2,6,10,14,18,22,26,30  */
int dac_localstart_gpio[MAX_DAC] = {
	418,
	422,
	426,
	430,
	434,
	438,
	442,
	446,
};
/* GPIO2,6,10,14,18,22,26,30  */
int dac_loopback_gpio[MAX_DAC] = {
	419,
	421,
	423,
	425,
	427,
	429,
	431,
	433,
};

/* Starts from GPIO64 to GPIO67 */
int dac_select_gpio[MAX_DAC] = {
	480,
	481,
	482,
	483,
};

struct rfsoc_info info;

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
int init_mem()
{
	int i, ret;
  	void *base;
	u32 max_dac, max_adc;
	u32 value;

	/* open memory file */
	info.fd_scratchpad = open("/dev/mem", O_RDWR | O_NDELAY);
	if (info.fd_scratchpad < 0) {
		printf("file /dev/mem open failed\n");
		return FAIL;
	}
	/* map size of memory */
	base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
			info.fd_scratchpad, (SCRATCHPAD_REG & ~MAP_MASK));
	if (base == MAP_FAILED) {
		perror("map");
		printf("Error mmapping the file\n");
		close(info.fd_scratchpad);
		return FAIL;
	}
	info.map_scratchreg = base + (SCRATCHPAD_REG & MAP_MASK);
	/* map size of memory */
	base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
			info.fd_scratchpad, (DESIGN_TYPE_REG & ~MAP_MASK));
	if (base == MAP_FAILED) {
		perror("map");
		printf("Error mmapping the file\n");
		close(info.fd_scratchpad);
		return FAIL;
	}
	info.map_design_type_reg = base + (DESIGN_TYPE_REG & MAP_MASK);
	info.design_type = 0;
	value = *(u32*) (info.map_design_type_reg);
	info.design_type = (value & (3 << 16));
	info.design_type = (info.design_type >> 16);

	if (info.design_type != NON_MTS) {
		printf("RFDC Example design is supported only for NON-MTS design\n");
		return FAIL;
	}

	if (info.design_type == DAC1_ADC1) {
		max_dac = 1;
		max_adc = 1;
	} else {
		max_dac = MAX_DAC;
		max_adc = MAX_ADC;
	}
	for (i = 0; i < max_dac; i++) {
		/* Set memory type */		
		ret = write_to_file(mem_type_path_dac[i], PL_MEM);
		if (ret != SUCCESS) {
			printf("Error configuring memroy\n");
			return FAIL;
		}
		/* open memory file */ 	
		info.fd_dac[i] = open(mem_path_dac[i], O_RDWR);
		if ((info.fd_dac[i]) < 0) {
			printf("file %s open failed\n", mem_path_dac[i]);
			return FAIL;
		}
		/* map size of memory */
		info.map_dac[i] = (signed char*)mmap(0, DAC_MAP_SZ, PROT_READ |
									PROT_WRITE, MAP_SHARED, info.fd_dac[i], 0);
		if ((info.map_dac[i]) == MAP_FAILED) {
			printf("Error mmapping the file %d\n", i);
			return FAIL;
		}
	}

	
	return SUCCESS;
}

int deinit_path(int *fd, signed char* map, unsigned int sz)
{
	if ((*fd) != 0) {
		fsync(*fd);
	}

	if(map != NULL) {
		if(munmap(map, sz) == -1)
			printf("unmap failed\n");
	}

	if ((*fd) != 0) {
		close(*fd);
		*fd = 0;
	}
}

int deinit_mem(void)
{
	int i, ret;
	u32 max_dac, max_adc;

	if (info.design_type == DAC1_ADC1) {
		max_dac = 1;
		max_adc = 1;
	} else {
		max_dac = MAX_DAC;
		max_adc = MAX_ADC;
	}
	for (i = 0; i < max_dac; i++) {
		ret = write_to_file(mem_type_path_dac[i], NO_MEM);
		if (ret != SUCCESS) {
			printf("Error releasing memory\n");
			return FAIL;
		}
		deinit_path(&info.fd_dac[i], info.map_dac[i], DAC_MAP_SZ);
	}

	if (info.map_scratchreg != NULL) {
		if (munmap((info.map_scratchreg - (SCRATCHPAD_REG & MAP_MASK)), MAP_SIZE) ==
				-1)
			printf("unmap failed for info.map_scratchreg\n");
	}
	if (info.fd_scratchpad) {
		close(info.fd_scratchpad);
		info.fd_scratchpad = 0;
	}

	return SUCCESS;
}

int init_gpio()
{
	int i, ret;
	u32 max_dac, max_adc;

	if (info.design_type == DAC1_ADC1) {
		max_dac = 1;
		max_adc = 1;
	} else {
		max_dac = MAX_DAC;
		max_adc = MAX_ADC;
	}
	for (i = 0; i < max_dac; i++) {
		/* Enable/Export reset GPIO */
		ret = enable_gpio(dac_reset_gpio[i]);
		if (ret) {
			printf("Unable to enable reset GPIO\n");
			return ret;
		}
		ret = config_gpio_op(dac_reset_gpio[i]);
		if (ret) {
			printf("Unable to set direction for reset gpio of dac %d\n", i);
			return ret;
		}
		/* Enable/Export localstart GPIO */
		ret = enable_gpio(dac_localstart_gpio[i]);
		if (ret) {
			printf("Unable to enable localstart gpio of dac %d\n", i);
			return ret;
		}
		ret = config_gpio_op(dac_localstart_gpio[i]);
		if (ret) {
			printf("Unable to set direction for localstart of dac %d\n", i);
			return ret;
		}
		/* Enable/Export loopback GPIO */
		ret = enable_gpio(dac_loopback_gpio[i]);
		if (ret) {
			printf("Unable to enable loopback gpio of dac %d\n", i);
			return ret;
		}
		ret = config_gpio_op(dac_loopback_gpio[i]);
		if (ret) {
			printf("Unable to set direction for loopback of dac %d\n", i);
			return ret;
		}
	}
	for (i = 0; i < DAC_MUX_GPIOS; i++) {
		/* Enable/Export channel select GPIO */
		ret = enable_gpio(dac_select_gpio[i]);
		if (ret) {
			printf("Unable to enable select GPIO of dac %d\n", i);
			return ret;
		}
		ret = config_gpio_op(dac_select_gpio[i]);
		if (ret) {
			printf("Unable to set direction for select gpio of dac %d\n", i);
			return ret;
		}
	}

	return SUCCESS;
}

int deinit_gpio()
{
	int i, ret;
	u32 max_dac, max_adc;

	if (info.design_type == DAC1_ADC1) {
		max_dac = 1;
		max_adc = 1;
	} else {
		max_dac = MAX_DAC;
		max_adc = MAX_ADC;
	}

	for (i = 0; i < max_dac; i++) {
		/* Release reset GPIO */
		ret = disable_gpio(dac_reset_gpio[i]);
		if (ret) {
			printf("Unable to release reset GPIO of dac\n");
			return ret;
		}
		/* Release localstart GPIO */
		ret = disable_gpio(dac_localstart_gpio[i]);
		if (ret) {
			printf("Unable to release localstart gpio of dac\n");
			return ret;
		}
		/* Release loopback GPIO */
		ret = disable_gpio(dac_loopback_gpio[i]);
		if (ret) {
			printf("Unable to release localstart gpio of dac\n");
			return ret;
		}
	}

	for (i = 0; i < DAC_MUX_GPIOS; i++) {
		/* Release channel select GPIO */
		ret = disable_gpio(dac_select_gpio[i]);
		if (ret) {
			printf("Unable to release select GPIO of dac\n");
			return ret;
		}
	}
}

int channel_select_gpio(int *sel_gpio_path, int val, int type) {

	int ret, i;
	char gpio_path[MAX_STRLEN];

	if (type == DAC) {
		// update scratch pad register
		*(volatile u32 *)info.map_scratchreg = info.scratch_value;

		ret = set_gpio(dac_select_gpio[0], 0);
		if (ret) {
			printf("Unable to set select GPIO value\n");
			return ret;
		}
		usleep(10);

		//	toggle gpio64
		ret = set_gpio(dac_select_gpio[0], 1);
		if (ret) {
			printf("Unable to set select GPIO value\n");
			return ret;
		}

	}
}

int change_fifo_stat(int fifo_id, int tile_id, int stat)
{
	int ret;

	ret = XRFdc_SetupFIFO(&RFdcInst, fifo_id, tile_id, stat);  /*FIFO disable*/
	if(ret != SUCCESS) {
		printf("Unable to disable FIFO\n");
		return ret;
	}
	return SUCCESS;
}

int disable_all_fifo(int fifo_id)
{
	int ret;
	int ct;
	u32 max_dac_tiles;
	u32 max_adc_tiles;

	if (info.design_type == DAC1_ADC1) {
		max_dac_tiles = 1;  
		max_adc_tiles = 1;
	} else {
		max_dac_tiles = MAX_DAC_TILE;
		max_adc_tiles = MAX_ADC_TILE;
	}
	if(fifo_id == DAC) {
		for(ct = 0; ct < max_dac_tiles; ct++) {
			ret = change_fifo_stat(fifo_id, ct, FIFO_DIS);
			if(ret != SUCCESS) {
				printf("Unable to disable DAC FIFO\n");
				return ret;
			}
		}
	}

	return SUCCESS;
}

int WriteDataToMemory(const signed short *data, int tile_id, int block_id, int size)
{
	unsigned int ret;
	char gpio_path[MAX_STRLEN];
	int dac;
	int i;

	if((size == 0) || ((size % ADC_DAC_DMA_SZ_ALIGNMENT) != 0) ||
			(size  > DAC_ADC_FIFO_SZ)) {
		printf("Requested size is bigger then FiFO size; Fifo size: %d bytes\n", DAC_ADC_FIFO_SZ);
		return FAIL;
	}

	/* extract dac channel ID */
	dac = (((tile_id & 0x1) << 2) | (block_id & 0x3));

	info.scratch_value = (1 << dac);
	/* Copy data to DMAble memory*/
	memcpy(info.map_dac[dac], data, size);

	/* Disable local start gpio */
	ret = set_gpio(dac_localstart_gpio[dac], 0);
	if (ret) {
		printf("Unable to re-set localstart GPIO value\n");
		return FAIL;
	}
	ret = set_gpio(dac_loopback_gpio[dac], 0);
	if (ret) {
		printf("Unable to re-set loopback GPIO value\n");
		return FAIL;
	}

	/* write to channel select GPIO */
	ret = channel_select_gpio(dac_select_gpio, dac, DAC);
	if(ret) {
		printf("Unable to set channelselect GPIO value\n");
		return FAIL;
	}

	/* Assert and De-Assert reset FIFO GPIO */
	ret = set_gpio(dac_reset_gpio[dac], 1);
	if (ret) {
		printf("Unable to assert reset GPIO value\n");
		return FAIL;
	}

	usleep(10);

	ret = set_gpio(dac_reset_gpio[dac], 0);
	if (ret) {
		printf("Unable to de-assert reset GPIO value\n");
		return FAIL;
	}


	/* Enable RFDC FIFO */
	ret = change_fifo_stat(DAC, tile_id, FIFO_EN);
	if(ret != SUCCESS) {
		printf("Unable to enable FIFO\n");
		return FAIL;
	}
	/* Trigger DMA */
	ret = write((info.fd_dac[dac]), info.map_dac[dac], size);
	if (ret < 0) {
		printf("Error in writing data\n");
		return FAIL;
	}

	ret = set_gpio(dac_loopback_gpio[dac], 1);
	if (ret) {
		printf("Unable to re-set localstart GPIO value\n");
		return FAIL;
	}
	/* enable local start GPIO */
	ret = set_gpio(dac_localstart_gpio[dac], 1);
	if (ret) {
		printf("Unable to set localstart GPIO value\n");
		return FAIL;
	}

	/* Reset DMA */
	fsync((info.fd_dac[dac]));

	return SUCCESS;
}

/*
 * The API is a wrapper function used as a bridge with the command interface.
 * The function disable hardware fifo and disable datapath memory.
 */
int disable_mem(void)
{
	int ret;

	/* clear memory initialized for data path */
	ret = deinit_mem();
	if(ret) {
		printf("Unsable to initialise memory\n");
		return FAIL;
	}

	/* clear/release the gpio's */
	ret = deinit_gpio();
	if(ret) {
		printf("Unsable to initialise memory\n");
		return FAIL;
	}

	return SUCCESS;
}

