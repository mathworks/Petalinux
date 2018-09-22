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
#include <unistd.h>

/************************** Constant Definitions *****************************/
extern XRFdc RFdcInst;      /* RFdc driver instance */

#define MAX_ADC 8
#define MAX_DAC 8
#define MAX_STRLEN 512

struct rfsoc_info {
	signed char *map_dac[MAX_DAC];
	signed char *map_adc[MAX_ADC];
	int fd_dac[MAX_DAC];
	int fd_adc[MAX_ADC];
	int adc_status, dac_status;
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

#define DAC0_RESET_GPIO	416
/* GPIO0,4,8,12,16,20,24,28 */
char dac_reset_gpio[MAX_DAC][MAX_STRLEN] = {
	"/sys/class/gpio/gpio416/",
	"/sys/class/gpio/gpio420/",
	"/sys/class/gpio/gpio424/",
	"/sys/class/gpio/gpio428/",
	"/sys/class/gpio/gpio432/",
	"/sys/class/gpio/gpio436/",
	"/sys/class/gpio/gpio440/",
	"/sys/class/gpio/gpio444/",
};

#define DAC0_LOCALSTART_GPIO	418
/* GPIO2,6,10,14,18,22,26,30  */
char dac_localstart_gpio[MAX_DAC][MAX_STRLEN] = {
	"/sys/class/gpio/gpio418/",
	"/sys/class/gpio/gpio422/",
	"/sys/class/gpio/gpio426/",
	"/sys/class/gpio/gpio430/",
	"/sys/class/gpio/gpio434/",
	"/sys/class/gpio/gpio438/",
	"/sys/class/gpio/gpio442/",
	"/sys/class/gpio/gpio446/",
};

#define DAC_SELECT_GPIO	480
/* Starts from GPIO64 to GPIO67 */
char dac_select_gpio[MAX_DAC][MAX_STRLEN] = {
	"/sys/class/gpio/gpio480/",
	"/sys/class/gpio/gpio481/",
	"/sys/class/gpio/gpio482/",
	"/sys/class/gpio/gpio483/",
};

struct rfsoc_info info;

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
int init_mem()
{
	int i, ret;

	for (i = 0; i < MAX_DAC; i++) {
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

	for (i = 0; i < MAX_DAC; i++) {
		ret = write_to_file(mem_type_path_dac[i], NO_MEM);
		if (ret != SUCCESS) {
			printf("Error releasing memory\n");
			return FAIL;
		}
		deinit_path(&info.fd_dac[i], info.map_dac[i], DAC_MAP_SZ);
	}

	return SUCCESS;
}

int disable_gpio(int gpio)
{
	int ret;

	ret = write_to_file(GPIO_REL, gpio);
	if (ret != SUCCESS) {
		printf("Unable to disable GPIO : %d\n", gpio);
		return FAIL;
	}

	return ret;
}

int enable_gpio(int gpio)
{
	int ret;

	ret = write_to_file(GPIO_EXPORT, gpio);
	if (ret != SUCCESS) {
		printf("Unable to enable GPIO : %d\n", gpio);
		return FAIL;
	}

	return ret;
}

int set_gpio(char *path, int value)
{
	int ret;

	ret = write_to_file(path, value);
	if (ret != SUCCESS) {
		printf("Unable to set GPIO : %s\n", path);
		return FAIL;
	}

	return ret;
}

int config_gpio_op(char *path)
{
	int ret;
	FILE *fp;

	fp = fopen(path, "w");
	if (fp == NULL) {
		printf("Cannot open file %s \n", path);
		return FAIL;
	}

	ret = fprintf(fp, "%s", "out");
	if(ret <= 0) {
		printf("Unable to configure GPIO\n");
		fclose(fp);
		return FAIL;
	}

	fclose(fp);

	return SUCCESS;
}

int init_gpio()
{
	int i, ret;
	char gpio_path[MAX_STRLEN];

	for (i = 0; i < MAX_DAC; i++) {
		/* Enable/Export reset GPIO */
		ret = enable_gpio((DAC0_RESET_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to enable reset GPIO\n");
			return ret;
		}
		strncpy(gpio_path, dac_reset_gpio[i], (MAX_STRLEN - 1));
		strncat(gpio_path, "direction", (MAX_STRLEN - 1));
		ret = config_gpio_op(gpio_path);	
		if(ret) {
			printf("Unable to set direction for reset gpio of dac %d\n", i);
			return ret;
		}
		/* Enable/Export localstart GPIO */
		ret = enable_gpio((DAC0_LOCALSTART_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to enable localstart gpio of dac %d\n", i);
			return ret;
		}
		strncpy(gpio_path, dac_localstart_gpio[i], (MAX_STRLEN - 1));
		strncat(gpio_path, "direction", (MAX_STRLEN - 1));
		ret = config_gpio_op(gpio_path);	
		if(ret) {
			printf("Unable to set direction for localstart gpio of dac: %d\n",
																			i);
			return ret;
		}
	}
	for (i = 0; i < DAC_MUX_GPIOS; i++) {
		/* Enable/Export channel select GPIO */
		ret = enable_gpio((DAC_SELECT_GPIO + i));
		if(ret) {
			printf("Unable to enable select GPIO of dac %d\n", i);
			return ret;
		}
		strncpy(gpio_path, dac_select_gpio[i], (MAX_STRLEN - 1));
		strncat(gpio_path, "direction", (MAX_STRLEN - 1));
		ret = config_gpio_op(gpio_path);	
		if(ret) {
			printf("Unable to set direction for select gpio of dac %d\n", i);
			return ret;
		}
	}

	return SUCCESS;
}

int deinit_gpio()
{
	int i, ret;
	char gpio_path[MAX_STRLEN];

	for (i = 0; i < MAX_DAC; i++) {
		/* Release reset GPIO */
		ret = disable_gpio((DAC0_RESET_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to release reset GPIO of dac\n");
			return ret;
		}
		/* Release localstart GPIO */
		ret = disable_gpio((DAC0_LOCALSTART_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to release localstart gpio of dac\n");
			return ret;
		}
	}

	for (i = 0; i < 4; i++) {
		/* Release channel select GPIO */
		ret = disable_gpio((DAC_SELECT_GPIO + i));
		if(ret) {
			printf("Unable to release select GPIO of dac\n");
			return ret;
		}
	}

}

int channel_select_gpio(char sel_gpio_path[][MAX_STRLEN], int val, int type)
{
	int ret, i, max_loop;
	char gpio_path[MAX_STRLEN];

	if (type == ADC)
		max_loop = ADC_MUX_GPIOS;
	else
		max_loop = DAC_MUX_GPIOS;

	for (i = 0; i < max_loop; i++) {
		strncpy(gpio_path, sel_gpio_path[i], (MAX_STRLEN - 1));
		strncat(gpio_path, "value", (MAX_STRLEN - 1));
		ret = set_gpio(gpio_path, (val & (1 << i)));
		if(ret) {
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

	if(fifo_id == DAC) {
		for(ct = 0; ct < MAX_DAC_TILE; ct++) {
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

	/* Copy data to DMAble memory*/
	memcpy(info.map_dac[dac], data, size);

	strncpy(gpio_path, dac_localstart_gpio[dac], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 0);
	if(ret) {
		printf("Unable to re-set localstart GPIO value\n");
		return FAIL;
	}
	/* write to channel select GPIO */
	ret = channel_select_gpio(dac_select_gpio, dac, DAC);
	if(ret) {
		printf("Unable to set channelselect GPIO value\n");
		return FAIL;
	}
	/* Assert and De-Assert reset FIFO GPIO */
	strncpy(gpio_path, dac_reset_gpio[dac], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 1);
	if(ret) {
		printf("Unable to assert reset GPIO value\n");
		return FAIL;
	}

	usleep(10);

	ret = set_gpio(gpio_path, 0);
	if(ret) {
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
	/* enable local start GPIO */
	strncpy(gpio_path, dac_localstart_gpio[dac], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 1);
	if(ret) {
		printf("Unable to set localstart GPIO value\n");
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

