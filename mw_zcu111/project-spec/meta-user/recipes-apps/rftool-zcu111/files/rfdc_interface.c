/******************************************************************************
*
* Copyright (C) 2018-2021 Xilinx, Inc.  All rights reserved.
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
/***************************** Include Files ********************************/
#include "rfdc_interface.h"
/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/
/************************** Function Prototypes *****************************/

/************************** Variable Definitions ****************************/

XRFdc RFdcInst;      /* RFdc driver instance */

int rfdc_inst_init(u16 rfdc_id) {
	XRFdc_Config *ConfigPtr;
	XRFdc *RFdcInstPtr = &RFdcInst;
	int Status;
	struct metal_device *device;
	int ret = 0;
  struct metal_init_params init_param = {
      .log_handler = MetalLoghandler, .log_level = METAL_LOG_ERROR,
	};

	if (metal_init(&init_param)) {
		sendString("ERROR: metal_init METAL_LOG_INIT_FAILURE", 40);
		return XRFDC_FAILURE;
	}

	/* Initialize the RFdc driver. */
	ConfigPtr = XRFdc_LookupConfig(rfdc_id);

  Status = XRFdc_RegisterMetal(RFdcInstPtr, rfdc_id, &device);
  if (Status != XRFDC_SUCCESS) {
    sendString("ERROR: XRFdc_RegisterMetal METAL_DEV_REGISTER_FAILURE", 53);
    return XRFDC_FAILURE;
  }

  /* Initializes the controller */
	Status = XRFdc_CfgInitialize(RFdcInstPtr, ConfigPtr);
	if (Status != XRFDC_SUCCESS) {
		sendString("ERROR: XRFdc_CfgInitialize RFDC_CFG_INIT_FAILURE", 48);
		return XRFDC_FAILURE;
	}
        

  return XRFDC_SUCCESS;
	}

int rfdc_init(void) {
  int i, ret, fd, val = 0;
  u32 max_dac_tiles, max_adc_tiles, value;
  void *base;

  /* open memory file */
  fd = open("/dev/mem", O_RDWR | O_NDELAY);
  if (fd < 0) {
    MetalLoghandler_firmware(
        FAIL, "Failed to load bitstream - file /dev/mem open failed\n");
    return FAIL;
  }
  /* map size of memory */
  base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
              (DESIGN_TYPE_REG & ~MAP_MASK));
  if (base == MAP_FAILED) {
    perror("map");
    MetalLoghandler_firmware(
        FAIL, "Failed to load bitstream - Error mmapping the file\n");
    close(fd);
    return FAIL;
	}

  /* Read design type register from the HW */
  info.map_design_type_reg = base + (DESIGN_TYPE_REG & MAP_MASK);
  info.design_type = 0;
  value = *(u32 *)(info.map_design_type_reg);
  info.design_type = (value & (3 << 16));
  info.design_type = (info.design_type >> 16);

  /*
   * SSR IP design supports only one DAC(DAC7) and one ADC(ADC0)
   * As DAC7 is supported only DAC tile 1 should be enabled/disabled.
   */
  if (info.design_type == DAC1_ADC1) {
    max_dac_tiles = 2;
    max_adc_tiles = 1;
    val = 1;
  } else if ((info.design_type == NON_MTS) || (info.design_type == MTS)) {
    max_dac_tiles = MAX_DAC_TILE;
    max_adc_tiles = MAX_ADC_TILE;
  } else {
    close(fd);
    munmap(base, MAP_SIZE);
    MetalLoghandler_firmware(FAIL, "Failed to load bitstream\n");
  }
  close(fd);
  munmap(base, MAP_SIZE);

  /* initialize RFDC instance */
  ret = rfdc_inst_init(RFDC_DEVICE_ID);
  if (ret != XRFDC_SUCCESS) {
    printf("Failed to initialize RFDC instance\n");
	}

  ret = initRFclock(ZCU111, LMK04208_12M8_3072M_122M88_REVA, DAC_245_76_MHZ,
                    DAC_245_76_MHZ, ADC_245_76_MHZ);
  if (ret != SUCCESS) {
    printf("Unable to enable RF clocks\n");
    return ret;
  }

  /* Disable DAC fifo */
  ret = config_all_fifo(DAC, FIFO_DIS);
  if (ret != SUCCESS) {
    printf("Failed to disable DAC FIFO\n");
    return ret;
  }

  /* Disable ADC fifo */
  ret = config_all_fifo(ADC, FIFO_DIS);
  if (ret != SUCCESS) {
    printf("Failed to disable ADC FIFO\n");
    return ret;
}

  for (i = 0; i < max_adc_tiles; i++) {
    ret = XRFdc_DynamicPLLConfig(&RFdcInst, ADC, i, 1, 245.76, 3194.88);
    if (ret != SUCCESS) {
      printf("could not set ADC tile %d to 3194.88freq ret = %d\n", i, ret);
      return ret;
    }
  }

  for (i = val; i < max_dac_tiles; i++) {
    ret = XRFdc_DynamicPLLConfig(&RFdcInst, DAC, i, 1, 245.76, 6389.76);
    if (ret != SUCCESS) {
      printf("could not set DAC tile %d to 6389.76freq ret = %d\n", i, ret);
      return ret;
    }
  }
  return SUCCESS;
}
