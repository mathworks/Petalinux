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
#include "cmd_interface.h"
#include "error_interface.h"
#include <unistd.h>

/************************** Constant Definitions *****************************/

#define SCRATCHPAD_REG_DAC 0xB0005050
#define SCRATCHPAD_REG_ADC 0xB0005054

extern XRFdc RFdcInst; /* RFdc driver instance */
extern int enTermMode;
extern int thread_stat;

char mem_path_dac[MAX_DAC][MAX_STRLEN] = {
    "/dev/plmem0", "/dev/plmem1", "/dev/plmem2", "/dev/plmem3",
    "/dev/plmem4", "/dev/plmem5", "/dev/plmem6", "/dev/plmem7",
};
char mem_path_adc[MAX_ADC][MAX_STRLEN] = {
    "/dev/plmem8",  "/dev/plmem9",  "/dev/plmem10", "/dev/plmem11",
    "/dev/plmem12", "/dev/plmem13", "/dev/plmem14", "/dev/plmem15",
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
char bram_ddr_path_dac[MAX_DAC][MAX_STRLEN] = {
    "/sys/class/plmem/plmem0/device/mem_type",
    "/sys/class/plmem/plmem1/device/mem_type",
    "/sys/class/plmem/plmem2/device/mem_type",
    "/sys/class/plmem/plmem3/device/mem_type",
    "/sys/class/plmem/plmem4/device/mem_type",
    "/sys/class/plmem/plmem5/device/mem_type",
    "/sys/class/plmem/plmem6/device/mem_type",
    "/sys/class/plmem/plmem7/device/mem_type",
};
char bram_ddr_path_adc[MAX_ADC][MAX_STRLEN] = {
    "/sys/class/plmem/plmem8/device/mem_type",
    "/sys/class/plmem/plmem9/device/mem_type",
    "/sys/class/plmem/plmem10/device/mem_type",
    "/sys/class/plmem/plmem11/device/mem_type",
    "/sys/class/plmem/plmem12/device/mem_type",
    "/sys/class/plmem/plmem13/device/mem_type",
    "/sys/class/plmem/plmem14/device/mem_type",
    "/sys/class/plmem/plmem15/device/mem_type",
};

/* GPIO0,4,8,12,16,20,24,28 */
int dac_reset_gpio[MAX_DAC] = {
    416, 420, 424, 428, 432, 436, 440, 444,
};
/* GPIO2,6,10,14,18,22,26,30  */
int dac_localstart_gpio[MAX_DAC] = {
    418, 422, 426, 430, 434, 438, 442, 446,
};
/* GPIO2,6,10,14,18,22,26,30  */
int dac_loopback_gpio[MAX_DAC] = {
    419, 421, 423, 425, 427, 429, 431, 433,
};

/* Starts from GPIO64 to GPIO67 */
int dac_select_gpio[DAC_MUX_GPIOS] = {
    480, 481, 482, 483,
};

/* GPIO32,36,40,44,48,52,56,60 */
int adc_reset_gpio[MAX_ADC] = {
    448, 452, 456, 460, 464, 468, 472, 476,
};

/* GPIO33,37,41,45,49,53,57,61 */
int adc_iq_gpio[MAX_ADC] = {
    449, 453, 457, 461, 465, 469, 473, 477,
};

/* GPIO34,38,42,46,50,54,58,62 */
int adc_localstart_gpio[MAX_ADC] = {
    450, 454, 458, 462, 466, 470, 474, 478,
};

/* Starts from GPIO80 to GPIO82 */
int adc_select_gpio[ADC_MUX_GPIOS] = {
    496, 497, 498,
};

#define DAC_MULTITILE_CTL_GPIO 484
#define DAC_MULTITILE_CTL_GPIO_STR "/sys/class/gpio/gpio484/"

#define ADC_MULTITILE_CTL_GPIO 500
#define ADC_MULTITILE_CTL_GPIO_STR "/sys/class/gpio/gpio500/"

struct rfsoc_info info;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
;

int init_mem() {
  int i, ret;
  void *base_dac;
  void *base_adc;
  u32 max_dac, max_adc;

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
      MetalLoghandler_firmware(ret, "Error configuring memory\n");
      return FAIL;
    }

    /* open memory file */
    info.fd_dac[i] = open(mem_path_dac[i], O_RDWR);
    if ((info.fd_dac[i]) < 0) {
      MetalLoghandler_firmware(METAL_LOG_ERROR, "file %s open failed\n",
                               mem_path_dac[i]);
      return FAIL;
    }
    /* map size of memory */
    info.map_dac[i] = (signed char *)mmap(0, DAC_MAP_SZ, PROT_READ | PROT_WRITE,
                                          MAP_SHARED, info.fd_dac[i], 0);
    if ((info.map_dac[i]) == MAP_FAILED) {
      MetalLoghandler_firmware(FAIL, "Error mmapping the file %d\n", i);
      return FAIL;
    }
  }
  for (i = 0; i < max_adc; i++) {
    /* Set memory type */
    ret = write_to_file(mem_type_path_adc[i], PL_MEM);
    if (ret != SUCCESS) {
      MetalLoghandler_firmware(ret, "Error configuring memory\n");
      return FAIL;
    }

    /* open memory file */
    info.fd_adc[i] = open(mem_path_adc[i], O_RDWR);
    if ((info.fd_adc[i]) < 0) {
      MetalLoghandler_firmware(FAIL, "file %s open failed\n", mem_path_adc[i]);
      return FAIL;
    }
    /* map size of memory */
    info.map_adc[i] = (signed char *)mmap(0, ADC_MAP_SZ, PROT_READ | PROT_WRITE,
                                          MAP_SHARED, info.fd_adc[i], 0);
    if ((info.map_adc[i]) == MAP_FAILED) {
      MetalLoghandler_firmware(FAIL, "Error mmapping the file %d\n", i);
      return FAIL;
    }
  }
  /* open memory file */
  info.fd_scratchpad = open("/dev/mem", O_RDWR | O_NDELAY);
  if (info.fd_scratchpad < 0) {
    MetalLoghandler_firmware(FAIL, "file /dev/mem open failed\n");
    return FAIL;
  }
  /* map size of memory */
  base_dac = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                  info.fd_scratchpad, (SCRATCHPAD_REG_DAC & ~MAP_MASK));
  if (base_dac == MAP_FAILED) {
    perror("map");
    MetalLoghandler_firmware(FAIL, "Error mmapping the file\n");
    close(info.fd_scratchpad);
    return FAIL;
  }
  info.map_scratchreg_dac = base_dac + (SCRATCHPAD_REG_DAC & MAP_MASK);
  /* map size of memory */
  base_adc = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                  info.fd_scratchpad, (SCRATCHPAD_REG_ADC & ~MAP_MASK));
  if (base_adc == MAP_FAILED) {
    perror("map");
    MetalLoghandler_firmware(FAIL, "Error mmapping the file\n");
    close(info.fd_scratchpad);
    return FAIL;
  }
  info.map_scratchreg_adc = base_adc + (SCRATCHPAD_REG_ADC & MAP_MASK);

  return SUCCESS;
}

int deinit_path(int *fd, signed char *map, unsigned int sz) {
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
}

int deinit_mem(void) {
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
      MetalLoghandler_firmware(
          ret, "Error configuring dac memory: DAC mem index: %d\n", i);
    }
    ret = write_to_file(bram_ddr_path_dac[i], BRAM);
    if (ret != SUCCESS) {
      MetalLoghandler_firmware(
          ret, "Error configuring dac memory: DAC mem index: %d\n", i);
    }
    deinit_path(&info.fd_dac[i], info.map_dac[i], DAC_MAP_SZ);
  }
  for (i = 0; i < max_adc; i++) {
    ret = write_to_file(mem_type_path_adc[i], NO_MEM);
    if (ret != SUCCESS) {
      MetalLoghandler_firmware(
          ret, "Error configuring ADC memory: ADC mem index: %d\n", i);
    }
    ret = write_to_file(bram_ddr_path_adc[i], BRAM);
    if (ret != SUCCESS) {
      MetalLoghandler_firmware(
          ret, "Error configuring dac mem type:  mem index: %d\n", i);
    }
    deinit_path(&info.fd_adc[i], info.map_adc[i], ADC_MAP_SZ);
  }
  if (info.map_scratchreg_dac != NULL) {
    if (munmap((info.map_scratchreg_dac - (SCRATCHPAD_REG_DAC & MAP_MASK)),
               MAP_SIZE) == -1)
      MetalLoghandler_firmware(METAL_LOG_ERROR,
                               "unmap failed for info.map_scratchreg\n");
  }
  if (info.fd_scratchpad) {
    close(info.fd_scratchpad);
    info.fd_scratchpad = 0;
  }
}

int init_gpio() {
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
    /* Enable/Export localstart GPIO */
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
  /* Enable/Export DAC Multi tile control GPIO */
  ret = enable_gpio(DAC_MULTITILE_CTL_GPIO);
  if (ret) {
    printf("Unable to enable multitile ctrl gpio of dac %d\n", i);
    return ret;
  }
  ret = config_gpio_op(DAC_MULTITILE_CTL_GPIO);
  if (ret) {
    printf("Unable to set direction for multitile ctrl of dac %d\n", i);
    return ret;
  }
  /* Enable/Export DAC SSR control GPIO */
  ret = enable_gpio(DAC_SSR_CTL_GPIO);
  if (ret) {
    printf("Unable to enable SSR ctrl gpio of dac %d\n", i);
    return ret;
  }
  ret = config_gpio_op(DAC_SSR_CTL_GPIO);
  if (ret) {
    printf("Unable to set direction for SSR ctrl of dac %d\n", i);
    return ret;
  }
  /* GPIO initialistion for ADC */
  for (i = 0; i < max_adc; i++) {
    /* Enable/Export reset GPIO */
    ret = enable_gpio(adc_reset_gpio[i]);
    if (ret) {
      printf("Unable to enable reset GPIO\n");
      return ret;
    }
    ret = config_gpio_op(adc_reset_gpio[i]);
    if (ret) {
      printf("Unable to set direction for reset gpio of adc %d\n", i);
      return ret;
    }

    /* Enable/Export IQ GPIO */
    ret = enable_gpio(adc_iq_gpio[i]);
    if (ret) {
      printf("Unable to enable IQ GPIO\n");
      return ret;
    }
    ret = config_gpio_op(adc_iq_gpio[i]);
    if (ret) {
      printf("Unable to set direction for IQ gpio of adc %d\n", i);
      return ret;
    }

    /* Enable/Export localstart GPIO */
    ret = enable_gpio(adc_localstart_gpio[i]);
    if (ret) {
      printf("Unable to enable localstart gpio of adc %d\n", i);
      return ret;
    }
    ret = config_gpio_op(adc_localstart_gpio[i]);
    if (ret) {
      printf("Unable to set direction for localstart of adc %d\n", i);
      return ret;
    }
  }
  for (i = 0; i < ADC_MUX_GPIOS; i++) {
    /* Enable/Export channel select GPIO */
    ret = enable_gpio(adc_select_gpio[i]);
    if (ret) {
      printf("Unable to enable select GPIO of adc %d\n", i);
      return ret;
    }
    ret = config_gpio_op(adc_select_gpio[i]);
    if (ret) {
      printf("Unable to set direction for select gpio of adc %d\n", i);
      return ret;
    }
  }
  /* Enable/Export ADC Multi tile control GPIO */
  ret = enable_gpio(ADC_MULTITILE_CTL_GPIO);
  if (ret) {
    printf("Unable to enable multitile control gpio of adc %d\n", i);
    return ret;
  }
  ret = config_gpio_op(ADC_MULTITILE_CTL_GPIO);
  if (ret) {
    printf("Unable to set direction for multitile ctrl of adc %d\n", i);
    return ret;
  }
  /* Enable/Export ADC SSR control GPIO */
  ret = enable_gpio(ADC_SSR_CTL_GPIO);
  if (ret) {
    printf("Unable to enable SSR control gpio of adc %d\n", i);
    return ret;
  }
  ret = config_gpio_op(ADC_SSR_CTL_GPIO);
  if (ret) {
    printf("Unable to set direction for SSR ctrl of adc %d\n", i);
    return ret;
  }
}

int deinit_gpio() {
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
      MetalLoghandler_firmware(ret, "Unable to release reset GPIO of dac\n");
      return ret;
    }
    /* Release localstart GPIO */
    ret = disable_gpio(dac_localstart_gpio[i]);
    if (ret) {
      MetalLoghandler_firmware(ret,
                               "Unable to release localstart gpio of dac\n");
      return ret;
    }
    /* Release loopback GPIO */
    ret = disable_gpio(dac_loopback_gpio[i]);
    if (ret) {
      MetalLoghandler_firmware(ret,
                               "Unable to release localstart gpio of dac\n");
      return ret;
    }
  }

  for (i = 0; i < DAC_MUX_GPIOS; i++) {
    /* Release channel select GPIO */
    ret = disable_gpio(dac_select_gpio[i]);
    if (ret) {
      MetalLoghandler_firmware(ret, "Unable to release select GPIO of dac\n");
      return ret;
    }
  }
  /* Release DAC Multitile GPIO */
  ret = disable_gpio(DAC_MULTITILE_CTL_GPIO);
  if (ret) {
    MetalLoghandler_firmware(ret, "Unable to release DAC Multitile GPIO\n");
    return ret;
  }
  /* Release DAC SSR control GPIO */
  ret = disable_gpio(DAC_SSR_CTL_GPIO);
  if (ret) {
    MetalLoghandler_firmware(ret, "Unable to release DAC SSR CTRL GPIO\n");
    return ret;
  }
  /* GPIO de-initialistion for ADC */
  for (i = 0; i < max_adc; i++) {
    /* Release reset GPIO */
    ret = disable_gpio(adc_reset_gpio[i]);
    if (ret) {
      MetalLoghandler_firmware(ret, "Unable to release reset GPIO of adc\n");
      return ret;
    }
    /* Release loopback GPIO */
    ret = disable_gpio(adc_iq_gpio[i]);
    if (ret) {
      MetalLoghandler_firmware(ret, "Unable to release iq GPIO of adc\n");
      return ret;
    }
    /* Release localstart GPIO */
    ret = disable_gpio(adc_localstart_gpio[i]);
    if (ret) {
      MetalLoghandler_firmware(ret,
                               "Unable to release localstart gpio of adc\n");
      return ret;
    }
  }

  for (i = 0; i < ADC_MUX_GPIOS; i++) {
    /* Release channel select GPIO */
    ret = disable_gpio(adc_select_gpio[i]);
    if (ret) {
      MetalLoghandler_firmware(ret, "Unable to release select GPIO of adc\n");
      return ret;
    }
  }
  /* Release ADC Multitile GPIO */
  ret = disable_gpio(ADC_MULTITILE_CTL_GPIO);
  if (ret) {
    MetalLoghandler_firmware(ret, "Unable to release ADC Multitile GPIO\n");
    return ret;
  }
  /* Release ADC SSR CTRL GPIO */
  ret = disable_gpio(ADC_SSR_CTL_GPIO);
  if (ret) {
    MetalLoghandler_firmware(ret, "Unable to release ADC SSR CTRL GPIO\n");
    return ret;
  }
}

int channel_select_gpio(int *sel_gpio_path, int val, int type) {
  int ret, i;

  if (type == ADC) {
    if (info.mem_type == BRAM) {
      // update scratch pad register for BRAM mode
      *(volatile u32 *)info.map_scratchreg_adc = 0;
      for (i = 0; i < ADC_MUX_GPIOS; i++) {
        ret = set_gpio(sel_gpio_path[i], (val & (1 << i)));
        if (ret) {
          MetalLoghandler_firmware(ret, "Unable to set select GPIO value\n");
          return ret;
        }
      }
    } else {
      // update scratch pad register for DDR mode
      *(volatile u32 *)info.map_scratchreg_adc =
          ((1 << 31) | (info.scratch_value_adc & 0xFF));
    }

    ret = set_gpio(dac_select_gpio[1], 0);
    if (ret) {
      MetalLoghandler_firmware(ret, "Unable to set select GPIO value\n");
      return ret;
    }
    usleep(10);

    //	toggle gpio65
    ret = set_gpio(dac_select_gpio[1], 1);
    if (ret) {
      MetalLoghandler_firmware(ret, "Unable to set select GPIO value\n");
      return ret;
    }
  } else {
    // update scratch pad register
    *(volatile u32 *)info.map_scratchreg_dac = info.scratch_value_dac;

    ret = set_gpio(dac_select_gpio[0], 0);
    if (ret) {
      MetalLoghandler_firmware(ret, "Unable to set select GPIO value\n");
      return ret;
    }
    usleep(10);

    //	toggle gpio64
    ret = set_gpio(dac_select_gpio[0], 1);
    if (ret) {
      MetalLoghandler_firmware(ret, "Unable to set select GPIO value\n");
      return ret;
    }
  }
}

int change_fifo_stat(int fifo_id, int tile_id, int stat) {
  int ret;

  /*FIFO disable*/
  ret = XRFdc_SetupFIFO(&RFdcInst, fifo_id, tile_id, stat);
  if (ret != SUCCESS) {
    MetalLoghandler_firmware(ret, "Unable to disable FIFO\n");
    return ret;
  }
  return SUCCESS;
}

int config_all_fifo(int fifo_id, int enable) {
  int ret;
  int ct;
  int val = 0;
  u32 max_dac_tiles;
  u32 max_adc_tiles;

  if (info.design_type == DAC1_ADC1) {
    max_dac_tiles = 2;
    max_adc_tiles = 1;
	val = 1;
  } else {
    max_dac_tiles = MAX_DAC_TILE;
    max_adc_tiles = MAX_ADC_TILE;
  }

  if (fifo_id == DAC) {
    for (ct = val; ct < max_dac_tiles; ct++) {
      ret = change_fifo_stat(fifo_id, ct, enable);
      if (ret != SUCCESS) {
        MetalLoghandler_firmware(ret, "Unable to disable DAC FIFO\n");
        return ret;
      }
    }
  } else if (fifo_id == ADC) {
    for (ct = 0; ct < max_adc_tiles; ct++) {
      ret = change_fifo_stat(fifo_id, ct, enable);
      if (ret != SUCCESS) {
        MetalLoghandler_firmware(ret, "Unable to disable ADC FIFO\n");
        return ret;
      }
    }
  }

  return SUCCESS;
}

int reset_pipe(int *reset_gpio, int *localstart_gpio) {
  int i;
  int ret;
  u32 max_dac, max_adc;

  if (info.design_type == DAC1_ADC1) {
    max_dac = 1;
    max_adc = 1;
  } else {
    max_dac = MAX_DAC;
    max_adc = MAX_ADC;
  }

  /* disable local start gpio for all dac/adc's*/
  for (i = 0; i < max_dac; i++) {
    ret = set_gpio(localstart_gpio[i], 0);
    if (ret) {
      ret = GPIO_SET_FAIL;
      MetalLoghandler_firmware(ret, "unable to assert reset gpio value\n");
      goto err;
    }
  }

  /* assert reset fifo gpio for all dac/adc's*/
  for (i = 0; i < max_dac; i++) {
    ret = set_gpio(reset_gpio[i], 1);
    if (ret) {
      ret = GPIO_SET_FAIL;
      MetalLoghandler_firmware(ret, "unable to assert reset gpio value\n");
      goto err;
    }
  }
  usleep(10);
  /* De-Assert reset FIFO GPIO for all DAC/ADC's*/
  for (i = 0; i < max_dac; i++) {
    ret = set_gpio(reset_gpio[i], 0);
    if (ret) {
      ret = GPIO_SET_FAIL;
      MetalLoghandler_firmware(ret, "Unable to de-assert reset GPIO value\n");
      goto err;
    }
  }
  return SUCCESS;
err:
  return ret;
}

void SetMemtype(convData_t *cmdVals, char *txstrPtr, int *status) {
  int i;
  int ret;
  char buff[BUF_MAX_LEN] = {0};
  int *reset_gpio;
  int *localstart_gpio;
  unsigned int mem_type;
  u32 max_dac, max_adc;

  mem_type = cmdVals[0].u;
  if (info.mem_type == mem_type) {
    printf("Already in the selected mode, no need to do anything\n");
    *status = SUCCESS;
    return;
  }
  info.mem_type = mem_type;
  if (info.design_type == DAC1_ADC1) {
    max_dac = 1;
    max_adc = 1;
  } else {
    max_dac = MAX_DAC;
    max_adc = MAX_ADC;
  }
  for (i = 0; i < max_dac; i++) {
    /* Set memory type for DAC */
    ret = write_to_file(bram_ddr_path_dac[i], info.mem_type);
    if (ret != SUCCESS) {
      MetalLoghandler_firmware(ret, "Error configuring memory\n");
      goto err;
    }
  }
  for (i = 0; i < max_adc; i++) {
    /* Set memory type for ADC */
    ret = write_to_file(bram_ddr_path_adc[i], info.mem_type);
    if (ret != SUCCESS) {
      MetalLoghandler_firmware(ret, "Error configuring memory\n");
      goto err;
    }
  }
  info.scratch_value_dac = 0;
  info.scratch_value_adc = 0;
  info.channel_size = 0;
  info.invalid_size = 0;
  info.adc_channels = 0;
  info.dac_channels = 0;
  /* write to channel select GPIO */
  ret = channel_select_gpio(dac_select_gpio, 0, DAC);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to set channelselect GPIO value\n");
    goto err;
  }
  for (i = 0; i < max_dac; i++) {
    ret = set_gpio(dac_loopback_gpio[i], 0);
    if (ret) {
      ret = GPIO_SET_FAIL;
      MetalLoghandler_firmware(ret, "Unable to re-set loopback GPIO value\n");
      goto err;
    }
    /* Reset DMA */
    fsync((info.fd_dac[i]));
  }

  for (i = 0; i < 2; i++) {
    if (i == ADC) {
      reset_gpio = adc_reset_gpio;
      localstart_gpio = adc_localstart_gpio;
    } else if (i == DAC) {
      reset_gpio = dac_reset_gpio;
      localstart_gpio = dac_localstart_gpio;
    }
    ret = reset_pipe(reset_gpio, localstart_gpio);
    if (ret < 0) {
      MetalLoghandler_firmware(ret, "Unable to re-set loopback GPIO value\n");
      goto err;
    }
  }

  *status = SUCCESS;
  return;
err:
  if (enTermMode) {
    MetalLoghandler_firmware(ret, "cmd = SetMemtype\n"
                                  "mem_type = %d\n",
                             info.mem_type);
  }
  /* append error code */
  sprintf(buff, " %d", ret);
  strcat(txstrPtr, buff);
  *status = FAIL;
}

void GetMemtype(convData_t *cmdVals, char *txstrPtr, int *status) {
  char buff[BUF_MAX_LEN] = {0};

  sprintf(buff, " %d ", info.mem_type);
  strncat(txstrPtr, buff, BUF_MAX_LEN);
  *status = SUCCESS;
  return;
}

int WriteDataToMemory_mts(int dac, u32 size) {
  int ret;
  int i;
  /* write to channel select GPIO */
  ret = channel_select_gpio(dac_select_gpio, dac, DAC);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to set channelselect GPIO value\n");
    goto err;
  }
  /* Trigger DMA */
  ret = write((info.fd_dac[dac]), info.map_dac[dac],
              ((size) * sizeof(signed char)));
  if (ret < 0) {
    ret = DMA_TX_TRIGGER_FAIL;
    MetalLoghandler_firmware(ret, "Error in writing data\n");
    goto err;
  }
  ret = set_gpio(dac_loopback_gpio[dac], 1);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to re-set localstart GPIO value\n");
    goto err;
  }
  /* Reset DMA */
  fsync((info.fd_dac[dac]));

  return SUCCESS;
err:
  return ret;
}

int WriteDataToMemory_bram(u32 block_id, int tile_id, u32 size, u32 il_pair) {
  int in_val_len = 0;
  int dac;
  int ret;

  if ((size == 0) || ((size % ADC_DAC_DMA_SZ_ALIGNMENT) != 0)) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(ret, "size should be multiples of 32\n");
    in_val_len = 1;
  }

  if (size > FIFO_SIZE) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(
        ret, "Requested size is bigger than available size(%d bytes)\n",
        FIFO_SIZE);
    in_val_len = 1;
  }

  /* extract DAC number from tile_id and block_id */
  dac = (((tile_id & 0x1) << 2) | (block_id & 0x3));

  if (info.design_type == DAC1_ADC1)
	  dac = 0;

  /* get data from socket */
  ret = getSamples((info.map_dac[dac]), size);
  if (ret != size) {
    ret = RCV_SAMPLE_FAIL;
    MetalLoghandler_firmware(
        ret, "Unable to read %d bytes of data, received : %d\n", size, ret);
    goto err;
  }

  if (in_val_len) {
    ret = INVAL_ARGS;
    goto err;
  }

  info.scratch_value_dac = (1 << dac);
  /* Disable local start gpio */
  ret = set_gpio(dac_localstart_gpio[dac], 0);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to re-set localstart GPIO value\n");
    goto err;
  }
  ret = set_gpio(dac_loopback_gpio[dac], 0);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to re-set loopback GPIO value\n");
    goto err;
  }
  if (info.mts_enable_dac) {
    ret = WriteDataToMemory_mts(dac, size);
    if (ret) {
      ret = MTS_FAIL;
      MetalLoghandler_firmware(ret, "MTS setup for DAC failed\n");
      goto err;
    }
    return 0;
  }
  /* write to channel select GPIO */
  ret = channel_select_gpio(dac_select_gpio, dac, DAC);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to set channelselect GPIO value\n");
    goto err;
  }
  /* Assert and De-Assert reset FIFO GPIO */
  ret = set_gpio(dac_reset_gpio[dac], 1);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to assert reset GPIO value\n");
    goto err;
  }

  usleep(10);

  ret = set_gpio(dac_reset_gpio[dac], 0);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to de-assert reset GPIO value\n");
    goto err;
  }
  /* Enable RFDC FIFO */
  ret = change_fifo_stat(DAC, tile_id, FIFO_EN);
  if (ret != SUCCESS) {
    ret = EN_FIFO_FAIL;
    MetalLoghandler_firmware(ret, "Unable to enable FIFO\n");
    goto err;
  }
  /* Trigger DMA */
  ret = write((info.fd_dac[dac]), info.map_dac[dac],
              ((size) * sizeof(signed char)));
  if (ret < 0) {
    ret = DMA_TX_TRIGGER_FAIL;
    MetalLoghandler_firmware(ret, "Error in writing data\n");
    goto err;
  }
  ret = set_gpio(dac_loopback_gpio[dac], 1);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to re-set localstart GPIO value\n");
    goto err;
  }
  /* enable local start GPIO */
  ret = set_gpio(dac_localstart_gpio[dac], 1);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to set localstart GPIO value\n");
    goto err;
  }
  /* Reset DMA */
  fsync((info.fd_dac[dac]));
  return 0;
err:
  return ret;
}

int WriteDataToMemory_ddr(u32 block_id, int tile_id, u32 size, u32 il_pair) {
  int in_val_len = 0;
  int dac;
  int ret;
  int *reset_gpio;
  int *local_start_gpio;

  if ((size == 0) || ((size % ADC_DAC_DMA_SZ_ALIGNMENT) != 0)) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(ret, "size should be multiples of 32\n");
    in_val_len = 1;
  }

  if (size > DAC_MAP_SZ) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(
        ret, "Requested size is bigger than available size(%d bytes)\n",
        DAC_MAP_SZ);
    in_val_len = 1;
  }

  if (info.mts_enable_dac) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(ret, "MTS not supported in DDR mode\n");
    goto err;
  }

  /* extract DAC number from tile_id and block_id */
  dac = (((tile_id & 0x1) << 2) | (block_id & 0x3));

  if (info.design_type == DAC1_ADC1)
	  dac = 0;
  /* get data from socket */
  ret = getSamples((info.map_dac[dac]), size);
  if (ret != size) {
    ret = RCV_SAMPLE_FAIL;
    MetalLoghandler_firmware(
        ret, "Unable to read %d bytes of data, received : %d\n", size, ret);
    goto err;
  }

  if (in_val_len) {
    ret = INVAL_ARGS;
    goto err;
  }
  /* check if channel size is same for all channels */
  if (info.channel_size) {
    if (info.channel_size != size) {
      info.invalid_size = -1;
      ret = INVAL_ARGS;
      goto err;
    }
  }
  /* Disable local start gpio */
  ret = set_gpio(dac_localstart_gpio[dac], 0);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to re-set localstart GPIO value\n");
    goto err;
  }

  /* Assert and De-Assert reset FIFO GPIO */
  ret = set_gpio(dac_reset_gpio[dac], 1);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to assert reset GPIO value\n");
    goto err;
  }

  usleep(10);

  ret = set_gpio(dac_reset_gpio[dac], 0);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to de-assert reset GPIO value\n");
    goto err;
  }
  /* Enable RFDC FIFO */
  ret = change_fifo_stat(DAC, tile_id, FIFO_EN);
  if (ret != SUCCESS) {
    ret = EN_FIFO_FAIL;
    MetalLoghandler_firmware(ret, "Unable to enable FIFO\n");
    goto err;
  }

  /* enable local start GPIO */
  ret = set_gpio(dac_localstart_gpio[dac], 1);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to set localstart GPIO value\n");
    goto err;
  }

  info.channel_size = size;
  return 0;
err:
  return ret;
}

void WriteDataToMemory(convData_t *cmdVals, char *txstrPtr, int *status) {
  u32 block_id;
  int tile_id;
  u32 size, il_pair;
  int ret;

  tile_id = cmdVals[0].i;
  block_id = cmdVals[1].u;
  size = cmdVals[2].u;
  il_pair = cmdVals[3].u;

  if (info.mem_type == BRAM) {
    ret = WriteDataToMemory_bram(block_id, tile_id, size, il_pair);
    if (ret < 0) {
      printf("Error in executing WriteDataToMemory_bram :%d\n", ret);
      goto err;
    }
  } else {
    ret = WriteDataToMemory_ddr(block_id, tile_id, size, il_pair);
    if (ret < 0) {
      printf("Error in executing WriteDataToMemory_ddr :%d\n", ret);
      goto err;
    }
  }
  *status = SUCCESS;
  return;
err:
  if (enTermMode) {
    printf("cmd = WriteDataToMemory\n"
           "tile_id = %lu\n"
           "block_id = %d\n"
           "size = %lu\n"
           "interleaved_pair = %lu\n\n",
           tile_id, block_id, size, il_pair);
  }

  *status = FAIL;
}

int ReadDataFromMemory_mts(int adc, u32 size) {
  int ret;

  /* Reset DMA */
  fsync((info.fd_adc[adc]));

  /* wait for 1 msec for DMA SW reset */
  usleep(1000);

  /* Trigger DMA */
  ret = read((info.fd_adc[adc]), info.map_adc[adc],
             (RFDC_DATA_ADC_ALIGN * sizeof(signed char)));
  if (ret < 0) {
    MetalLoghandler_firmware(ret, "Error in reading data\n");
    ret = DMA_TX_TRIGGER_FAIL;
    goto err;
  }
  return SUCCESS;
err:
  return ret;
}

int ReadDataFromMemory_bram(u32 block_id, int tile_id, u32 size, u32 il_pair) {
  int adc;
  int ret;
  signed char *map_adc;

  if ((size == 0) || ((size % ADC_DAC_DMA_SZ_ALIGNMENT) != 0)) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(ret, "size should be multiples of 32\n");
    goto err;
  }

  if (size > FIFO_SIZE) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(
        ret, "Requested size is bigger than available size(%d bytes)\n",
        FIFO_SIZE);
    goto err;
  }

  if (il_pair > 1) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(ret, "Invalid arguments\n");
    goto err;
  }

  /* extract ADC number from tile_id and block_id */
  adc = (((tile_id & 0x3) << 1) | (block_id & 0x1));

  /* write to channel select GPIO */
  ret = channel_select_gpio(adc_select_gpio, adc, ADC);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to set channelselect GPIO value\n");
    goto err;
  }

  /* Enable iq mode */
  ret = set_gpio(adc_iq_gpio[adc], il_pair);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to re-set iq GPIO value\n");
    goto err;
  }

  if (info.mts_enable_adc) {
    ret = ReadDataFromMemory_mts(adc, size);
    if (ret) {
      ret = MTS_FAIL;
      MetalLoghandler_firmware(ret, "MTS setup for ADC failed\n");
      goto err;
    }
    goto mts_done;
  }
  /* Assert and De-Assert reset FIFO GPIO */
  ret = set_gpio(adc_reset_gpio[adc], 1);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to assert reset GPIO value\n");
    goto err;
  }

  usleep(10);

  ret = set_gpio(adc_reset_gpio[adc], 0);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to de-assert reset GPIO value\n");
    goto err;
  }
  /* Enable RFDC FIFO */
  ret = change_fifo_stat(ADC, tile_id, FIFO_EN);
  if (ret != SUCCESS) {
    ret = EN_FIFO_FAIL;
    MetalLoghandler_firmware(ret, "Unable to enable FIFO\n");
    goto err;
  }
  /* enable local start gpio */
  ret = set_gpio(adc_localstart_gpio[adc], 1);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to re-set localstart GPIO value\n");
    goto err;
  }

  /* Reset DMA */
  fsync((info.fd_adc[adc]));

  /* wait for 1 msec for DMA SW reset */
  usleep(1000);

  /* Trigger DMA */
  ret = read((info.fd_adc[adc]), info.map_adc[adc],
             (RFDC_DATA_ADC_ALIGN * sizeof(signed char)));
  if (ret < 0) {
    ret = DMA_TX_TRIGGER_FAIL;
    MetalLoghandler_firmware(ret, "Error in reading data\n");
    goto err;
  }

  /* disable ADC FIFO */
  ret = change_fifo_stat(ADC, tile_id, FIFO_DIS);
  ;
  if (ret != SUCCESS) {
    ret = DIS_FIFO_FAIL;
    MetalLoghandler_firmware(ret, "Failed to disable FIFO\n");
    goto err;
  }
  /* disable local start gpio */
  ret = set_gpio(adc_localstart_gpio[adc], 0);
  if (ret) {
    ret = GPIO_SET_FAIL;
    MetalLoghandler_firmware(ret, "Unable to re-set localstart GPIO value\n");
    goto err;
  }

mts_done:
  if (info.design_type == DAC1_ADC1) {
    map_adc = info.map_adc[adc];
    ret = sendSamples((map_adc + (128 * 1024)), (size * sizeof(signed char)));
  } else {
    ret = sendSamples(info.map_adc[adc], (size * sizeof(signed char)));
  }
  if (ret != (size * sizeof(signed char))) {
    ret = SND_SAMPLE_FAIL;
    MetalLoghandler_firmware(ret, "Unable to send %d bytes, sent %d bytes\n",
                             (size * sizeof(signed char)), ret);
    goto err;
  }

  return 0;
err:
  return ret;
}

int ReadDataFromMemory_ddr(u32 block_id, int tile_id, u32 size, u32 il_pair) {
  int adc;
  int ret;
  int adc_sel;
  signed char *map_adc;
  int i;
  struct rfsoc_channel_info adc_chaninfo;

  if ((size == 0) || ((size % ADC_DAC_DMA_SZ_ALIGNMENT) != 0)) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(ret, "size should be multiples of 32\n");
    goto err;
  }

  if (size > ADC_MAP_SZ) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(
        ret, "Requested size is bigger than available size(%d bytes)\n",
        ADC_MAP_SZ);
    goto err;
  }

  if (il_pair > 1) {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(ret, "Invalid arguments\n");
    goto err;
  }

  /* extract ADC number from tile_id and block_id */
  adc_sel = (((tile_id & 0x3) << 1) | (block_id & 0x1));

  if (info.first_read) {
    /* write to channel select GPIO */
    ret = channel_select_gpio(adc_select_gpio, adc, ADC);
    if (ret) {
      ret = GPIO_SET_FAIL;
      MetalLoghandler_firmware(ret, "Unable to re-set iq GPIO value\n");
      goto err;
    }
    memset(&adc_chaninfo, 0, sizeof(struct rfsoc_channel_info));
    adc_chaninfo.num_channels = 0;
    for (adc = 0; adc < 8; adc++) {
      if (info.scratch_value_adc & (1 << adc)) {
        adc_chaninfo.channel[adc_chaninfo.num_channels] = adc;
        adc_chaninfo.channel_size[adc_chaninfo.num_channels] =
            (size + (256 * 1024));
        adc_chaninfo.num_channels++;
        /* Enable iq mode */
        ret = set_gpio(adc_iq_gpio[adc], il_pair);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret, "Unable to re-set iq GPIO value\n");
          goto err;
        }

        /* Assert and De-Assert reset FIFO GPIO */
        ret = set_gpio(adc_reset_gpio[adc], 1);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret, "Unable to assert reset GPIO value\n");
          goto err;
        }

        usleep(10);

        ret = set_gpio(adc_reset_gpio[adc], 0);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret,
                                   "Unable to de-assert reset GPIO value\n");
          goto err;
        }

        tile_id = (adc / 2);
        /* Enable RFDC FIFO */
        ret = change_fifo_stat(ADC, tile_id, FIFO_EN);
        if (ret != SUCCESS) {
          ret = EN_FIFO_FAIL;
          MetalLoghandler_firmware(ret, "Unable to enable FIFO\n");
          goto err;
        }
        /* enable local start gpio */
        ret = set_gpio(adc_localstart_gpio[adc], 1);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret,
                                   "Unable to re-set localstart GPIO value\n");
          goto err;
        }
      }
    }
    adc = adc_sel;
    /* Reset DMA */
    fsync((info.fd_adc[adc]));

    /* wait for 1 msec for DMA SW reset */
    usleep(1000);

    /* Trigger DMA */
    ret = read((info.fd_adc[adc]), &adc_chaninfo,
               (((adc_chaninfo.channel_size[0])) * sizeof(signed char)));
    if (ret < 0) {
      ret = DMA_TX_TRIGGER_FAIL;
      MetalLoghandler_firmware(ret, "Error in reading data\n");
      goto err;
    }
    info.first_read = 0;
  }
  adc = adc_sel;
  map_adc = info.map_adc[adc];
  ret = sendSamples((map_adc + (128 * 1024)), (size * sizeof(signed char)));
  if (ret != (size * sizeof(signed char))) {
    ret = SND_SAMPLE_FAIL;
    MetalLoghandler_firmware(ret, "Unable to send %d bytes, sent %d bytes\n",
                             (size * sizeof(signed char)), ret);
    goto err;
  }
  return 0;
err:
  return ret;
}

void ReadDataFromMemory(convData_t *cmdVals, char *txstrPtr, int *status) {

  u32 block_id;
  int tile_id;
  u32 size, il_pair;
  int ret;

  tile_id = cmdVals[0].i;
  block_id = cmdVals[1].u;
  size = cmdVals[2].u;
  il_pair = cmdVals[3].u;

  if (info.mem_type == BRAM) {
    ret = ReadDataFromMemory_bram(block_id, tile_id, size, il_pair);
    if (ret < 0) {
      printf("Error in executing ReadDataFromMemory_bram :%d\n", ret);
      goto err;
    }
  } else {
    ret = ReadDataFromMemory_ddr(block_id, tile_id, size, il_pair);
    if (ret < 0) {
      printf("Error in executing ReadDataFromMemory_ddr :%d\n", ret);
      goto err;
    }
  }
  *status = SUCCESS;
  return;
err:
  if (enTermMode) {
    printf("cmd = ReadDataFromMemory\n"
           "tile_id = %d\n"
           "block_id = %lu\n"
           "size = %lu\n"
           "interleaved_pair = %lu\n\n",
           tile_id, block_id, size, il_pair);
  }

  *status = FAIL;
}

/*
 * The API is a wrapper function used as a bridge with the command interface.
 * The function disable hardware fifo and disable datapath memory.
 */
void disconnect(convData_t *cmdVals, char *txstrPtr, int *status) {
  int ret;
  char buff[BUF_MAX_LEN] = {0};

  /* disable ADC and DAC fifo */
  ret = config_all_fifo(DAC, FIFO_DIS);
  if (ret != SUCCESS) {
    ret = DIS_FIFO_FAIL;
    MetalLoghandler_firmware(ret, "Failed to disable DAC FIFO\n");
    goto err;
  }

  ret = config_all_fifo(ADC, FIFO_DIS);
  if (ret != SUCCESS) {
    ret = DIS_FIFO_FAIL;
    MetalLoghandler_firmware(ret, "Failed to disable ADC FIFO\n");
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
  if (enTermMode) {
    MetalLoghandler_firmware(ret, "cmd = shutdown\n");
  }

  /* Append Error code */
  sprintf(buff, " %d", ret);
  strcat(txstrPtr, buff);

  *status = FAIL;
}

/* Thread function for data path. It receives command/data from data socket
 */
void *datapath_t(void *args) {
  int ret;
  /* receive buffer of BUF_MAX_LEN character */
  char rcvBuf[BUF_MAX_LEN] = {0};
  /* tx buffer of BUF_MAX_LEN character */
  char txBuf[BUF_MAX_LEN] = {0};
  /* buffer len must be set to max buffer minus 1 */
  int bufLen = BUF_MAX_LEN - 1;
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
  if (ret) {
    deinit_mem();
    MetalLoghandler_firmware(ret, "Unable to initialise memory\n");
    return NULL;
  }
  /* initialise the gpio's for data path */
  ret = init_gpio();
  if (ret) {
    MetalLoghandler_firmware(ret, "Unable to initialise gpio's\n");
    deinit_gpio();
    goto gpio_init_failed;
  }
  printf("%s:Releasing lock\n", __func__);
  pthread_mutex_unlock(&count_mutex);
  info.mts_enable_adc = 0;
  info.mts_enable_dac = 0;
  info.scratch_value_dac = 0;
  info.scratch_value_adc = 0;
  info.channel_size = 0;
  info.invalid_size = 0;
  info.adc_channels = 0;
  info.dac_channels = 0;
  info.mem_type = BRAM;
whileloop:
  while (thread_stat) {
    /* get string from io interface (Non blocking) */
    numCharacters = getdataString(rcvBuf, bufLen);
    if (numCharacters > 0) {
      /* parse and run with error check */
      cmdStatus = data_cmdParse(rcvBuf, txBuf);
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
      /* clear txBuf each time anew command is received and a response returned
       */
      memset(txBuf, 0, sizeof(txBuf));

    } else if (numCharacters == 0) {
      goto whileloop;
    }
  }
gpio_init_failed:
  deinit_mem();
}
