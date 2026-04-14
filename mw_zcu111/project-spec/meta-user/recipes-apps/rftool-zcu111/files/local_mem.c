/******************************************************************************
*
* Copyright (C) 2018-2021 Xilinx, Inc.  All rights reserved.
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

  *status = SUCCESS;
  if (*status == SUCCESS) {
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
  u32 adc = 0, dac = 0;
  struct rfsoc_channel_info dac_chaninfo;
  u32 max_dac, max_adc;
  u32 tile_id;
  static unsigned int mask_prev;

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

  if ((info.mts_enable_dac == 0) && (info.mts_enable_adc == 0)) {
    if (info.design_type == MTS) {
      ret = INVAL_ARGS;
      MetalLoghandler_firmware(ret, "Design doesnot support non-MTS mode\n");
      goto err;
    } else if ((info.design_type == DAC1_ADC1) &&
               (((type == DAC) && ((channels != 0x80) && (channels != 0))) ||
                ((type == ADC) && channels > 1))) {

  /*
   * SSR IP design supports only one DAC(DAC7) and one ADC(ADC0)
   * So if channel selected is other than DAC7 or ADC0 return error.
   */
      ret = INVAL_ARGS;
      MetalLoghandler_firmware(ret, "Design support only one channel\n");
      goto err;
    }
  }

  if (type == DAC) {
    /*
     * channel mask is 0, so it is stop command.
     * Reset the pipies and clean-up DAC/ADC.
     */
    if (channels == 0) {
      if (info.mem_type == DDR) {
        info.scratch_value_dac = 0;
        for (i = 0; i < max_dac; i++)
          info.channel_size[i] = 0;
        /* reset dma */
        fsync((info.fd_dac[0]));
        memset(&dac_chaninfo, 0, sizeof(struct rfsoc_channel_info));
      } else {
        mask_prev = 0;
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
      }
      /* disable local start and reset fifo's */
      ret = reset_pipe(dac_reset_gpio, dac_localstart_gpio);
      if (ret < 0) {
        MetalLoghandler_firmware(ret, "un-able to reset dac\n");
        goto err;
      }
      info.scratch_value_dac = 0;
      *status = SUCCESS;
      return;
    }
    /*
     * Check the mode, if it is DDR mode, start the
     * required DAC channels depending on the mask value.
     */
    if (info.mem_type == DDR) {
      for (i = 0; i < MAX_DAC; i++) {
        if ((channels & (1 << i)))
          dac = i;
        else
          continue;
        tile_id = (dac / 4);
        /*
* SSR IP design supports only one DAC(DAC7) and one ADC(ADC0)
* So irrespective of channel force the channel to 1, as supported
* DAC uses gpio's correponding to DAC0
*/

        if (info.design_type == DAC1_ADC1) {
          channels = 1;
          dac = 0;
        }

        /* Disable local start gpio */
        ret = set_gpio(dac_localstart_gpio[dac], 0);
      if (ret) {
        ret = GPIO_SET_FAIL;
          for (i = 0; i < max_dac; i++)
            info.channel_size[i] = 0;
          MetalLoghandler_firmware(ret,
                                   "Unable to re-set localstart GPIO value\n");
        goto err;
      }

        /* Assert and De-Assert reset FIFO GPIO */
        ret = set_gpio(dac_reset_gpio[dac], 1);
    if (ret) {
      ret = GPIO_SET_FAIL;
          for (i = 0; i < max_dac; i++)
            info.channel_size[i] = 0;
          MetalLoghandler_firmware(ret, "Unable to assert reset GPIO value\n");
      goto err;
    }

        usleep(10);

        ret = set_gpio(dac_reset_gpio[dac], 0);
    if (ret) {
      ret = GPIO_SET_FAIL;
          for (i = 0; i < max_dac; i++)
            info.channel_size[i] = 0;
          MetalLoghandler_firmware(ret,
                                   "Unable to de-assert reset GPIO value\n");
      goto err;
    }
        /* Enable RFDC FIFO */
        ret = change_fifo_stat(DAC, tile_id, FIFO_EN);
        if (ret != SUCCESS) {
          ret = EN_FIFO_FAIL;
          for (i = 0; i < max_dac; i++)
            info.channel_size[i] = 0;
          MetalLoghandler_firmware(ret, "Unable to enable FIFO\n");
      goto err;
    }

        /* enable local start GPIO */
        ret = set_gpio(dac_localstart_gpio[dac], 1);
        if (ret) {
          ret = GPIO_SET_FAIL;
          for (i = 0; i < max_dac; i++)
            info.channel_size[i] = 0;
          MetalLoghandler_firmware(ret,
                                   "Unable to set localstart GPIO value\n");
			goto err;
		}
      }

      if (info.invalid_size < 0) {
        ret = FAIL;
        MetalLoghandler_firmware(
            ret, "in tdm size should be same for all channels\n");
        info.invalid_size = 0;
        for (i = 0; i < max_dac; i++)
          info.channel_size[i] = 0;
        goto err;
      }

      info.scratch_value_dac = channels;
      // update scratch pad register
      *(volatile u32 *)info.map_scratchreg_dac = info.scratch_value_dac;

      ret = set_gpio(dac_select_gpio[0], 0);
      if (ret) {
        ret = GPIO_SET_FAIL;
        MetalLoghandler_firmware(ret, "unable to set select gpio value\n");
        for (i = 0; i < max_dac; i++)
          info.channel_size[i] = 0;
        goto err;
      }
      usleep(10);

      //	toggle gpio64
      ret = set_gpio(dac_select_gpio[0], 1);
      if (ret) {
        ret = GPIO_SET_FAIL;
        MetalLoghandler_firmware(ret, "unable to set select gpio value\n");
        for (i = 0; i < max_dac; i++)
          info.channel_size[i] = 0;
        goto err;
      }

      usleep(10);

      dac_chaninfo.num_channels = 0;
      for (i = 0; i < 8; i++) {
        if (channels & (1 << i)) {
          dac_chaninfo.channel[dac_chaninfo.num_channels] = i;
          dac_chaninfo.channel_size[dac_chaninfo.num_channels] =
              info.channel_size[0];
          dac_chaninfo.num_channels++;
        }
      }

      if (dac_chaninfo.num_channels == 0) {
        ret = FAIL;
        MetalLoghandler_firmware(ret, "no channel is selected\n");
        for (i = 0; i < max_dac; i++)
          info.channel_size[i] = 0;
        goto err;
      }

      /* trigger dma */
      ret = write((info.fd_dac[0]), (void *)&dac_chaninfo,
                  ((dac_chaninfo.channel_size[0]) * sizeof(signed char)));
      if (ret < 0) {
        ret = DMA_TX_TRIGGER_FAIL;
        MetalLoghandler_firmware(ret, "error in writing data\n");
        for (i = 0; i < max_dac; i++)
          info.channel_size[i] = 0;
        goto err;
      }

        *status = SUCCESS;
        return;
    } else {
      /*
       * Check the mode, if it is BRAM mode, start the
       * required DAC channels depending on the mask value.
       * Take care of MTS and NON-MTS sequence depending on
       * mts_enable_dac flag.
       */
      for (i = 0; i < MAX_DAC; i++) {
        if ((channels & (1 << i))) {
          /*
           * If a channel is already running no need to
           * re-start it again. Start the channel which is
           * not in running state.
           */
          if ((mask_prev & (1 << i))) {
            continue;
          } else {
            dac = i;
      }
        } else {
          /*
           * Ensure DAC's which are not required
           * are disabled. No need to do this for SSR IP design.
           */
          if (info.design_type == DAC1_ADC1)
            continue;
          ret = set_gpio(dac_loopback_gpio[i], 0);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "Unable to re-set loopback GPIO value\n");
            mask_prev = 0;
            goto err;
          }
          ret = set_gpio(dac_localstart_gpio[i], 0);
          if (ret) {
            ret = GPIO_SET_FAIL;
            for (i = 0; i < max_dac; i++)
              info.channel_size[i] = 0;
            MetalLoghandler_firmware(
                ret, "Unable to re-set localstart GPIO value\n");
            mask_prev = 0;
            goto err;
        }
          /* assert reset fifo gpio for all dac/adc's*/
          ret = set_gpio(dac_reset_gpio[i], 1);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "unable to assert reset gpio value\n");
            mask_prev = 0;
            goto err;
          }
          usleep(10);
          /* De-Assert reset FIFO GPIO for all DAC/ADC's*/
          ret = set_gpio(dac_reset_gpio[i], 0);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "Unable to de-assert reset GPIO value\n");
            mask_prev = 0;
            goto err;
          }
          continue;
        }

        tile_id = (dac / MAX_DAC_PER_TILE);
        if (info.design_type == DAC1_ADC1)
          dac = 0;
        info.scratch_value_dac = (1 << dac);

        /* write to channel select GPIO */
        ret = channel_select_gpio(dac_select_gpio, dac, DAC);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret,
                                   "Unable to set channelselect GPIO value\n");
          mask_prev = 0;
          goto err;
        }

        /*
         * If it is MTS mode, no need to reset GPIO's and enable FIFIO's
         * as it is already taken care of in MTS routines.
         */
        if (!info.mts_enable_dac) {
          /* Assert and De-Assert reset FIFO GPIO */
          ret = set_gpio(dac_reset_gpio[dac], 1);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "Unable to assert reset GPIO value\n");
            mask_prev = 0;
            goto err;
          }

          usleep(10);

          ret = set_gpio(dac_reset_gpio[dac], 0);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "Unable to de-assert reset GPIO value\n");
            mask_prev = 0;
            goto err;
          }
          /* Enable RFDC FIFO */
          ret = change_fifo_stat(DAC, tile_id, FIFO_EN);
          if (ret != SUCCESS) {
            ret = EN_FIFO_FAIL;
            MetalLoghandler_firmware(ret, "Unable to enable FIFO\n");
            mask_prev = 0;
            goto err;
          }
        }
        /* Trigger DMA */
        ret = write((info.fd_dac[dac]), info.map_dac[dac],
                    ((info.channel_size[dac]) * sizeof(signed char)));
        if (ret < 0) {
          ret = DMA_TX_TRIGGER_FAIL;
          MetalLoghandler_firmware(ret, "Error in writing data\n");
          mask_prev = 0;
          goto err;
        }
        ret = set_gpio(dac_loopback_gpio[dac], 1);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret,
                                   "Unable to re-set localstart GPIO value\n");
          mask_prev = 0;
          goto err;
        }

        /*
         * If it is MTS, no need to enable local start as MTS
         * depending on the global start GPIO.
         */
        if (!info.mts_enable_dac) {
          /* enable local start GPIO */
          ret = set_gpio(dac_localstart_gpio[dac], 1);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "Unable to set localstart GPIO value\n");
            mask_prev = 0;
            goto err;
          }
        }
        /* Reset DMA */
        fsync((info.fd_dac[dac]));
      }
      mask_prev = channels;
      /* If it is MTS, enable global start GPIO */
      if (info.mts_enable_dac) {
        ret = set_gpio(dac_localstart_gpio[0], 1);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret,
                                   "unable to set dac global start gpio\n");
          goto err;
      }
    }
    }
  } else if (type == ADC) {
    /*
     * On recieving stop command, clean-up required stuff for DDR mode.
     * No need of any clean-up in BRAM mode.
     */
    if ((channels == 0) && (info.mem_type == DDR)) {
      info.scratch_value_adc = 0;
      info.first_read = 0;
      /* Reset DMA */
      fsync((info.fd_adc[0]));
    *status = SUCCESS;
    return;
    }
    /* DDR mode */
    if (info.mem_type == DDR) {
      info.scratch_value_adc = channels;
      info.first_read = 1;
  } else {
      /* BRAM mode */

      /*
       * In MTS mode, disable global start, reset FIFO's and then enable global
       * start
       * so as to support multiple ADC acquire in MTS mode.
       */
      if (info.mts_enable_adc) {
        /* disable global start */
        ret = set_gpio(adc_localstart_gpio[0], 0);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret,
                                   "unable to assert adc global start gpio\n");
    goto err;
  }

        /* assert reset fifo gpio for all adc's */
        for (i = 0; i < max_dac; i++) {
          ret = set_gpio(adc_reset_gpio[i], 1);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "unable to assert reset gpio value\n");
            goto err;
          }
        }

        usleep(10);

        /* De-Assert reset FIFO GPIO for all adc's */
        for (i = 0; i < max_dac; i++) {
          ret = set_gpio(adc_reset_gpio[i], 0);
          if (ret) {
            ret = GPIO_SET_FAIL;
            MetalLoghandler_firmware(ret,
                                     "Unable to de-assert reset GPIO value\n");
            goto err;
          }
        }

        /* enable global start */
        ret = set_gpio(adc_localstart_gpio[0], 1);
        if (ret) {
          ret = GPIO_SET_FAIL;
          MetalLoghandler_firmware(ret,
                                   "unable to assert adc global start gpio\n");
          goto err;
        }
      }
    }
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
