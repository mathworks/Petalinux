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
/***************************** Include Files ********************************/
#include "cmd_interface.h"
#include "tcp_interface.h"
#include "rfdc_interface.h"
#include "local_mem.h"
#include "data_interface.h"
#include "xrfdc_mts.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/
/************************** Function Prototypes *****************************/

/************************** Variable Definitions ****************************/

XRFdc RFdcInst; /* RFdc driver instance */
struct Channel_Map DacMap[16];
struct Channel_Map AdcMap[16];
u16 AdcMemMap[16];
u16 DacMemMap[16];
u32 AdcInitDataType[16];
struct MMCMReg_struct MMCMReg_config[8];
extern u32 MMCMFin[8];

u32 lmem_rd32(void *vaddr)
{
	return (*(u32 *)(vaddr));
}

void lmem_wr32(void *vaddr, u32 val)
{
	*(u32 *)vaddr = val;
}

int rfdc_inst_init(u16 rfdc_id)
{
	XRFdc_Config *ConfigPtr;
	XRFdc *RFdcInstPtr = &RFdcInst;
	int Status;
	struct metal_device *device;
	struct metal_init_params init_param = {
		.log_handler = MetalLoghandler,
		.log_level = METAL_LOG_WARNING,
	};
	if (metal_init(&init_param)) {
		if (0 >
		    sendString("ERROR: metal_init METAL_LOG_INIT_FAILURE", 40))
			printf("%s: Error in sendString\n", __func__);
		return XRFDC_FAILURE;
	}

	/* Initialize the RFdc driver. */
	ConfigPtr = XRFdc_LookupConfig(rfdc_id);
	Status = XRFdc_RegisterMetal(RFdcInstPtr, rfdc_id, &device);
	if (Status != XRFDC_SUCCESS) {
		if (0 >
		    sendString(
			    "ERROR: XRFdc_CfgInitialize RFDC_CFG_INIT_FAILURE",
			    48))
			printf("%s: Error in sendString\n", __func__);
		return XRFDC_FAILURE;
	}
	/* Initializes the controller */
	Status = XRFdc_CfgInitialize(RFdcInstPtr, ConfigPtr);
	if (Status != XRFDC_SUCCESS) {
		if (0 >
		    sendString(
			    "ERROR: XRFdc_CfgInitialize RFDC_CFG_INIT_FAILURE",
			    48))
			printf("%s: Error in sendString\n", __func__);
		return XRFDC_FAILURE;
	}

	return XRFDC_SUCCESS;
}

int rfdc_init(void)
{
	int ret;

	/* initialize RFDC instance */
	ret = rfdc_inst_init(RFDC_DEVICE_ID);
	if (ret != XRFDC_SUCCESS) {
		printf("Failed to initialize RFDC instance\n");
	}
	return SUCCESS;
}

/* Initialize Memory Mapping. */
int RFInitBuildMemoryMap(void)
{
	/* build the channel address map for localmemaddr */
	int Tile_Id = 0;
	u32 Block_Id = 0;
	u32 mem_size = 0;
	u32 num_mem = 0;
	u32 mem_info;
	u16 channel;
	u16 numblockpertile = 0;
	int i;
	u32 CurAddr;

	printf("%s RFdcInst.ADC4GSPS = %d\n", __func__, RFdcInst.ADC4GSPS);

	info.fd = open("/dev/mem", O_RDWR | O_NDELAY);
	if (info.fd < 0) {
		printf("%s: /dev/mem open failed\n", __func__);
		goto err;
	}
	info.base_adc =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((ADC_SINK_I_BASEADDR) & ~MAP_MASK));
	if (info.base_adc == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the DAC file\n", __func__);
		close(info.fd);
		goto err;
	}
	info.vaddr_adc = info.base_adc + (ADC_SINK_I_BASEADDR & MAP_MASK);

	info.base_dac =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((DAC_SOURCE_I_BASEADDR) & ~MAP_MASK));
	if (info.base_dac == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the ADC file\n", __func__);
		munmap(info.base_adc, MAP_SIZE);
		close(info.fd);
		goto err;
	}
	info.vaddr_dac = info.base_dac + (DAC_SOURCE_I_BASEADDR & MAP_MASK);

	info.paddr_dac = (DAC_SOURCE_I_BASEADDR);
	info.paddr_adc = (ADC_SINK_I_BASEADDR);

	mem_info = lmem_rd32(info.vaddr_adc + LMEM_INFO);
	mem_size = mem_info & 0x00FFFFFF; /* (23:0) */
	num_mem = (mem_info >> 24) & 0x0000007f; /* {30:24} */
	mem_size = 2 * mem_size / (num_mem + 1) / 16;
	CurAddr = info.paddr_adc + mem_size;

	printf("DAC BASE ADDRESS = %x\n", info.paddr_dac);
	printf("ADC BASE ADDRESS = %x\n", info.paddr_adc);
	i = 0;
	channel = 0;
	for (Tile_Id = 0; Tile_Id < 4; Tile_Id++) {
		numblockpertile =
			RFdcInst.RFdc_Config.ADCTile_Config[Tile_Id].NumSlices;
		for (Block_Id = 0; Block_Id < numblockpertile; Block_Id++) {
			i++;
			if (XRFdc_IsADCDigitalPathEnabled(&RFdcInst, Tile_Id,
							  Block_Id)) {
				printf("ADC: Tile %d Block %d, addr = %x"
				       " channel=%d\n",
				       Tile_Id, Block_Id, CurAddr, channel);
				AdcMap[Tile_Id * numblockpertile + Block_Id]
					.addr_I = CurAddr;
				AdcMap[Tile_Id * numblockpertile + Block_Id]
					.Channel_I = channel;
				AdcMemMap[channel] =
					Tile_Id * numblockpertile + Block_Id;
				AdcInitDataType[Tile_Id * numblockpertile +
						Block_Id] = 0;
				CurAddr += mem_size;
				channel++;
				if (XRFdc_IsHighSpeedADC(&RFdcInst, Tile_Id) &&
				    XRFdc_GetDataType(
					    &RFdcInst, ADC, Tile_Id,
					    Block_Id << RFdcInst.ADC4GSPS)) {
					printf("ADC: Tile %d Block %d, addrQ= %x channel = %d \n",
					       Tile_Id, Block_Id, CurAddr,
					       channel);
					AdcMap[Tile_Id * numblockpertile +
					       Block_Id]
						.addr_Q = CurAddr;
					AdcMap[Tile_Id * numblockpertile +
					       Block_Id]
						.Channel_Q = channel;
					AdcMemMap[channel] =
						Tile_Id * numblockpertile +
						Block_Id;
					AdcInitDataType[Tile_Id *
								numblockpertile +
							Block_Id] = 1;
					CurAddr += mem_size;
					channel++;

				} else {
					printf("ADC: Tile %d Block %d, addrQ1"
					       " = %x\n",
					       Tile_Id, Block_Id, CurAddr);
					AdcMap[Tile_Id * numblockpertile +
					       Block_Id]
						.addr_Q =
						AdcMap[Tile_Id * numblockpertile +
						       Block_Id]
							.addr_I;
					AdcMap[Tile_Id * numblockpertile +
					       Block_Id]
						.Channel_Q =
						AdcMap[Tile_Id * numblockpertile +
						       Block_Id]
							.Channel_I;
					AdcInitDataType[Tile_Id *
								numblockpertile +
							Block_Id] = 0;
				}
			} else {
				AdcMap[Tile_Id * numblockpertile + Block_Id]
					.addr_I = 0xffffffff;
				AdcMap[Tile_Id * numblockpertile + Block_Id]
					.addr_Q = 0xffffffff;
				AdcMap[Tile_Id * numblockpertile + Block_Id]
					.Channel_I = 0xffffffff;
				AdcMap[Tile_Id * numblockpertile + Block_Id]
					.Channel_Q = 0xffffffff;
				AdcInitDataType[Tile_Id * numblockpertile +
						Block_Id] = 0;
			}
		}
	}
	for (; i < 16; i++) {
		AdcMap[i].addr_I = 0xffffffff;
		AdcMap[i].addr_Q = 0xffffffff;
		AdcMap[i].Channel_I = 0xffffffff;
		AdcMap[i].Channel_Q = 0xffffffff;
		AdcInitDataType[i] = 0;
	}
	channel = 0;
	i = 0;
	mem_info = lmem_rd32(info.vaddr_dac + LMEM_INFO);
	mem_size = mem_info & 0x00FFFFFF; /* (23:0) */
	num_mem = (mem_info >> 24) & 0x0000007f; /* {30:24} */
	mem_size = 2 * mem_size / (num_mem + 1) / 16;
	CurAddr = info.paddr_dac + mem_size;
	for (Tile_Id = 0;
	     Tile_Id < (4 >> (RFdcInst.ADC4GSPS &&
			      (RFdcInst.RFdc_Config.IPType < XRFDC_GEN3)));
	     Tile_Id++) {
		for (Block_Id = 0; Block_Id < 4; Block_Id++) {
			i++;
			if (XRFdc_IsDACDigitalPathEnabled(&RFdcInst, Tile_Id,
							  Block_Id)) {
				printf("DAC: Tile %d Block %d, addrQ1 = %x channel = %d\n",
				       Tile_Id, Block_Id, CurAddr, channel);
				DacMap[Tile_Id * 4 + Block_Id].addr_I = CurAddr;
				DacMap[Tile_Id * 4 + Block_Id].addr_Q = CurAddr;
				DacMap[Tile_Id * 4 + Block_Id].Channel_I =
					channel;
				DacMap[Tile_Id * 4 + Block_Id].Channel_Q =
					channel;
				DacMemMap[channel] = Tile_Id * 4 + Block_Id;
				CurAddr += mem_size;
				channel++;
			} else {
				DacMap[Tile_Id * 4 + Block_Id].addr_I =
					0xffffffff;
				DacMap[Tile_Id * 4 + Block_Id].addr_Q =
					0xffffffff;
				DacMap[Tile_Id * 4 + Block_Id].Channel_I =
					0xffffffff;
				DacMap[Tile_Id * 4 + Block_Id].Channel_Q =
					0xffffffff;
			}
		}
	}
	for (; i < 16; i++) {
		DacMap[i].addr_I = 0xffffffff;
		DacMap[i].addr_Q = 0xffffffff;
		DacMap[i].Channel_I = 0xffffffff;
		DacMap[i].Channel_Q = 0xffffffff;
	}
	info.clk_wiz_adc0 =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((CLK_WIZ_ADC0_BASEADDR) & ~MAP_MASK));
	if (info.clk_wiz_adc0 == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the ADC Clk Wiz0\n", __func__);
		goto err_clk_adc0;
	}
	info.clk_wiz_adc1 =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((CLK_WIZ_ADC1_BASEADDR) & ~MAP_MASK));
	if (info.clk_wiz_adc1 == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the ADC Clk Wiz1\n", __func__);
		goto err_clk_adc1;
	}
	info.clk_wiz_adc2 =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((CLK_WIZ_ADC2_BASEADDR) & ~MAP_MASK));
	if (info.clk_wiz_adc2 == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the ADC Clk Wiz2\n", __func__);
		goto err_clk_adc2;
	}
	info.clk_wiz_adc3 =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((CLK_WIZ_ADC3_BASEADDR) & ~MAP_MASK));
	if (info.clk_wiz_adc3 == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the ADC Clk Wiz3\n", __func__);
		goto err_clk_adc3;
	}

	info.clk_wiz_dac0 =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((CLK_WIZ_DAC0_BASEADDR) & ~MAP_MASK));
	if (info.clk_wiz_dac0 == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the DAC Clk Wiz0\n", __func__);
		goto err_clk_dac0;
	}
	info.clk_wiz_dac1 =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((CLK_WIZ_DAC1_BASEADDR) & ~MAP_MASK));
	if (info.clk_wiz_dac1 == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the DAC Clk Wiz1\n", __func__);
		goto err_clk_dac1;
	}
	info.clk_wiz_dac2 =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((CLK_WIZ_DAC2_BASEADDR) & ~MAP_MASK));
	if (info.clk_wiz_dac2 == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the DAC Clk Wiz1\n", __func__);
		goto err_clk_dac2;
	}
	info.clk_wiz_dac3 =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((CLK_WIZ_DAC3_BASEADDR) & ~MAP_MASK));
	if (info.clk_wiz_dac3 == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the DAC Clk Wiz1\n", __func__);
		goto err_clk_dac3;
	}

	return SUCCESS;

err_clk_dac3:
	munmap(info.clk_wiz_dac2, MAP_SIZE);
err_clk_dac2:
	munmap(info.clk_wiz_dac1, MAP_SIZE);
err_clk_dac1:
	munmap(info.clk_wiz_dac0, MAP_SIZE);
err_clk_dac0:
	munmap(info.clk_wiz_adc3, MAP_SIZE);
err_clk_adc3:
	munmap(info.clk_wiz_adc2, MAP_SIZE);
err_clk_adc2:
	munmap(info.clk_wiz_adc1, MAP_SIZE);
err_clk_adc1:
	munmap(info.clk_wiz_adc0, MAP_SIZE);
err_clk_adc0:
	munmap(info.base_adc, MAP_SIZE);
	munmap(info.base_dac, MAP_SIZE);
	close(info.fd);
err:
	return FAIL;
}

int InitMMCM_ADC()
{
	int Tile_Id = 0;
	int ClkWiz_Id = 0;
	u32 ClkConfigReg;
	void *ClkBaseAddr;
	u32 mult;
	u32 div;
	u32 clkoutdiv;
	u32 clkoutfrac;
	u32 fracmult;
	u32 MmcmFout;
	u32 FabricDataRate;
	u32 Block_Id = 0;
	int status = 0;
	u32 MTSenable = 0;
	XRFdc_PLL_Settings PLLSettings;
	u32 SampleRate = 0;
	XRFdc_Mixer_Settings Mixer_Settings;
	u32 DataIQ = 1; /* 1 is real, 2 is IQ */
	for (Tile_Id = 0; Tile_Id < 4; Tile_Id++) {
		if (XRFdc_CheckTileEnabled(&RFdcInst, XRFDC_ADC_TILE,
					   Tile_Id) == SUCCESS) {
			switch (Tile_Id & !MTSenable) {
			case 0:
				ClkBaseAddr = info.clk_wiz_adc0;
				break;
			case 1:
				ClkBaseAddr = info.clk_wiz_adc1;
				break;
			case 2:
				ClkBaseAddr = info.clk_wiz_adc2;
				break;
			case 3:
				ClkBaseAddr = info.clk_wiz_adc3;
				break;
			}
			printf("ADC Tile_Id = %d\n", Tile_Id);
			/* Calculate Fratio = mult/div */
			ClkConfigReg = lmem_rd32(ClkBaseAddr + 0x200);
			mult = (ClkConfigReg & 0x0000ff00) >> 8;
			div = (ClkConfigReg & 0x000000ff);
			fracmult = ((ClkConfigReg & 0x03ff0000) >> 16);
			mult = (1000 * mult) + fracmult;
			ClkConfigReg = lmem_rd32(ClkBaseAddr + 0x208);
			clkoutdiv = ((ClkConfigReg & 0x000000ff));
			clkoutfrac = ((ClkConfigReg & 0x0003ff00) >> 8);
			clkoutdiv = (1000 * clkoutdiv) + clkoutfrac;
			/* calculate MMCM MmcmFout, DC Fin */
			status = XRFdc_GetPLLConfig(&RFdcInst, XRFDC_ADC_TILE,
						    Tile_Id, &PLLSettings);
			SampleRate = 1000000 * PLLSettings.SampleRate;
			Block_Id = 0;
			status = XRFdc_GetPLLConfig(&RFdcInst, XRFDC_ADC_TILE,
						    Tile_Id, &PLLSettings);
			SampleRate = 1000000 * PLLSettings.SampleRate;
			Block_Id = 0;
			while ((XRFdc_CheckDigitalPathEnabled(
					&RFdcInst, XRFDC_ADC_TILE, Tile_Id,
					Block_Id) != SUCCESS) &&
			       (Block_Id < 4)) {
				Block_Id++;
			}
			XRFdc_GetFabRdVldWords(&RFdcInst, XRFDC_ADC_TILE,
					       Tile_Id, Block_Id,
					       &FabricDataRate);
			status |= XRFdc_GetMixerSettings(&RFdcInst,
							 XRFDC_ADC_TILE,
							 Tile_Id, Block_Id,
							 &Mixer_Settings);
			/* TODO CR-1049584 below, remove check on coarse+bypass */
			if ((Mixer_Settings.MixerMode ==
			     XRFDC_MIXER_MODE_R2R) ||
			    XRFdc_IsHighSpeedADC(&RFdcInst, Tile_Id) ||
			    (Mixer_Settings.MixerType ==
				     XRFDC_MIXER_TYPE_COARSE &&
			     Mixer_Settings.CoarseMixFreq ==
				     XRFDC_COARSE_MIX_BYPASS)) {
				DataIQ = 1;
			} else {
				DataIQ = 2;
			}
			MmcmFout =
				SampleRate * DataIQ /
				((RFdcInst.RFdc_Config.ADCTile_Config[Tile_Id]
					  .ADCBlock_Digital_Config[Block_Id]
					  .DecimationMode) *
				 FabricDataRate);
			MMCMFin[ClkWiz_Id] = MmcmFout * div * clkoutdiv;
			MMCMFin[ClkWiz_Id] = MMCMFin[ClkWiz_Id] / mult;

		} else {
			MMCMFin[ClkWiz_Id] = 0;
		}
		ClkWiz_Id++;
	}
	return status;
}

int InitMMCM_DAC()
{
	int Tile_Id = 0;
	int ClkWiz_Id = 4; /* 4 for 4 ADC tiles */
	u32 ClkConfigReg;
	void *ClkBaseAddr;
	u32 mult;
	u32 div;
	u32 clkoutdiv;
	u32 clkoutfrac;
	u32 fracmult;
	u32 MmcmFout;
	u32 FabricDataRate;
	u32 Block_Id = 0;
	int status = 0;
	u32 MTSenable = 0;
	XRFdc_PLL_Settings PLLSettings;
	u32 SampleRate = 0;
	u32 DataPathMode = 1;
	XRFdc_Mixer_Settings Mixer_Settings;
	u32 DataIQ = 1; /* 1 is real, 2 is IQ */
	u32 InterDecim = 1;

	for (Tile_Id = 0;
	     Tile_Id < (4 >> (RFdcInst.ADC4GSPS &&
			      (RFdcInst.RFdc_Config.IPType < XRFDC_GEN3)));
	     Tile_Id++) {
		if (XRFdc_CheckTileEnabled(&RFdcInst, XRFDC_DAC_TILE,
					   Tile_Id) == SUCCESS) {
			switch (Tile_Id & !MTSenable) {
			case 0:
				ClkBaseAddr = info.clk_wiz_dac0;
				break;
			case 1:
				ClkBaseAddr = info.clk_wiz_dac1;
				break;
			case 2:
				ClkBaseAddr = info.clk_wiz_dac2;
				break;
			case 3:
				ClkBaseAddr = info.clk_wiz_dac3;
				break;
			}
			printf("DAC Tile_Id = %d, \n", Tile_Id);
			/* Calculate Fratio = mult/div */
			ClkConfigReg = lmem_rd32(ClkBaseAddr + 0x200);
			mult = (ClkConfigReg & 0x0000ff00) >> 8;
			div = (ClkConfigReg & 0x000000ff);
			fracmult = ((ClkConfigReg & 0x03ff0000) >> 16);
			mult = (1000 * mult) + fracmult;
			ClkConfigReg = lmem_rd32(ClkBaseAddr + 0x208);
			clkoutdiv = (ClkConfigReg & 0x000000ff);
			clkoutfrac = ((ClkConfigReg & 0x0003ff00) >> 8);
			clkoutdiv = (1000 * clkoutdiv) + clkoutfrac;

			/* calculate MMCM Fout, DC Fin */
			status = XRFdc_GetPLLConfig(&RFdcInst, XRFDC_DAC_TILE,
						    Tile_Id, &PLLSettings);
			SampleRate = 1000000 * PLLSettings.SampleRate;
			while (XRFdc_CheckDigitalPathEnabled(
				       &RFdcInst, XRFDC_DAC_TILE, Tile_Id,
				       Block_Id) != SUCCESS &&
			       Block_Id < 4) {
				Block_Id++;
			}
			XRFdc_GetFabWrVldWords(&RFdcInst, XRFDC_DAC_TILE,
					       Tile_Id, Block_Id,
					       &FabricDataRate);
			if (RFdcInst.RFdc_Config.IPType >= XRFDC_GEN3) {
				status |=
					XRFdc_GetDataPathMode(&RFdcInst,
							      Tile_Id, Block_Id,
							      &DataPathMode);
			}
			if (DataPathMode != 4) {
				status |= XRFdc_GetMixerSettings(
					&RFdcInst, XRFDC_DAC_TILE, Tile_Id,
					Block_Id, &Mixer_Settings);
				XRFdc_GetInterpolationFactor(&RFdcInst, Tile_Id,
							     Block_Id,
							     &InterDecim);
			}
			if (DataPathMode == 2 || DataPathMode == 3) {
				InterDecim = 2 * InterDecim;
			}
			/* TODO CR-1049584 below, remove check on coarse+bypass */
			if (DataPathMode == 4 ||
			    (Mixer_Settings.MixerMode ==
			     XRFDC_MIXER_MODE_R2R) ||
			    Mixer_Settings.MixerMode == XRFDC_MIXER_MODE_R2C ||
			    (Mixer_Settings.MixerType ==
				     XRFDC_MIXER_TYPE_COARSE &&
			     Mixer_Settings.CoarseMixFreq ==
				     XRFDC_COARSE_MIX_BYPASS)) {
				DataIQ = 1;
			} else {
				DataIQ = 2;
			}

			MmcmFout = SampleRate * DataIQ /
				   (InterDecim * FabricDataRate);
			/* Calculate Fin */
			MMCMFin[ClkWiz_Id] =
				MmcmFout * div * clkoutdiv / (mult);
		} else {
			MMCMFin[ClkWiz_Id] = 0;
		}
		ClkWiz_Id++;
	}
	return status;
}
