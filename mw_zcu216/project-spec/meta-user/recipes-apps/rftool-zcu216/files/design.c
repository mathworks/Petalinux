/******************************************************************************
 *
 * Copyright (C) 2017-2023 Xilinx, Inc.  All rights reserved.
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
#include "design.h"
#include "cmd_interface.h"
#include "common.h"
#include "data_interface.h"
#include "rfdc_interface.h"
#include "tcp_interface.h"
#include <stdio.h>
#include <string.h>

#define XPAR_EXDES_TDDRTSCTRL_0_BASEADDR 0xB0200000

extern XRFdc RFdcInst;
extern int enTermMode;
u32 MMCMFin[8];

void MMCM_Rst(convData_t *cmdVals, char *txstrPtr, int *status)
{
	(void)txstrPtr;
	u32 Type = cmdVals[0].u;
	int Tile_Id = cmdVals[1].i;
	u32 MMCM_Lock = 0;
	MMCM_Lock = MMCM_Rst_HW(Type, Tile_Id);
	if (MMCM_Lock == 0) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM Tile %d is not locked after reset\r\n",
			  Tile_Id);
		*status = FAIL;
	} else if (MMCM_Lock == ERROR_EXECUTE) {
		*status = ERROR_EXECUTE;
	} else {
		*status = SUCCESS;
	}
	return;
}

u32 MMCM_Rst_HW(u32 Type, int Tile_Id)
{
	u32 MMCM_Lock = 0;
	void *BaseAddr;

	if (Type == ADC) {
		if (info.mts_enable_adc_mask & (0x1 << Tile_Id)) {
			Tile_Id = 0;
		}
		switch (Tile_Id) {
		case 0:
			BaseAddr = info.clk_wiz_adc0;
			break;
		case 1:
			BaseAddr = info.clk_wiz_adc1;
			break;
		case 2:
			BaseAddr = info.clk_wiz_adc2;
			break;
		case 3:
			BaseAddr = info.clk_wiz_adc3;
			break;
		default:
			metal_log(METAL_LOG_WARNING,
				  "\n MMCM associated with ADC Tile_Id %d does"
				  " not exist\r\n",
				  Tile_Id);
			return ERROR_EXECUTE;
		}
	} else {
		switch (Tile_Id) {
		case 0:
			BaseAddr = info.clk_wiz_dac0;
			break;
		case 1:
			BaseAddr = info.clk_wiz_dac1;
			break;
		case 2:
			BaseAddr = info.clk_wiz_dac2;
			break;
		case 3:
			BaseAddr = info.clk_wiz_dac3;
			break;
		default:
			metal_log(METAL_LOG_WARNING,
				  "\n MMCM associated with DAC Tile_Id %d does"
				  " not exist\r\n",
				  Tile_Id);
			return ERROR_EXECUTE;
		}
	}
	/* Read the reg of interest so that we can reprogram them after reset */
	u32 Clk_conf0 = lmem_rd32(BaseAddr + 0x200);
	u32 Clkoutdiv0 = lmem_rd32(BaseAddr + 0x208);
	/* Reset */
	lmem_wr32(BaseAddr + 0x00, 0x0A);
	usleep(100);

	/* Reprogram the reg of interests so that they are in sync */
	lmem_wr32(BaseAddr + 0x200, Clk_conf0);
	lmem_wr32(BaseAddr + 0x208, Clkoutdiv0);
	/* read the lock */
	MMCM_Lock = lmem_rd32(BaseAddr + 0x04) & 0x00000001;
	return MMCM_Lock;
}

void SetMMCMReg(convData_t *cmdVals, char *txstrPtr, int *status)
{
	(void)txstrPtr;
	u32 Type = cmdVals[0].u;
	int Tile_Id = cmdVals[1].i;
	u32 Mult = cmdVals[2].u;
	u32 Mult_frac = cmdVals[3].u;
	u32 Div = cmdVals[4].u;
	u32 clkout0_div = cmdVals[5].u;
	u32 clkout0_frac = cmdVals[6].u;
	u32 MMCM_Lock = 0;

	*status = SUCCESS;
	/* Check we are not trying to program the MMCM with invalid values */
	if (Mult_frac < 0 || Mult_frac > 875 || Mult_frac % 125 != 0) {
		metal_log(
			METAL_LOG_ERROR,
			"\n Invalid Fractional multiplier %d, must be multiple"
			" of 125 \r\n",
			Mult_frac);
		*status |= FAIL;
	}
	if (clkout0_frac < 0 || clkout0_frac > 875 || clkout0_frac % 125 != 0) {
		metal_log(METAL_LOG_ERROR,
			  "\n Invalid Fractional divider %d, must be multiple"
			  " of 125 \r\n",
			  clkout0_frac);
		*status |= FAIL;
	}
	if (Div == 0) {
		metal_log(METAL_LOG_ERROR,
			  "\n Invalid Divider D %d, cannot be 0 \r\n", Div);
		*status |= FAIL;
	}
	if (*status == SUCCESS) {
		MMCM_Lock = MMCM_Reprog_HW(Type, Tile_Id, Mult, Mult_frac, Div,
					   clkout0_div, clkout0_frac);
		if (MMCM_Lock == 0) {
			metal_log(METAL_LOG_ERROR,
				  "\n MMCM Tile %d is not locked after reprog"
				  " Mult %lu Div %lu\r\n",
				  Tile_Id, Mult, clkout0_div);
			*status = FAIL;
		} else if (MMCM_Lock == ERROR_EXECUTE) {
			*status = ERROR_EXECUTE;
		} else {
			*status = SUCCESS;
		}
	}
	return;
}

u32 MMCM_Reprog_HW(u32 Type, int Tile_Id, u32 Mult, u32 Mult_frac, u32 Div,
		   u32 clkout0_div, u32 clkout0_frac)
{
	void *BaseAddr;
	u32 MMCM_Lock = 0;
	/* XRFdc_GetMTSEnable(&RFdcInst, Type, Tile_Id, &MTSEnable); */
	if (Type == ADC) {
		if (info.mts_enable_adc_mask & (0x1 << Tile_Id)) {
			Tile_Id = 0;
		}
		printf("info.mts_enable_adc_mask = %d Tile_Id = %d\n",
		       info.mts_enable_adc_mask, Tile_Id);
		switch (Tile_Id) {
		case 0:
			BaseAddr = info.clk_wiz_adc0;
			break;
		case 1:
			BaseAddr = info.clk_wiz_adc1;
			break;
		case 2:
			BaseAddr = info.clk_wiz_adc2;
			break;
		case 3:
			BaseAddr = info.clk_wiz_adc3;
			break;
		default:
			metal_log(METAL_LOG_WARNING,
				  "\n MMCM associated with ADC Tile_Id %d does"
				  " not exist\r\n",
				  Tile_Id);
			return ERROR_EXECUTE;
		}
	} else {
		printf("info.mts_enable_dac_mask = %d Tile_Id = %d\n",
		       info.mts_enable_dac_mask, Tile_Id);
		switch (Tile_Id) {
		case 0:
			BaseAddr = info.clk_wiz_dac0;
			break;
		case 1:
			BaseAddr = info.clk_wiz_dac1;
			break;
		case 2:
			BaseAddr = info.clk_wiz_dac2;
			break;
		case 3:
			BaseAddr = info.clk_wiz_dac3;
			break;
		default:
			metal_log(METAL_LOG_WARNING,
				  "\n MMCM associated with DAC Tile_Id %d does"
				  " not exist\r\n",
				  Tile_Id);
			return ERROR_EXECUTE;
		}
	}

	usleep(100);
	lmem_wr32(BaseAddr + 0x200, (Mult_frac << 16) | (Mult << 8) | Div);
	usleep(100);
	/* MMCM CLKOUT DIV value */
	lmem_wr32(BaseAddr + 0x208, (clkout0_frac << 8) | clkout0_div);
	lmem_wr32(BaseAddr + 0x214,
		  clkout0_div << 1); /* clkoutdiv1 always half clkoutdiv0 */
	usleep(100);
	lmem_wr32(BaseAddr + 0x25C, 0x03);
	usleep(100);
	MMCM_Lock = lmem_rd32(BaseAddr + 0x04) & 0x00000001;
	usleep(100);
	return MMCM_Lock;
}

void GetMMCMReg(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Type = cmdVals[0].u;
	int Tile_Id = cmdVals[1].u;
	u32 Mult = 0;
	u32 MultFrac = 0;
	u32 Div = 0;
	u32 Clk0Div = 0;
	u32 Clk1Div = 0;
	u32 Clk0DivFrac = 0;
	u32 Lock = 0;

	*status = GetMMCMRegHW(Type, Tile_Id, &Lock, &Mult, &MultFrac, &Div,
			       &Clk0Div, &Clk0DivFrac, &Clk1Div);
	if (SUCCESS == *status) {
		sprintf(Response, " %u %u %u %u %u %u %u", Lock, Mult, MultFrac,
			Div, Clk0Div, Clk0DivFrac, Clk1Div);
		strncat(txstrPtr, Response, BUF_MAX_LEN);
	}
}

u32 GetMMCMRegHW(u32 Type, int Tile_Id, u32 *Lock, u32 *Mult, u32 *MultFrac,
		 u32 *Div, u32 *Clk0Div, u32 *Clk0DivFrac, u32 *Clk1Div)
{
	*Mult = 0;
	*MultFrac = 0;
	*Div = 0;
	*Clk0Div = 0;
	*Clk0DivFrac = 0;
	*Clk1Div = 0;
	u32 Clk_conf0;
	void *BaseAddr;
	u32 Enable;
	u32 status = FAIL;
	/* XRFdc_GetMTSEnable(&RFdcInst, Type, Tile_Id, &MTSEnable); */

	if (Type == ADC) {
		if (info.mts_enable_adc_mask & (0x1 << Tile_Id)) {
			Tile_Id = 0;
		}
		Enable = RFdcInst.RFdc_Config.ADCTile_Config[Tile_Id].Enable;
		printf("Type: %d Tile_Id: %d Enable: %d\n", Type, Tile_Id,
		       Enable);
		switch (Tile_Id) {
		case 0:
			BaseAddr = info.clk_wiz_adc0;
			break;
		case 1:
			BaseAddr = info.clk_wiz_adc1;
			break;
		case 2:
			BaseAddr = info.clk_wiz_adc2;
			break;
		case 3:
			BaseAddr = info.clk_wiz_adc3;
			break;
		default:
			metal_log(METAL_LOG_WARNING,
				  "\n MMCM associated with ADC Tile_Id %d does"
				  " not exist\r\n",
				  Tile_Id);
			return ERROR_EXECUTE;
		}
	} else {
		Enable = RFdcInst.RFdc_Config.DACTile_Config[Tile_Id].Enable;
		printf("Type: %d Tile_Id: %d Enable: %d\n", Type, Tile_Id,
		       Enable);
		switch (Tile_Id) {
		case 0:
			BaseAddr = info.clk_wiz_dac0;
			break;
		case 1:
			BaseAddr = info.clk_wiz_dac1;
			break;
		case 2:
			BaseAddr = info.clk_wiz_dac2;
			break;
		case 3:
			BaseAddr = info.clk_wiz_dac3;
			break;
		default:
			metal_log(METAL_LOG_WARNING,
				  "\n MMCM associated with DAC Tile_Id %d does"
				  " not exist\r\n",
				  Tile_Id);
			return ERROR_EXECUTE;
		}
	}
	if (Enable) {
		*Lock = lmem_rd32(BaseAddr + 0x04);
		Clk_conf0 = lmem_rd32(BaseAddr + 0x200);
		*MultFrac = (0x03ff0000 & Clk_conf0) >> 16;
		*Mult = (0x0000ff00 & Clk_conf0) >> 8;
		*Div = 0x000000ff & Clk_conf0;
		Clk_conf0 = lmem_rd32(BaseAddr + 0x208);
		*Clk0Div = 0x000000ff & Clk_conf0;
		*Clk0DivFrac = (0x0003ff00 & Clk_conf0) >> 8;
		Clk_conf0 = lmem_rd32(BaseAddr + 0x214);
		*Clk1Div = 0x000000ff & Clk_conf0;
		status = SUCCESS;
	} else {
		metal_log(METAL_LOG_ERROR, "\n Tile is not enabled %s \r\n",
			  __func__);
		status = FAIL;
	}
	return status;
}

void SetMMCM(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Type = cmdVals[0].u;
	u32 Tile_Id = cmdVals[1].i;
	XRFdc_PLL_Settings PLLSettings;
	XRFdc_Mixer_Settings Mixer_Settings;
	u32 DataIQ = 1; /* 1 is real, 2 is IQ */
	u16 FabClkDiv;
	u32 DataPathMode = 1;
	double SampleRate = 0;
	double FDCout = 0;
	double Fplin = 0;
	double FRatio = 0;
	u32 InterDecim = 1;
	u32 RdWidth = 0;
	u32 WrWidth = 0;
	u32 Wpl = 0;
	u32 Block_Id = 0;
	u32 Mult_Max;
	u32 Mult_Min;
	u32 Mult = 1;
	u32 Div_Min;
	u32 Div_Max;
	u32 Div;
	u8 found_ratio = 0;

	while (XRFdc_CheckDigitalPathEnabled(&RFdcInst, Type, Tile_Id,
					     Block_Id) == 1 &&
	       Block_Id < 4) {
		Block_Id++;
	}
	/* TODO: Look at reducing the equation by negating. */
	u32 ConstDivider = ((Type == XRFDC_ADC_TILE &&
			     RFdcInst.RFdc_Config.IPType < XRFDC_GEN3 &&
			     XRFdc_IsHighSpeedADC(&RFdcInst, Tile_Id)) ||
			    Type == XRFDC_DAC_TILE ||
			    (Type == XRFDC_ADC_TILE &&
			     (RFdcInst.RFdc_Config.IPType >= XRFDC_GEN3) &&
			     XRFdc_IsHighSpeedADC(&RFdcInst, Tile_Id))) ?
				   8 :
				   4;
	u32 Clk0DivFrac = 0;
	u32 clkout0_div = 1;
	u16 MMCM_Lock = 0;
	int Fpdmax = 450; /* This covers MMCM (-1=450, -2=500)
                       and PLL (667.5) cf. datasheet */
	int Fpdmin = 70;
	int FvcoMax = 1500; /* MMCM = 1600, PLL = 1500 */
	int FvcoMin = 800;
	int FoutMin = 6250; /* Minimum MMMCM output frequency in kHz */

	/* get the Tile and block settings */
	*status = XRFdc_GetPLLConfig(&RFdcInst, Type, Tile_Id, &PLLSettings);
	*status |= XRFdc_GetFabClkOutDiv(&RFdcInst, Type, Tile_Id, &FabClkDiv);

	*status |= XRFdc_GetFabRdVldWords(&RFdcInst, Type, Tile_Id, Block_Id,
					  &RdWidth);
	*status |= XRFdc_GetFabWrVldWords(&RFdcInst, Type, Tile_Id, Block_Id,
					  &WrWidth);

	if (Type == XRFDC_DAC_TILE) {
		Wpl = WrWidth;
		if (RFdcInst.RFdc_Config.IPType >= XRFDC_GEN3) {
			*status |= XRFdc_GetDataPathMode(
				&RFdcInst, Tile_Id, Block_Id, &DataPathMode);
		}
		if (DataPathMode != 4) {
			*status |= XRFdc_GetMixerSettings(&RFdcInst, Type,
							  Tile_Id, Block_Id,
							  &Mixer_Settings);
			XRFdc_GetInterpolationFactor(&RFdcInst, Tile_Id,
						     Block_Id, &InterDecim);
		}
		if (DataPathMode == 2 || DataPathMode == 3) {
			InterDecim = 2 * InterDecim;
		}
		/* TODO CR-1049584 below, remove check on coarse+bypass */
		if ((DataPathMode == 4) ||
		    (Mixer_Settings.MixerMode == XRFDC_MIXER_MODE_R2R) ||
		    (Mixer_Settings.MixerMode == XRFDC_MIXER_MODE_R2C) ||
		    (Mixer_Settings.MixerType == XRFDC_MIXER_TYPE_COARSE &&
		     Mixer_Settings.CoarseMixFreq == XRFDC_COARSE_MIX_BYPASS)) {
			DataIQ = 1;
		} else {
			DataIQ = 2;
		}
	} else {
		Wpl = RdWidth;
		*status |= XRFdc_GetMixerSettings(&RFdcInst, Type, Tile_Id,
						  Block_Id, &Mixer_Settings);
		XRFdc_GetDecimationFactor(&RFdcInst, Tile_Id, Block_Id,
					  &InterDecim);
		/* TODO CR-1049584 below, remove check on coarse+bypass */
		if ((Mixer_Settings.MixerMode == XRFDC_MIXER_MODE_R2R) ||
		    XRFdc_IsHighSpeedADC(&RFdcInst, Tile_Id) ||
		    (Mixer_Settings.MixerType == XRFDC_MIXER_TYPE_COARSE &&
		     Mixer_Settings.CoarseMixFreq == XRFDC_COARSE_MIX_BYPASS)) {
			DataIQ = 1;
		} else {
			DataIQ = 2;
		}
	}
	/* set the relevant parameters */
	SampleRate = 1000 * PLLSettings.SampleRate;
	FDCout = SampleRate / (double)(ConstDivider * (1 << (FabClkDiv - 1)));
	Fplin = FDCout;
	MMCMFin[4 * Type + Tile_Id] = (u32)(1000 * FDCout);
	/* Must respect FdcIn_Expected * D*clkdiv=M*Fplin, that is
     Fratio_N = FdcIn_Expected and Fratio_D=Fplin
     or Fratio_N = Fabclkdic*Cst and Fratio_D=interdecim*Wpl */
	Div_Min = (Fplin > Fpdmax) ?
			  2 :
			  1; /* Div min can only be 1 or 2 in our case. */
	Div_Max = (u32)Fplin / Fpdmin;
	if (Div_Max == 0) {
		Fpdmin = 10;
		Div_Max = (u32)(Fplin / Fpdmin);
	}
	if (Div_Max == 0) {
		*status |= FAIL;
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM spec violation %s Tile %d: Fin=%f"
			  " is below %d\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  Fplin, Fpdmin);
	}

	int i = 0;
	u32 Fratio_N;
	u32 Fratio_D;

	Fratio_N = DataIQ * ConstDivider * (1 << (FabClkDiv - 1));
	Fratio_D = InterDecim * Wpl;
	FRatio = Fratio_N / (double)Fratio_D;
	/* Try to find a clkout0_div that matches the ratio */
	for (Div = Div_Min; Div <= Div_Max; Div++) {
		if ((u32)(FvcoMin * Div / (double)Fplin) ==
		    FvcoMin * Div / (double)Fplin) {
			Mult_Min = (u32)(FvcoMin * Div /
					 (double)Fplin); /* Max VCO freq */
		} else {
			Mult_Min = (u32)(1 + FvcoMin * Div / (double)Fplin);
		}
		Mult_Max = (u32)(FvcoMax * Div / Fplin);
		for (Mult = Mult_Max; Mult >= Mult_Min; Mult--) {
			for (i = 1; i <= 128; i++) {
				if (Div * Fratio_N * i == Mult * Fratio_D) {
					clkout0_div = i;
					found_ratio = 1;
					goto end;
				} /* that is when we find
             Fratio_N/Fratio_D == Mult/D/Clkout0_Div */
			}
		}
	}

end:
	if (Mult < 2 || Mult > 128) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM spec violation %s Tile %d: Mult=%d "
			  "is outside range 2 to 128\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  Mult);
		*status |= FAIL;
	}
	if (FRatio * Fplin < 6.25) {
		metal_log(
			METAL_LOG_ERROR,
			"\nMMCM spec violation %s Tile %d: Output frequency %f < 6.25MHz is too low, may be due to interpolation/decimation too high\r\n",
			(Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			FRatio * Fplin);
		*status |= FAIL;
	}

	if (found_ratio == 0) {
		metal_log(METAL_LOG_ERROR,
			  "\nCould not find MMCM/PLL ratio for %s Tile %d "
			  "Fin %f Fout %f\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  Fplin, FRatio * Fplin);
		*status |= FAIL;
	}
	/* Check the VCO frequency and MMCM input freq is ok
     before ruining the MMCM */
	if (Fplin < 10) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM %s Tile %d specification violation: Fin=%f"
			  " Fin<10, M=%d, Div=%d, clkout0div=%d\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  FDCout, Mult, Div, clkout0_div);
		*status |= FAIL;
	}
	if ((FvcoMin * Div > (Fplin * Mult)) ||
	    (Div * FvcoMax < (Fplin * Mult))) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM %s Tile %d specification violation: VCO=%f"
			  " 800<VCO<%d, M=%d, Div=%d, clkout0div=%d\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  FDCout * Mult / (double)Div, FvcoMax, Mult, Div,
			  clkout0_div);
		*status |= FAIL;
	}
	if ((Fpdmin > Fplin / (double)Div) || (Fpdmax < Fplin / (double)Div)) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM %s Tile %d specification violation: Fph is"
			  " outside range %d<%f<%d, M=%d, Div=%d, "
			  "clkout0div=%d\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  Fpdmin, Fplin / (double)Div, Fpdmax, Mult, Div,
			  clkout0_div);
		*status |= FAIL;
	}
	if ((1000 * Fplin * Mult / (double)(Div * clkout0_div) < FoutMin)) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM %s Tile %d specification violation:"
			  " Fout=%fkHz<%dkHz, M=%d, Div=%d, clkout0div=%d\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  1000 * Fplin * Mult / (double)(Div * clkout0_div),
			  FoutMin, Mult, Div, clkout0_div);
		*status |= FAIL;
	}
	if (*status == SUCCESS || *status == WARN_EXECUTE) {
		usleep(200);
		(void)MMCM_Reprog_HW(Type, Tile_Id, Mult, 0, Div, clkout0_div,
				     Clk0DivFrac);
		usleep(200);
		MMCM_Lock = MMCM_Rst_HW(Type, Tile_Id);
		if (!MMCM_Lock) {
			metal_log(METAL_LOG_ERROR,
				  "\nMMCM %s Tile %d is not locked after reset "
				  "with Fin %f, Mult %lu, Div %lu, "
				  "ClkoutDiv %lu, try shutdown/startup the tile"
				  " but it is likely you will need to "
				  "re-configure the device\r\n",
				  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC",
				  Tile_Id, Fplin, Mult, Div, clkout0_div);
			*status |= FAIL;
		}
	}
	if (*status == SUCCESS) {
		sprintf(Response, " %u %u %u %u %u ", MMCM_Lock, Mult, Div,
			clkout0_div, Clk0DivFrac);
		strncat(txstrPtr, Response, BUF_MAX_LEN);
	}
}

void GetmmcmFin(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Type = cmdVals[0].u;
	u32 Tile_Id = cmdVals[1].i;
	if (Type == ADC) {
		if (info.mts_enable_adc_mask & (0x1 << Tile_Id)) {
			Tile_Id = 0;
		}
	}
	*status = SUCCESS;
	sprintf(Response, " %u  ", MMCMFin[(4 * Type) + Tile_Id]);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
}

void SetmmcmFin(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Type = cmdVals[0].u;
	u32 Tile_Id = cmdVals[1].i;
	u32 Fplin = cmdVals[2].u;

	XRFdc_PLL_Settings PLLSettings;
	XRFdc_Mixer_Settings Mixer_Settings;
	u32 DataIQ = 1; /* 1 is real, 2 is IQ */
	u32 DataPathMode = 1;
	u32 FDCout = 0;
	u32 SampleRate = 0;
	double FRatio = 0;
	u32 InterDecim = 1;
	u32 RdWidth = 0;
	u32 WrWidth = 0;
	u32 Wpl = 0;
	u32 Block_Id = 0;
	u32 Mult_Max;
	u32 Mult_Min;
	u32 Mult = 1;
	u32 Div_Min;
	u32 Div_Max;
	u32 Div;
	u8 found_ratio = 0;

	u32 Interrupt[16];
	u32 i;
	/* TODO: Look at reducing the equation by negating. */
	u32 Clk0DivFrac = 0;
	u32 clkout0_div = 1;
	u16 MMCM_Lock = 0;
	u32 Fpdmax = 450000; /* This covers MMCM (-1=450, -2=500) and
                       PLL (667.5) cf. datasheet */
	u32 Fpdmin = 70000;
	u32 FvcoMax = 1500000; /* MMCM = 1600, PLL = 1500 */
	u32 FvcoMin = 800000;
	u32 FoutMin = 6250; /* Minimum MMMCM output frequency in kHz. */
	u32 FoutMax;

	while (XRFdc_CheckDigitalPathEnabled(&RFdcInst, Type, Tile_Id,
					     Block_Id) == 1 &&
	       Block_Id < 4) {
		Block_Id++;
	}
	/* get the Tile and block settings */
	*status = XRFdc_GetPLLConfig(&RFdcInst, Type, Tile_Id, &PLLSettings);

	*status |= XRFdc_GetFabRdVldWords(&RFdcInst, Type, Tile_Id, Block_Id,
					  &RdWidth);
	*status |= XRFdc_GetFabWrVldWords(&RFdcInst, Type, Tile_Id, Block_Id,
					  &WrWidth);

	if (Type == XRFDC_DAC_TILE) {
		Wpl = WrWidth;
		if (RFdcInst.RFdc_Config.IPType >= XRFDC_GEN3) {
			*status |= XRFdc_GetDataPathMode(
				&RFdcInst, Tile_Id, Block_Id, &DataPathMode);
		}
		FoutMax = RFdcInst.RFdc_Config.IPType < XRFDC_GEN3 ?
				  500000 :
				  ((DataPathMode == 4) ? 625000 : 614000);
		if (DataPathMode != 4) {
			*status |= XRFdc_GetMixerSettings(&RFdcInst, Type,
							  Tile_Id, Block_Id,
							  &Mixer_Settings);
			XRFdc_GetInterpolationFactor(&RFdcInst, Tile_Id,
						     Block_Id, &InterDecim);
		}
		if (DataPathMode == 2 || DataPathMode == 3) {
			InterDecim = 2 * InterDecim;
		}
		/* TODO CR-1049584 below, remove check on coarse+bypass */
		if ((DataPathMode == 4) ||
		    (Mixer_Settings.MixerMode == XRFDC_MIXER_MODE_R2R) ||
		    (Mixer_Settings.MixerMode == XRFDC_MIXER_MODE_R2C) ||
		    (Mixer_Settings.MixerType == XRFDC_MIXER_TYPE_COARSE &&
		     Mixer_Settings.CoarseMixFreq == XRFDC_COARSE_MIX_BYPASS)) {
			DataIQ = 1;
		} else {
			DataIQ = 2;
		}
	} else {
		FoutMax = RFdcInst.RFdc_Config.IPType < XRFDC_GEN3 ? 520000 :
								     614000;
		Wpl = RdWidth;
		XRFdc_GetDecimationFactor(&RFdcInst, Tile_Id, Block_Id,
					  &InterDecim);
		*status |= XRFdc_GetMixerSettings(&RFdcInst, Type, Tile_Id,
						  Block_Id, &Mixer_Settings);
		/* TODO CR-1049584 below, remove check on coarse+bypass */
		if ((Mixer_Settings.MixerMode == XRFDC_MIXER_MODE_R2R) ||
		    XRFdc_IsHighSpeedADC(&RFdcInst, Tile_Id) ||
		    (Mixer_Settings.MixerType == XRFDC_MIXER_TYPE_COARSE &&
		     Mixer_Settings.CoarseMixFreq == XRFDC_COARSE_MIX_BYPASS)) {
			DataIQ = 1;
		} else {
			DataIQ = 2;
		}
	}
	/* set the relevant parameters */
	SampleRate = 1000000 * PLLSettings.SampleRate;
	/* Must respect FdcIn_Expected * D*clkdiv=M*Fplin, that is
     Fratio_N = FdcIn_Expected and Fratio_D=Fplin
     or Fratio_N = Fabclkdic*Cst and Fratio_D=interdecim*Wpl */
	FDCout = Fplin;
	Div_Min = (((u32)Fplin) > Fpdmax) ?
			  2 :
			  1; /* Div min can only be 1 or 2 in our case. */
	Div_Max = (u32)(Fplin / Fpdmin);
	if (Div_Max == 0) {
		Fpdmin = 10;
		Div_Max = (u32)(Fplin / Fpdmin);
	}
	if (Div_Max == 0) {
		*status |= FAIL;
		metal_log(
			METAL_LOG_ERROR,
			"\nMMCM specification violation, Fin=%f is below %d\n",
			Fplin, Fpdmin);
	}

	u32 Fratio_N;
	u32 Fratio_D;

	Fratio_N = SampleRate * DataIQ;
	Fratio_D = InterDecim * Wpl * Fplin;
	FRatio = Fratio_N / (double)Fratio_D;
	/* Try to find a clkout0_div that matches the ratio */
	for (Div = Div_Min; Div <= Div_Max + 1; Div++) {
		if ((u32)(FvcoMin * Div / (double)Fplin) ==
		    FvcoMin * Div / (double)Fplin) {
			Mult_Min = (u32)(FvcoMin * Div /
					 Fplin); /* Max VCO freq */
		} else {
			Mult_Min = (u32)(1 + FvcoMin * Div / Fplin);
		}
		Mult_Max = (u32)(FvcoMax * Div / Fplin);
		for (Mult = Mult_Max; Mult >= Mult_Min; Mult--) {
			for (i = 1; i <= 128; i++) {
				if (Div * Fratio_N * i == Mult * Fratio_D) {
					clkout0_div = i;
					found_ratio = 1;
					goto end;
				} /* that is when we find
             Fratio_N/Fratio_D == Mult/D/Clkout0_Div */
			}
		}
	}

end:
	if (Mult < 2 || Mult > 128) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM specification violation %s Tile %d: Mult=%d "
			  "is outside range 2 to 128\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  Mult);
		*status |= FAIL;
	}
	if (FRatio * Fplin < 6250) {
		metal_log(
			METAL_LOG_ERROR,
			"\nMMCM spec violation %s Tile %d: Output frequency %f"
			" < 6.25MHz is too low, may be due to"
			" interpolation/decimation too high\r\n",
			(Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			FRatio * Fplin);
		*status |= FAIL;
	}
	if (found_ratio == 0) {
		metal_log(METAL_LOG_ERROR,
			  "\nCould not find MMCM/PLL ratio for %s Tile %d "
			  "Fin %f Fout %f\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  Fplin, FRatio * Fplin);
		*status |= FAIL;
	}
	/* Check the VCO frequency and MMCM input freq is ok before
     ruining the MMCM */
	if (Fplin < 10) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM %s Tile %d specification violation: Fin=%f"
			  " Fin<10, M=%d, Div=%d, clkout0div=%d\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  FDCout, Mult, Div, clkout0_div);
		*status |= FAIL;
	}
	if ((FvcoMin * Div > (Fplin * Mult)) ||
	    (Div * FvcoMax < (Fplin * Mult))) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM %s Tile %d specification violation: VCO=%f"
			  " 800<VCO<%d, M=%d, Div=%d, clkout0div=%d\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  FDCout * Mult / (double)Div, FvcoMax, Mult, Div,
			  clkout0_div);
		*status |= FAIL;
	}
	if ((Fpdmin > Fplin / (double)Div) || (Fpdmax < Fplin / (double)Div)) {
		metal_log(METAL_LOG_ERROR,
			  "\nMMCM %s Tile %d specification violation: Fph is "
			  "outside range %d<%f<%d, M=%d, Div=%d, "
			  "clkout0div=%d\r\n",
			  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			  Fpdmin, Fplin / (double)Div, Fpdmax, Mult, Div,
			  clkout0_div);
		*status |= FAIL;
	}
	if ((Fplin * Mult / (double)(Div * clkout0_div) < FoutMin)) {
		metal_log(
			METAL_LOG_ERROR,
			"\nMMCM %s Tile %d specification violation: Fout=%fkHz<%dkHz, M=%d, Div=%d, clkout0div=%d\r\n",
			(Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			Fplin * Mult / (double)(Div * clkout0_div), FoutMin,
			Mult, Div, clkout0_div);
		*status |= FAIL;
	}
	if ((Fplin * Mult / (double)(Div * clkout0_div) > FoutMax)) {
		metal_log(
			METAL_LOG_ERROR,
			"\nMMCM %s Tile %d specification violation: Fout=%fkHz>%dkHz, M=%d, Div=%d, clkout0div=%d\r\n",
			(Type == XRFDC_ADC_TILE) ? "ADC" : "DAC", Tile_Id,
			Fplin * Mult / (double)(Div * clkout0_div), FoutMax,
			Mult, Div, clkout0_div);
		*status |= FAIL;
	}
	/* Reprogram the MMMCM */
	if (*status == SUCCESS || *status == WARN_EXECUTE) {
		usleep(200);
		(void)MMCM_Reprog_HW(Type, Tile_Id, Mult, 0, Div, clkout0_div,
				     Clk0DivFrac);
		MMCMFin[4 * Type + Tile_Id] = cmdVals[2].u;
		usleep(200);
		MMCM_Lock = MMCM_Rst_HW(Type, Tile_Id);
		if (!MMCM_Lock) {
			metal_log(METAL_LOG_ERROR,
				  "\nMMCM %s Tile %d is not locked after reset "
				  "with Fin %f, Mult %lu, Div %lu, "
				  "ClkoutDiv %lu, try shutdown/startup the tile"
				  " but it is likely you will need to "
				  "re-configure the device\r\n",
				  (Type == XRFDC_ADC_TILE) ? "ADC" : "DAC",
				  Tile_Id, Fplin, Mult, Div, clkout0_div);
			*status |= FAIL;
		}
	} else {
		metal_log(METAL_LOG_ERROR,
			  "MMCM/PLL has not been reprogrammed, it is likely the"
			  " AXI Stream clock is incorrect");
	}
	*status |=
		XRFdc_IntrClr(&RFdcInst, Type, Tile_Id, Block_Id, 0xffffffff);
	for (i = 0; i < 16; i++) {
		XRFdc_GetIntrStatus(&RFdcInst, Type, Tile_Id, Block_Id,
				    &Interrupt[i]);
		if ((0x80000003 & Interrupt[i]) != 0x00000000) {
			metal_log(
				METAL_LOG_WARNING,
				"FIFO under/overflow detected on %s Tile %d"
				" Block %d, Fabric clock frequency is probably"
				" incorrect\r\n",
				(Type == XRFDC_ADC_TILE) ? "ADC" : "DAC",
				Tile_Id, Block_Id);
			*status |= WARN_EXECUTE;
		}
	}
	if (*status == SUCCESS) {
		sprintf(Response, " %u %u %u %u %u ", MMCM_Lock, Mult, Div,
			clkout0_div, Clk0DivFrac);
		strncat(txstrPtr, Response, BUF_MAX_LEN);
	}
}

int Write32TDDRTSCtrlOffset(u32 offset, u32 value)
{
	int fd;
	int ret = SUCCESS;
	void *base_tddrtsctrl;

	fd = open("/dev/mem", O_RDWR | O_NDELAY);
	if (fd < 0) {
		perror("/dev/mem open failed: ");
		printf("%s: device open error\n", __func__);
		ret = FAIL;
	}
	base_tddrtsctrl =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((XPAR_EXDES_TDDRTSCTRL_0_BASEADDR) & ~MAP_MASK));
	if (base_tddrtsctrl == MAP_FAILED) {
		perror("mmap failed: ");
		printf("%s: Error mmapping the TDDRTSCTRL\n", __func__);
		close(fd);
		ret = FAIL;
	}
	*(u32 *)(base_tddrtsctrl + (offset & MAP_MASK)) = value;
	if (FAIL == ret) {
		metal_log(
			METAL_LOG_ERROR,
			"Failed to write to XPAR_EXDES_TDDRTSCTRL register\n");
	}
	close(fd);
	return ret;
}

int Read32TDDRTSCtrlOffset(u32 offset, u32 *value)
{
	int fd;
	int ret = SUCCESS;
	void *base_tddrtsctrl;

	fd = open("/dev/mem", O_RDWR | O_NDELAY);
	if (fd < 0) {
		perror("/dev/mem open failed: ");
		printf("%s: device open error\n", __func__);
		ret = FAIL;
	}
	base_tddrtsctrl =
		mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE, MAP_SHARED,
		     info.fd, ((XPAR_EXDES_TDDRTSCTRL_0_BASEADDR) & ~MAP_MASK));
	if (base_tddrtsctrl == MAP_FAILED) {
		perror("mmap failed: ");
		printf("%s: Error mmapping the TDDRTSCTRL\n", __func__);
		close(fd);
		ret = FAIL;
	}
	*value = *(u32 *)(base_tddrtsctrl + (offset & MAP_MASK));
	if (FAIL == ret) {
		metal_log(
			METAL_LOG_ERROR,
			"Failed to read from XPAR_EXDES_TDDRTSCTRL register\n");
	}
	close(fd);
	return ret;
}

void SetTDDRTSPinCtrl(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 Type;
	u32 TDD_ModePin;
	u32 Offset;

	Type = cmdVals[0].u;
	TDD_ModePin = cmdVals[1].u;

	/* Set TDD mode pins to assert on trigger */
	if (Type == XRFDC_ADC_TILE) {
		Offset = 0x4;
	} else {
		Offset = 0x0;
	}
	*status = Write32TDDRTSCtrlOffset(Offset, TDD_ModePin);

	return;
}

void GetTDDRTSPinCtrl(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Type;
	u32 Offset;
	u32 ReadReg;

	Type = cmdVals[0].u;
	if (Type == XRFDC_ADC_TILE) {
		Offset = 0x4;
	} else {
		Offset = 0x0;
	}
	*status = Read32TDDRTSCtrlOffset(Offset, &ReadReg);
	sprintf(Response, " 0x%08X  ", ReadReg);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	return;
}
/* Get/Set the trigger delay for ADC per tile.
   This is the delay between the tdd trigger (from SetTDDRTSTrigSlot) to
   the hw_trigger of the capture memory
   In AXI control clock cycles (slow clock), then resync to tile clock.
*/

void GetTDDRTSTrigDelay(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Tile_Id;
	u32 Trig_Delay;
	u32 Offset;
	Tile_Id = cmdVals[0].u;

	Offset = 0x30 + 4 * Tile_Id;
	*status = Read32TDDRTSCtrlOffset(Offset, &Trig_Delay);
	sprintf(Response, " %zu  ", (size_t)Trig_Delay);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	return;
}

void SetTDDRTSTrigDelay(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 Tile_Id;
	u32 Trig_Delay;
	u32 Offset;

	Tile_Id = cmdVals[0].u;
	Trig_Delay = cmdVals[1].u;

	/* Set trigger delay regs, 1 per tile */
	Offset = 0x30 + (4 * Tile_Id);
	*status = Write32TDDRTSCtrlOffset(Offset, Trig_Delay);
	return;
}

/* Get/Set the trigger.
0: we stop triggering the memory automatically via hw_trigger
1: we enable triggering of capture memory.s

*/
void SetTDDRTSTrig(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 Trig;
	u32 Offset;
	Trig = cmdVals[0].u;

	Offset = 0x18;
	*status = Write32TDDRTSCtrlOffset(Offset, Trig);
	return;
}
void GetTDDRTSTrig(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Trig;
	u32 Offset;

	Offset = 0x18;
	*status = Read32TDDRTSCtrlOffset(Offset, &Trig);
	sprintf(Response, " %zu  ", (size_t)Trig);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	return;
}

/* get/set which ofdm frame and symbol to trigger on
 */
void SetTDDRTSTrigSlot(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 trig_symbol;
	u32 trig_ofdm;
	u32 Offset;

	trig_symbol = cmdVals[0].u;
	trig_ofdm = cmdVals[1].u;
	Offset = 0x10;
	*status |= Write32TDDRTSCtrlOffset(Offset, trig_symbol);
	Offset = 0x14;
	*status |= Write32TDDRTSCtrlOffset(Offset, trig_ofdm);
	return;
}

void GetTDDRTSTrigSlot(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 trig_symbol;
	u32 trig_ofdm;
	u32 Offset;

	Offset = 0x10;
	*status |= Read32TDDRTSCtrlOffset(Offset, &trig_symbol);
	Offset = 0x14;
	*status |= Read32TDDRTSCtrlOffset(Offset, &trig_ofdm);
	sprintf(Response, " %zu %zu  ", (size_t)trig_symbol, (size_t)trig_ofdm);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	return;
}

/* Get/set configuration of guard band, symbol length and slot
 * configuration (UL/DL).

*/
void GetTDDRTSSlot(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Guard_Length;
	u32 Symbol_Length;
	u32 Slot_Config;
	u32 Offset;

	*status = SUCCESS;
	Offset = 0x48;
	*status |= Read32TDDRTSCtrlOffset(Offset, &Guard_Length);
	Offset = 0x4C;
	*status |= Read32TDDRTSCtrlOffset(Offset, &Symbol_Length);
	Offset = 0x50;
	*status |= Read32TDDRTSCtrlOffset(Offset, &Slot_Config);
	sprintf(Response, " %zu %zu 0x%08X ", (size_t)Guard_Length,
		(size_t)Symbol_Length, Slot_Config);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	return;
}
void SetTDDRTSSlot(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 Guard_Length;
	u32 Symbol_Length;
	u32 Slot_Config;
	u32 Offset;

	Guard_Length = cmdVals[0].u;
	Symbol_Length = cmdVals[1].u;
	Slot_Config = cmdVals[2].u;

	/* Set Guard Length of OFDM Symbol
     Length of time in symbol before enabling UL/DL slot */
	Offset = 0x48;
	if (Guard_Length != 0) {
		Guard_Length = Guard_Length - 1;
	}
	*status |= Write32TDDRTSCtrlOffset(Offset, Guard_Length);

	/* Set Symbol Length of OFDM Symbol
     Total length of OFDM Symbol
     Max 2^17 */
	Offset = 0x4C;
	if (Symbol_Length != 0) {
		Symbol_Length = Symbol_Length - 1;
	}
	*status |= Write32TDDRTSCtrlOffset(Offset, Symbol_Length);

	/* Set slot configuration
     10 symbols per slot
     1 bit per symbol
     DL = 0 UL = 1 */
	Offset = 0x50;
	*status |= Write32TDDRTSCtrlOffset(Offset, Slot_Config);

	return;
}

/* enable/disable the hw_trigger_en on capture mem, per tile
 */
void GetTDDRTSEnables(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Enable;
	u32 Offset;

	Offset = 0xC;
	*status = Read32TDDRTSCtrlOffset(Offset, &Enable);
	sprintf(Response, " 0x%08X  ", Enable);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	return;
}

void SetTDDRTSEnables(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 Enable;
	u32 Offset;

	Enable = cmdVals[0].u;
	Offset = 0xC;
	*status = Write32TDDRTSCtrlOffset(Offset, Enable);
	return;
}
/* reset
 */
void GetTDDRTSRst(convData_t *cmdVals, char *txstrPtr, int *status)
{
	char Response[BUF_MAX_LEN] = { 0 };
	u32 Rst;
	u32 Offset;
	Offset = 0x8;
	*status = Read32TDDRTSCtrlOffset(Offset, &Rst);

	sprintf(Response, " %zu  ", (size_t)Rst);
	strncat(txstrPtr, Response, BUF_MAX_LEN);
	return;
}

void SetTDDRTSRst(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 Offset;
	u32 Rst = cmdVals[0].u;

	Offset = 0x8;
	*status = Write32TDDRTSCtrlOffset(Offset, Rst);
	return;
}
