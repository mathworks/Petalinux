/******************************************************************************
*
* Copyright (C) 2018 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sub license, and/or sell
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

#include "local_mem.h"
#include "data_interface.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern int enTermMode; /* Enable printing in terminal mode */
extern XRFdc RFdcInst;
/*
 *  Get local Mem Info bridge
 */

void LocalMemInfo(convData_t *cmdVals, char *txstrPtr, int *status) {
  int numTiles;    /* value of parameter tiles set on the stimulus gen or data
                      capture IPI */
  int numMem;      /* number of stim gen or data capture memory channels */
  int memSize;     /* stim gen or data capture memory channel size */
  int numWords;    /* streaming interface number of words per clk cycle */
  u32 memBaseAddr; /* AXI Base address */
  int mem_enable;
  int mem_clksel;
  char Response[BUF_MAX_LEN] = {
      0}; /* hold command response to return to the host */

  if (cmdVals[0].i == 0) {
    numTiles = 4;
    memBaseAddr = 82000000;
    mem_enable = 0;
    mem_clksel = 0;
  } else {
    numTiles = 2;
    memBaseAddr = 86000000;
    mem_enable = 0;
    mem_clksel = 1;
  }
  if (info.mem_type == BRAM) {
    if (cmdVals[0].i == 0) {
      mem_enable = info.adc_channels;
    } else {
      mem_enable = info.dac_channels;
    }
  }

  numMem = 8;
  memSize = (128 * 1024);
  numWords = 16;

  if (*status == XRFDC_SUCCESS) {
    sprintf(Response, " %d %08X %d %d %d %d %d %d", cmdVals[0].i, memBaseAddr,
            numTiles, numMem * 2, memSize, numWords, mem_enable, mem_clksel);
    strncat(txstrPtr, Response, BUF_MAX_LEN);
  }
}

/*
 * Function is returning the address for each channel
 */
void LocalMemAddr(convData_t *cmdVals, char *txstrPtr, int *status) {
  *status = SUCCESS;
}

/*
 *  Function is setting the number of sample on a channel
 */
void SetLocalMemSample(convData_t *cmdVals, char *txstrPtr, int *status) {
  *status = SUCCESS;
}

/*
 *  Get local Mem Info hw
 */
void GetMemInfoHw(u32 mem, int *num_tiles, int *num_mem, int *mem_size,
                  int *num_words) {
  u32 data;

  data = lmem_rd32(RFdcInst.io, (mem + LMEM_INFO));

  if (num_words != NULL) {
    *num_words = (data)&0x03F;
  } /* 6 bit wide result (5:0) */
  if (mem_size != NULL) {
    *mem_size = (data >> 6) & 0x7FFFF;
  } /* 19 bit wide result (24:6) */
  if (num_mem != NULL) {
    *num_mem = (data >> 25) & 0x07;
  } /*  3 bit wide result (27:25) */
  if (num_tiles != NULL) {
    *num_tiles = (data >> 28) & 0x07;
  } /*  3 bit wide result (30:28) */
}

void LocalMemTrigger(convData_t *cmdVals, char *txstrPtr, int *status) {
  int ret;
  u32 type;
  int *localstart_gpio;
  int *reset_gpio;
  int i;
  u32 channels;
  u32 size;
  struct rfsoc_channel_info dac_chaninfo;
  u32 max_dac, max_adc;

  if (info.design_type == DAC1_ADC1) {
    max_dac = 1;
    max_adc = 1;
  } else {
    max_dac = MAX_DAC;
    max_adc = MAX_ADC;
  }
  type = cmdVals[0].u;
  size = cmdVals[2].u;
  channels = cmdVals[3].u;
  if (type == ADC)
    info.adc_channels |= channels;
  else
    info.dac_channels |= channels;
  /*
   * currently first argument is swapped for this command
   * if arg1 is 0 then it is DAC and if arg1 is 1 it is ADC
   */
  if ((type == ADC) && info.mts_enable_adc) {
    reset_gpio = adc_reset_gpio;
    localstart_gpio = adc_localstart_gpio;

    /* disable global start */
    ret = set_gpio(localstart_gpio[0], 0);
    if (ret) {
      ret = GPIO_SET_FAIL;
      MetalLoghandler_firmware(METAL_LOG_ERROR,
                               "unable to assert adc global start gpio\n");
      goto err;
    }

    /* assert reset fifo gpio for all adc's */
    for (i = 0; i < max_dac; i++) {
      ret = set_gpio(reset_gpio[i], 1);
      if (ret) {
        ret = GPIO_SET_FAIL;
        MetalLoghandler_firmware(METAL_LOG_ERROR,
                                 "unable to assert reset gpio value\n");
        goto err;
      }
    }

    usleep(10);

    /* De-Assert reset FIFO GPIO for all adc's */
    for (i = 0; i < max_dac; i++) {
      ret = set_gpio(reset_gpio[i], 0);
      if (ret) {
        ret = GPIO_SET_FAIL;
        MetalLoghandler_firmware(METAL_LOG_ERROR,
                                 "Unable to de-assert reset GPIO value\n");
        goto err;
      }
    }

    /* enable global start */
    ret = set_gpio(localstart_gpio[0], 1);
    if (ret) {
      ret = GPIO_SET_FAIL;
      MetalLoghandler_firmware(METAL_LOG_ERROR,
                               "unable to assert adc global start gpio\n");
      goto err;
    }

  } else if ((type == DAC) && info.mts_enable_dac) {
    localstart_gpio = dac_localstart_gpio;
    ret = set_gpio(localstart_gpio[0], 1);
    if (ret) {
      ret = GPIO_SET_FAIL;
      MetalLoghandler_firmware(METAL_LOG_ERROR,
                               "unable to set dac global start gpio\n");
      goto err;
    }
  } else if ((info.mts_enable_dac == 0) && (info.mts_enable_adc == 0)) {
    if (info.design_type == MTS) {
      ret = INVAL_ARGS;
      MetalLoghandler_firmware(ret, "Design doesnot support non-MTS mode\n");
      goto err;
    } else if ((info.design_type == DAC1_ADC1) && (((type == DAC) && ((channels != 0x80) && (channels != 0))) || ((type == ADC) && channels > 1))) {
      ret = INVAL_ARGS;
      MetalLoghandler_firmware(ret, "Design support only one channel\n");
      goto err;
    }
    if ((type == DAC) && (info.mem_type == DDR)) {
      if (channels == 0) {
        info.scratch_value_dac = 0;
        info.channel_size = 0;
        /* Reset DMA */
        fsync((info.fd_dac[0]));
        memset(&dac_chaninfo, 0, sizeof(struct rfsoc_channel_info));
		/* disable local start and reset FIFO's */
		ret = reset_pipe(dac_reset_gpio, dac_localstart_gpio);
		if (ret < 0) {
			MetalLoghandler_firmware(METAL_LOG_ERROR,"un-able to reset DAC\n");
			goto err;
		}
        *status = SUCCESS;
        return;
      }

      if (info.invalid_size < 0) {
        ret = FAIL;
        MetalLoghandler_firmware(
            METAL_LOG_ERROR, "In TDM size should be same for all channels\n");
        info.invalid_size = 0;
        info.channel_size = 0;
        goto err;
      }

  	  if (info.design_type == DAC1_ADC1)
		  channels = 1;
      info.scratch_value_dac = channels;
      // update scratch pad register
      *(volatile u32 *)info.map_scratchreg_dac = info.scratch_value_dac;

      ret = set_gpio(dac_select_gpio[0], 0);
      if (ret) {
        ret = GPIO_SET_FAIL;
        MetalLoghandler_firmware(METAL_LOG_ERROR,
                                 "Unable to set select GPIO value\n");
        info.channel_size = 0;
        goto err;
      }
      usleep(10);

      //	toggle gpio64
      ret = set_gpio(dac_select_gpio[0], 1);
      if (ret) {
        ret = GPIO_SET_FAIL;
        MetalLoghandler_firmware(METAL_LOG_ERROR,
                                 "Unable to set select GPIO value\n");
        info.channel_size = 0;
        goto err;
      }

      usleep(10);

      dac_chaninfo.num_channels = 0;
      for (i = 0; i < 8; i++) {
        if (channels & (1 << i)) {
          dac_chaninfo.channel[dac_chaninfo.num_channels] = i;
          dac_chaninfo.channel_size[dac_chaninfo.num_channels] =
              info.channel_size;
          dac_chaninfo.num_channels++;
        }
      }

      if (dac_chaninfo.num_channels == 0) {
        ret = FAIL;
        MetalLoghandler_firmware(METAL_LOG_ERROR, "No channel is selected\n");
        info.channel_size = 0;
        goto err;
      }

      /* Trigger DMA */
      ret = write((info.fd_dac[0]), (void *)&dac_chaninfo,
                  ((dac_chaninfo.channel_size[0]) * sizeof(signed char)));
      if (ret < 0) {
        ret = DMA_TX_TRIGGER_FAIL;
        MetalLoghandler_firmware(METAL_LOG_ERROR, "Error in writing data\n");
        info.channel_size = 0;
        goto err;
      }
      info.channel_size = 0;
    } else if ((type == ADC) && (info.mem_type == DDR)) {
      if (channels == 0) {
        info.scratch_value_adc = 0;
        info.first_read = 0;
        /* Reset DMA */
        fsync((info.fd_adc[0]));
        *status = SUCCESS;
        return;
      }
      info.scratch_value_adc = channels;
      info.first_read = 1;
    } else if ((type == DAC) && (info.mem_type == BRAM)) {
      if (channels == 0) {
        for (i = 0; i < max_dac; i++) {
          /* Disable local start gpio */
          ret = set_gpio(dac_loopback_gpio[i], 0);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "Unable to re-set loopback GPIO value\n");
            goto err;
          }
        }
        /* disable local start and reset FIFO's */
        ret = reset_pipe(dac_reset_gpio, dac_localstart_gpio);
        if (ret < 0) {
          MetalLoghandler_firmware(METAL_LOG_ERROR, "un-able to reset DAC\n");
          goto err;
        }
      }
    }
    *status = SUCCESS;
    return;
  } else {
    ret = INVAL_ARGS;
    MetalLoghandler_firmware(
        METAL_LOG_ERROR,
        "unsupported type or MTS not enabled for selected type\n");
    goto err;
  }

  *status = SUCCESS;
  return;

err:
  if (enTermMode) {
    printf("**********localMemTrigger()***********\r\n");
    printf("Local mem type (input from host):   %d\r\n", cmdVals[0].i);
    printf("Local mem clk sel:   %d\r\n", cmdVals[1].u);
    printf("Local mem num samples:  %d\r\n", cmdVals[2].u);
    printf("Local mem channel enabled:   %d\r\n", cmdVals[3].u);
  }
  *status = FAIL;
}
