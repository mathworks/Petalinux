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
#include "error_interface.h"
#include <unistd.h>

/************************** Constant Definitions *****************************/

extern XRFdc RFdcInst;      /* RFdc driver instance */
extern int enTermMode;
extern int thread_stat;

#define MAX_ADC 8
#define MAX_DAC 8
#define DAC_MUX_GPIOS 4
#define ADC_MUX_GPIOS 3
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
char mem_path_adc[MAX_ADC][MAX_STRLEN] = {
	"/dev/plmem8",	
	"/dev/plmem9",	
	"/dev/plmem10",
	"/dev/plmem11",
	"/dev/plmem12",
	"/dev/plmem13",
	"/dev/plmem14",
	"/dev/plmem15",
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
char mem_type_path_adc[MAX_ADC][MAX_STRLEN] = {
	"/sys/class/plmem/plmem8/device/select_mem",
	"/sys/class/plmem/plmem9/device/select_mem",
	"/sys/class/plmem/plmem10/device/select_mem",
	"/sys/class/plmem/plmem11/device/select_mem",
	"/sys/class/plmem/plmem12/device/select_mem",
	"/sys/class/plmem/plmem13/device/select_mem",
	"/sys/class/plmem/plmem14/device/select_mem",
	"/sys/class/plmem/plmem15/device/select_mem",
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

#define ADC0_RESET_GPIO	448
/* GPIO32,36,40,44,48,52,56,60 */
char adc_reset_gpio[MAX_ADC][MAX_STRLEN] = {
	"/sys/class/gpio/gpio448/",
	"/sys/class/gpio/gpio452/",
	"/sys/class/gpio/gpio456/",
	"/sys/class/gpio/gpio460/",
	"/sys/class/gpio/gpio464/",
	"/sys/class/gpio/gpio468/",
	"/sys/class/gpio/gpio472/",
	"/sys/class/gpio/gpio476/",
};

#define ADC0_IQ_GPIO	449
/* GPIO33,37,41,45,49,53,57,61 */
char adc_iq_gpio[MAX_ADC][MAX_STRLEN] = {
	"/sys/class/gpio/gpio449/",
	"/sys/class/gpio/gpio453/",
	"/sys/class/gpio/gpio457/",
	"/sys/class/gpio/gpio461/",
	"/sys/class/gpio/gpio465/",
	"/sys/class/gpio/gpio469/",
	"/sys/class/gpio/gpio473/",
	"/sys/class/gpio/gpio477/",
};

#define ADC0_LOCALSTART_GPIO	450
/* GPIO34,38,42,46,50,54,58,62 */
char adc_localstart_gpio[MAX_ADC][MAX_STRLEN] = {
	"/sys/class/gpio/gpio450/",
	"/sys/class/gpio/gpio454/",
	"/sys/class/gpio/gpio458/",
	"/sys/class/gpio/gpio462/",
	"/sys/class/gpio/gpio466/",
	"/sys/class/gpio/gpio470/",
	"/sys/class/gpio/gpio474/",
	"/sys/class/gpio/gpio478/",
};

#define ADC_SELECT_GPIO	496
/* Starts from GPIO80 to GPIO82 */
char adc_select_gpio[MAX_ADC][MAX_STRLEN] = {
	"/sys/class/gpio/gpio496/",
	"/sys/class/gpio/gpio497/",
	"/sys/class/gpio/gpio498/",
};

struct rfsoc_info info;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;;

int init_mem()
{
	int i, ret;
	
// Avnet changes
return SUCCESS;

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
	for (i = 0; i < MAX_ADC; i++) {
		/* Set memory type */
		ret = write_to_file(mem_type_path_adc[i], PL_MEM);
		if (ret != SUCCESS) {
			printf("Error configuring memroy\n");
			return FAIL;
		}

		/* open memory file */ 	
		info.fd_adc[i] = open(mem_path_adc[i], O_RDWR);
		if ((info.fd_adc[i]) < 0) {
			printf("file %s open failed\n", mem_path_adc[i]);
			return FAIL;
		}
		/* map size of memory */
		info.map_adc[i] = (signed char*)mmap(0, ADC_MAP_SZ, PROT_READ |
							PROT_WRITE, MAP_SHARED, info.fd_adc[i], 0);
		if ((info.map_adc[i]) == MAP_FAILED) {
			printf("Error mmapping the file %d\n", i);
			return FAIL;
		}
	}
	return SUCCESS;
}

int deinit_path(int *fd, signed char* map, unsigned int sz)
{
// Avnet changes
return SUCCESS;

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

// Avnet changes
return SUCCESS;

	for (i = 0; i < MAX_DAC; i++) {
		ret = write_to_file(mem_type_path_dac[i], NO_MEM);
		if (ret != SUCCESS) {
			printf("Error configuring dac memory: DAC mem index: %d\n", i);
		}
		deinit_path(&info.fd_dac[i], info.map_dac[i], DAC_MAP_SZ);
	}
	for (i = 0; i < MAX_ADC; i++) {
		ret = write_to_file(mem_type_path_adc[i], NO_MEM);
		if (ret != SUCCESS) {
			printf("Error configuring ADC memory: ADC mem index: %d\n", i);
		}
		deinit_path(&info.fd_adc[i], info.map_adc[i], ADC_MAP_SZ);
	}
}

int disable_gpio(int gpio)
{
	int ret;

// Avnet changes
return SUCCESS;

	ret = write_to_file(GPIO_REL, gpio);
	if (ret != SUCCESS) {
		printf("Unable to disable GPIO : %d\n", gpio);
		return FAIL;
	}

	return SUCCESS;
}

int enable_gpio(int gpio)
{
	int ret;

// Avnet changes
return SUCCESS;

	ret = write_to_file(GPIO_EXPORT, gpio);
	if (ret != SUCCESS) {
		printf("Unable to enable GPIO : %d\n", gpio);
		return FAIL;
	}

	return SUCCESS;
}

int set_gpio(char *path, int value)
{
	int ret;

// Avnet changes
return SUCCESS;

	ret = write_to_file(path, value);
	if (ret != SUCCESS) {
		printf("Unable to set GPIO : %s\n", path);
		return FAIL;
	}

	return SUCCESS;
}

int config_gpio_op(char *path)
{
	int ret;
	FILE *fp;

// Avnet changes
return SUCCESS;

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

// Avnet changes
return SUCCESS;

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
			printf("Unable to set direction for localstart of dac %d\n", i);
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

	/* GPIO initialistion for ADC */
	for (i = 0; i < MAX_ADC; i++) {
		/* Enable/Export reset GPIO */
		ret = enable_gpio((ADC0_RESET_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to enable reset GPIO\n");
			return ret;
		}
		strncpy(gpio_path, adc_reset_gpio[i], (MAX_STRLEN - 1));
		strncat(gpio_path, "direction", (MAX_STRLEN - 1));
		ret = config_gpio_op(gpio_path);	
		if(ret) {
			printf("Unable to set direction for reset gpio of adc %d\n", i);
			return ret;
		}

		/* Enable/Export IQ GPIO */
		ret = enable_gpio((ADC0_IQ_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to enable IQ GPIO\n");
			return ret;
		}
		strncpy(gpio_path, adc_iq_gpio[i], (MAX_STRLEN - 1));
		strncat(gpio_path, "direction", (MAX_STRLEN - 1));
		ret = config_gpio_op(gpio_path);	
		if(ret) {
			printf("Unable to set direction for IQ gpio of adc %d\n", i);
			return ret;
		}

		/* Enable/Export localstart GPIO */
		ret = enable_gpio((ADC0_LOCALSTART_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to enable localstart gpio of adc %d\n", i);
			return ret;
		}
		strncpy(gpio_path, adc_localstart_gpio[i], (MAX_STRLEN - 1));
		strncat(gpio_path, "direction", (MAX_STRLEN - 1));
		ret = config_gpio_op(gpio_path);	
		if(ret) {
			printf("Unable to set direction for localstart of adc %d\n", i);
			return ret;
		}

	}
	for (i = 0; i < ADC_MUX_GPIOS; i++) {
		/* Enable/Export channel select GPIO */
		ret = enable_gpio((ADC_SELECT_GPIO + i));
		if(ret) {
			printf("Unable to enable select GPIO of adc %d\n", i);
			return ret;
		}
		strncpy(gpio_path, adc_select_gpio[i], (MAX_STRLEN - 1));
		strncat(gpio_path, "direction", (MAX_STRLEN - 1));
		ret = config_gpio_op(gpio_path);	
		if(ret) {
			printf("Unable to set direction for select gpio of adc %d\n", i);
			return ret;
		}
	}

}

int deinit_gpio()
{
	int i, ret;
	char gpio_path[MAX_STRLEN];

// Avnet changes
return SUCCESS;
	
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

	for (i = 0; i < DAC_MUX_GPIOS; i++) {
		/* Release channel select GPIO */
		ret = disable_gpio((DAC_SELECT_GPIO + i));
		if(ret) {
			printf("Unable to release select GPIO of dac\n");
			return ret;
		}
	}
	/* GPIO de-initialistion for ADC */
	for (i = 0; i < MAX_ADC; i++) {
		/* Release reset GPIO */
		ret = disable_gpio((ADC0_RESET_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to release reset GPIO of adc\n");
			return ret;
		}
		/* Release loopback GPIO */
		ret = disable_gpio((ADC0_IQ_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to release iq GPIO of adc\n");
			return ret;
		}
		/* Release localstart GPIO */
		ret = disable_gpio((ADC0_LOCALSTART_GPIO + (i * 4)));
		if(ret) {
			printf("Unable to release localstart gpio of adc\n");
			return ret;
		}
	}

	for (i = 0; i < ADC_MUX_GPIOS; i++) {
		/* Release channel select GPIO */
		ret = disable_gpio((ADC_SELECT_GPIO + i));
		if(ret) {
			printf("Unable to release select GPIO of adc\n");
			return ret;
		}
	}
}

int channel_select_gpio(char sel_gpio_path[][MAX_STRLEN], int val, int type)
{
	int ret, i, max_loop;
	char gpio_path[MAX_STRLEN];

// Avnet changes
return SUCCESS;
	
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
	
	/*FIFO disable*/
	ret = XRFdc_SetupFIFO(&RFdcInst, fifo_id, tile_id, stat);
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
	} else if(fifo_id == ADC) {
		for(ct = 0; ct < MAX_ADC_TILE; ct++) {
			ret = change_fifo_stat(fifo_id, ct, FIFO_DIS);
			if(ret != SUCCESS) {
				printf("Unable to disable ADC FIFO\n");
				return ret;
			}
		}
	}

	return SUCCESS;
}

void WriteDataToMemory(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 block_id;
	int tile_id;
	u32 size;
	unsigned int ret;
	char buff[BUF_MAX_LEN] = {0};
	int in_val_len = 0;
	char gpio_path[MAX_STRLEN];
	u32 il_pair;
	int dac;
	int i, index = 0;
	signed short *map_dac;

	tile_id = cmdVals[0].i;
	block_id = cmdVals[1].u;
	size = cmdVals[2].u;
	il_pair = cmdVals[3].u;

// Avnet changes
ret = RCV_SAMPLE_FAIL;
goto err;
	
	if ((size == 0) || ((size % ADC_DAC_DMA_SZ_ALIGNMENT) != 0)) {
		printf("size should be multiples of 32\n");
		in_val_len = 1;
	}

	if (size > FIFO_SIZE) {
		printf("Requested size is bigger than FIFO size: %d bytes\n",
				FIFO_SIZE);
		in_val_len = 1;
	}

	/* extract DAC number from tile_id and block_id */
	dac  = (((tile_id & 0x1) << 2) | (block_id & 0x3));

	/* get data from socket */
	ret = getSamples((info.map_dac[dac]), size);
	if(ret != size) {
		printf("Unable to read %d bytes of data, received : %d\n", size, ret);
		ret = RCV_SAMPLE_FAIL;
		goto err;
	}

	if(in_val_len) {
		ret = INVAL_ARGS;
		goto err;
	}
	/* Disable local start gpio */
	strncpy(gpio_path, dac_localstart_gpio[dac], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 0);
	if(ret) {
		printf("Unable to re-set localstart GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}
	/* write to channel select GPIO */
	ret = channel_select_gpio(dac_select_gpio, dac, DAC);
	if(ret) {
		printf("Unable to set channelselect GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}
	/* Assert and De-Assert reset FIFO GPIO */
	strncpy(gpio_path, dac_reset_gpio[dac], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 1);
	if(ret) {
		printf("Unable to assert reset GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}

	usleep(10);

	ret = set_gpio(gpio_path, 0);
	if(ret) {
		printf("Unable to de-assert reset GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}
	/* Enable RFDC FIFO */
	ret = change_fifo_stat(DAC, tile_id, FIFO_EN);
	if(ret != SUCCESS) {
		printf("Unable to enable FIFO\n");
		ret = EN_FIFO_FAIL;
		goto err;
	}
	/* Trigger DMA */
	ret = write((info.fd_dac[dac]), info.map_dac[dac],
				((size) * sizeof (signed char)));
	if (ret < 0) {
		printf("Error in writing data\n");
		ret = DMA_TX_TRIGGER_FAIL;
		goto err;
	}
	/* enable local start GPIO */
	strncpy(gpio_path, dac_localstart_gpio[dac], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 1);
	if(ret) {
		printf("Unable to set localstart GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}
	/* Rest DMA */
	fsync((info.fd_dac[dac]));

	*status = SUCCESS;

	return;
err:
	if(enTermMode) {
		printf("cmd = WriteDataToMemory\n"
				"tile_id = %lu\n"
				"block_id = %d\n"
				"size = %lu\n"
				"interleaved_pair = %lu\n\n", tile_id, block_id, size, il_pair);
	}

	/* Append Error code */
	sprintf(buff, " %d", ret);
	strcat(txstrPtr, buff);

	*status = FAIL;
}

void ReadDataFromMemory(convData_t *cmdVals, char *txstrPtr, int *status)
{

	u32 block_id;
	int tile_id;
	u32 size, il_pair;
	int ret;
	char buff[BUF_MAX_LEN] = {0};
	static int adc_cal = 1;
	char *mem_type_path, *mem_ptr;
	int adc;
	char gpio_path[MAX_STRLEN];

	tile_id = cmdVals[0].i;
	block_id = cmdVals[1].u;
	size = cmdVals[2].u;
	il_pair = cmdVals[3].u;

// Avnet changes
ret = SND_SAMPLE_FAIL;
goto err;

	if ((size == 0) || ((size % ADC_DAC_DMA_SZ_ALIGNMENT) != 0)) {
		printf("size should be multiples of 32\n");
		ret = INVAL_ARGS;
		goto err;
	}

	if (size > FIFO_SIZE) {
		printf("Requested size is bigger than FIFO size: %d bytes\n",
				FIFO_SIZE);
		ret = INVAL_ARGS;
		goto err;
	}

	if(il_pair > 1) {
		printf("Invalid arguments\n");
		ret = INVAL_ARGS;
		goto err;
	}

	/* extract ADC number from tile_id and block_id */
	adc  = (((tile_id & 0x3) << 1) | (block_id & 0x1));
	
	/* write to channel select GPIO */
	ret = channel_select_gpio(adc_select_gpio, adc, ADC);
	if(ret) {
		printf("Unable to set channelselect GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}

	/* Enable iq mode */
	strncpy(gpio_path, adc_iq_gpio[adc], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, il_pair);
	if(ret) {
		printf("Unable to re-set iq GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}
	/* Assert and De-Assert reset FIFO GPIO */
	strncpy(gpio_path, adc_reset_gpio[adc], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 1);
	if(ret) {
		printf("Unable to assert reset GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}

	usleep(10);

	ret = set_gpio(gpio_path, 0);
	if(ret) {
		printf("Unable to de-assert reset GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}
	/* Enable RFDC FIFO */
	ret = change_fifo_stat(ADC, tile_id, FIFO_EN);
	if(ret != SUCCESS) {
		printf("Unable to enable FIFO\n");
		ret = EN_FIFO_FAIL;
		goto err;
	}
	/* enable local start gpio */
	strncpy(gpio_path, adc_localstart_gpio[adc], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 1);
	if(ret) {
		printf("Unable to re-set localstart GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}

	/* Rest DMA */
	fsync((info.fd_adc[adc]));

	/* wait for 1 msec for DMA SW reset */
	usleep(1000);

	/* Trigger DMA */
	ret = read((info.fd_adc[adc]), info.map_adc[adc],
			   (RFDC_DATA_ADC_ALIGN * sizeof (signed char)));
	if (ret < 0) {
		printf("Error in reading data\n");
		ret = DMA_TX_TRIGGER_FAIL;
		goto err;
	}

	/* disable ADC FIFO */
	ret = change_fifo_stat(ADC, tile_id, FIFO_DIS);;
	if(ret != SUCCESS) {
		printf("Failed to disable FIFO\n");
		ret = DIS_FIFO_FAIL;
		goto err;
	}
	/* disable local start gpio */
	strncpy(gpio_path, adc_localstart_gpio[adc], (MAX_STRLEN - 1));
	strncat(gpio_path, "value", (MAX_STRLEN - 1));
	ret = set_gpio(gpio_path, 0);
	if(ret) {
		printf("Unable to re-set localstart GPIO value\n");
		ret = GPIO_SET_FAIL;
		goto err;
	}

	ret = sendSamples(info.map_adc[adc], (size * sizeof (signed char)));
	if(ret != (size * sizeof (signed char))) {
		printf("Unable to send %d bytes, sent %d bytes\n",
						(size * sizeof (signed char)), ret);
		ret = SND_SAMPLE_FAIL;
		goto err;
	}

	*status = SUCCESS;
	return;
err:
	if(enTermMode) {
		printf("cmd = ReadDataFromMemory\n"
				"tile_id = %d\n"
				"block_id = %lu\n"
				"size = %lu\n"
				"interleaved_pair = %lu\n\n", tile_id, block_id, size, il_pair);
	}


	/* Append Error code */
	sprintf(buff, " %d", ret);
	strcat(txstrPtr, buff);
	*status = FAIL;
}

/*
 * The API is a wrapper function used as a bridge with the command interface.
 * The function disable hardware fifo and disable datapath memory.
 */
void disconnect(convData_t *cmdVals, char *txstrPtr, int *status)	
{
	int ret;
	char buff[BUF_MAX_LEN] = {0};

	/* disable ADC and DAC fifo */
	ret = disable_all_fifo(DAC);
	if(ret != SUCCESS) {
		printf("Failed to disable DAC FIFO\n");
		ret = DIS_FIFO_FAIL;
		goto err;
	}

	ret = disable_all_fifo(ADC);
	if(ret != SUCCESS) {
		printf("Failed to disable ADC FIFO\n");
		ret = DIS_FIFO_FAIL;
		goto err;
	}

	printf("%s: waiting for lock\n", __func__);
	pthread_mutex_lock(&count_mutex);		
	printf("%s: acquired lock\n", __func__);
	/* clear memory initialized for data path */
	deinit_mem();
	/* clear/release the gpio's */
	deinit_gpio();
	printf("%s:Releasing lock\n", __func__);
	pthread_mutex_unlock(&count_mutex);		
	
	*status = SUCCESS;
	return;

err:
	if(enTermMode) {
		printf("cmd = shutdown\n");
	}

	/* Append Error code */
	sprintf(buff, " %d", ret);
	strcat(txstrPtr, buff);


	*status = FAIL;

}

/* Thread function for data path. It receives command/data from data socket
 */
void *datapath_t(void *args)
{
	int ret;
	/* receive buffer of BUF_MAX_LEN character */
	char rcvBuf[BUF_MAX_LEN]={0};
	/* tx buffer of BUF_MAX_LEN character */
	char txBuf[BUF_MAX_LEN]={0};
	/* buffer len must be set to max buffer minus 1 */
	int bufLen = BUF_MAX_LEN-1;
	/* number of character received per command line */
	int numCharacters;
	/* status of the command: XST_SUCCES - ERROR_CMD_UNDEF
	 * - ERROR_NUM_ARGS - ERROR_EXECUTE
	 */
	int cmdStatus;

	/* initialse the memory for data path */
	printf("%s: waiting for lock\n", __func__);
	pthread_mutex_lock(&count_mutex);		
	printf("%s: acquired lock\n", __func__);
	ret = init_mem();
   	if(ret) {
		deinit_mem();
		printf("Unable to initialise memory\n");
		return NULL;
	}
	/* initialise the gpio's for data path */
	ret = init_gpio();
   	if(ret) {
		printf("Unable to initialise gpio's\n");
		deinit_gpio();
		goto gpio_init_failed;
	}
	printf("%s:Releasing lock\n", __func__);
	pthread_mutex_unlock(&count_mutex);		
whileloop:
	while(thread_stat) {
		/* get string from io interface (Non blocking) */
		numCharacters = getdataString(rcvBuf, bufLen);
		if(numCharacters > 0) {
			/* parse and run with error check */
			cmdStatus = data_cmdParse(rcvBuf,txBuf);
			/* check cmParse status - return an error message or the response */
			if (cmdStatus != SUCCESS) {
				/* command returned an errors */
				errorIf_data(txBuf, cmdStatus);
			} else {
				/* send response */
				senddataString(txBuf, strlen(txBuf));
			}

			/* clear rcvBuf each time anew command is received and processed */
			memset(rcvBuf, 0, sizeof(rcvBuf));
			/* clear txBuf each time anew command is received and a response returned */
			memset(txBuf, 0, sizeof(txBuf));

		} else if(numCharacters == 0) {
			goto whileloop;
		}
	}
gpio_init_failed:
	deinit_mem();
}
