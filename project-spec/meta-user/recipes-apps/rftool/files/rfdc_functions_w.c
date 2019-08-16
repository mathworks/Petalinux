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

#include "rfdc_functions_w.h"
#include "data_interface.h"
/**************************** Type Definitions *******************************/

#define CAL_FREEZE 0xB000504C
#define CAL_FROZEN 0xB0005048
/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern int enTermMode; /* Enable printing in terminal mode */
extern XRFdc RFdcInst;

/*****************************   Functions  **********************************/

/*
 * The API Restarts the requested tile. It can restart a single tile and
 * alternatively can restart all the tiles. Existing register settings are not
 * alternatively can restart all the tiles. Existing register settings are not
 * lost or altered in the process. It just starts the requested tile(s).
 *
 */
void StartUp (convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 Type;
	int Tile_Id;

    Type    = cmdVals[0].u;
	Tile_Id  = cmdVals[1].i;

	*status = XRFdc_StartUp (&RFdcInst, Type, Tile_Id);


	if (enTermMode) {
		if (*status != SUCCESS) {
	         printf("\nRFdc_StartUp failed\r\n");
	    } else {
			printf("    **********RFdc_StartUp*********\r\n");
			printf("\n    Type: %d\n",Type);
			printf("\n    Tile_Id: %d\n",Tile_Id);
			printf("    *********************************\r\n");
		}
	}
}

/*
 * * The API stops the tile as requested. It can also stop all the tiles if
 * asked for. It does not clear any of the existing register settings. It just
 * stops the requested tile(s)
 *
 */

void Shutdown (convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 Type;
	int Tile_Id;

	Type    = cmdVals[0].u;
	Tile_Id  = cmdVals[1].i;

	*status = XRFdc_Shutdown (&RFdcInst, Type, Tile_Id);

	if (enTermMode) {
		if (*status != SUCCESS) {
			printf("\nXRFdc_Shutdown failed\r\n");
		} else {
			printf("    **********XRFdc_Shutdown*********\r\n");
			printf("\n    Type: %d\n",Type);
			printf("\n    Tile_Id: %d\n",Tile_Id);
			printf("    *********************************\r\n");
		}
    }
}

void GetIPStatus (convData_t *cmdVals, char *txstrPtr, int *status)
{
	int Tile_Id;
	char Response[BUF_MAX_LEN]={0};
	XRFdc_IPStatus ipStatus;

	/* calling this function gets the status of the IP */
	*status = XRFdc_GetIPStatus(&RFdcInst, &ipStatus);

    /* format response */
    for (Tile_Id = 0; Tile_Id <= 3; Tile_Id++) {
    	sprintf(Response," %d %d %d %d %d %d %d %d %d %d ",
						ipStatus.DACTileStatus[Tile_Id].IsEnabled,ipStatus.DACTileStatus[Tile_Id].BlockStatusMask,
    					ipStatus.DACTileStatus[Tile_Id].TileState,ipStatus.DACTileStatus[Tile_Id].PowerUpState,
						ipStatus.DACTileStatus[Tile_Id].PLLState,
						ipStatus.ADCTileStatus[Tile_Id].IsEnabled,ipStatus.ADCTileStatus[Tile_Id].BlockStatusMask,
						ipStatus.ADCTileStatus[Tile_Id].TileState,ipStatus.ADCTileStatus[Tile_Id].PowerUpState,
						ipStatus.ADCTileStatus[Tile_Id].PLLState);
    	strncat (txstrPtr, Response,BUF_MAX_LEN);
    }

	sprintf(Response," %d ", ipStatus.State);
	strncat (txstrPtr, Response,BUF_MAX_LEN);

	if(enTermMode) {
 	   if (*status != SUCCESS) {
			printf("\nXRFdc_GetIPStatus failed\r\n");
 	   } else {
			printf("    **********XRFdc_GetIPStatus*********\r\n");
			printf("IP Status State: %d\r\n",ipStatus.State);

 		   for (Tile_Id = 0; Tile_Id <= 3; Tile_Id++) {
				/* DAC */
				printf("Tile: %d DAC Enabled= %d \r\n", Tile_Id,ipStatus.DACTileStatus[Tile_Id].IsEnabled);
				printf("  BlockStatus:  0x%x\r\n",   ipStatus.DACTileStatus[Tile_Id].BlockStatusMask);
				printf("  TileState:    0x%08x\r\n", ipStatus.DACTileStatus[Tile_Id].TileState);
				printf("  PowerUpState: 0x%08x\r\n", ipStatus.DACTileStatus[Tile_Id].PowerUpState);
				printf("  PLLState:     0x%08x\r\n", ipStatus.DACTileStatus[Tile_Id].PLLState);

 			/* ADC */
				printf("Tile: %d ADC Enabled= %d \r\n", Tile_Id,ipStatus.ADCTileStatus[Tile_Id].IsEnabled);
				printf("  BlockStatus:  0x%x\r\n", ipStatus.ADCTileStatus[Tile_Id].BlockStatusMask);
				printf("  TileState:    0x%08x\r\n", ipStatus.ADCTileStatus[Tile_Id].TileState);
				printf("  PowerUpState: 0x%08x\r\n", ipStatus.ADCTileStatus[Tile_Id].PowerUpState);
				printf("  PLLState:     0x%08x\r\n", ipStatus.ADCTileStatus[Tile_Id].PLLState);
				printf("    *********************************\r\n");
 	   }
     }
   }
}


/*
 * The API is a wrapper function used as a bridge with the command interface. The function calls
 * XRFdc_SetMixerSettings driver.
 * the function read the Mixer and NCO settings for the a given hardware target <Type,Tile_Id,Block_Id>
 * The results are returned to command interface over the tx buffer
 */

void SetMixerSettings(convData_t *cmdVals, char *txstrPtr,  int *status) {

   u32 Type;
   int Tile_Id;
   u32 Block_Id;

   // Mixer settings struct
   XRFdc_Mixer_Settings Mixer_Settings;

   Type                           = cmdVals[0].u;
   Tile_Id                        = cmdVals[1].i;
   Block_Id                       = cmdVals[2].u;
   Mixer_Settings.Freq            = cmdVals[3].d;
   Mixer_Settings.PhaseOffset     = cmdVals[4].d;
   Mixer_Settings.EventSource     = cmdVals[5].u;
   Mixer_Settings.MixerType 	  = cmdVals[6].u;
   Mixer_Settings.CoarseMixFreq   = cmdVals[7].u; // 2
   Mixer_Settings.MixerMode 	  = cmdVals[8].u;
   Mixer_Settings.FineMixerScale  = cmdVals[9].u; // 0

   // execute driver
   *status = XRFdc_SetMixerSettings(&RFdcInst, Type, Tile_Id, Block_Id, &Mixer_Settings);
   if(enTermMode) {
	   if (*status != SUCCESS) {
		   printf("\nXRFdc_SetMixerSettings failed\r\n");
	   } else {
		   printf("    **********XRFdc_SetMixerSettings*********\r\n");
		   printf("\n    Type: %d\n",Type);
		   printf("\n    Tile_id: %d\n",Tile_Id);
		   printf("\n    Block_id: %d\n",Block_Id);
		   printf("    FREQ:              %fMHz\n", Mixer_Settings.Freq);
		   printf("    PHASE OFFSET:      %f\n", Mixer_Settings.PhaseOffset);
		   printf("    EVENT SOURCE:      %d\n", Mixer_Settings.EventSource);
		   printf("    FINE MIXER MODE:   %d\n", Mixer_Settings.MixerMode);
		   printf("    COARSE MIXER FREQ: %d\n", Mixer_Settings.CoarseMixFreq);
		   printf("    Mixer Type: %d\n", Mixer_Settings.MixerType);
		   printf("    FINE MIXER SCALE: %d\n", Mixer_Settings.FineMixerScale);
	       printf("    *********************************\r\n");
       }
   }
   return;
}

/*
 * GetMixerSettings is a wrapper function used as a bridge with the command interface. The function calls
 * XRFdc_GetMixerSettings driver.
 * Mixer and NCO settings passed in are used to update the corresponding hardware registers
 * XRFdc_Mixer_Settings struct is updated with the input values received from command interface
 * Type,Tile_Id,Block_Id are also received over the command interface and passed as input to XRFdc_GetMixerSettings
 */

void GetMixerSettings (convData_t *cmdVals, char *txstrPtr, int *status) {

   u32 Type;
   int Tile_Id;
   u32 Block_Id;
   char Response[BUF_MAX_LEN]={0};
   u32 FineMixerMode = 0;
   u32 CoarseMixMode = 0;

   // Mixer settings struct
   XRFdc_Mixer_Settings Mixer_Settings;

   Type                           = cmdVals[0].u;
   Tile_Id                        = cmdVals[1].i;
   Block_Id                       = cmdVals[2].u;

   // Get MixerSettings
   *status =  XRFdc_GetMixerSettings(&RFdcInst, Type, Tile_Id, Block_Id, &Mixer_Settings);
   //*status = SUCCESS;

   if (enTermMode) {
      if (*status != SUCCESS) {
         printf("\nXRFdc_GetMixerSettings failed\r\n");
      } else {

		 printf("    **********XRFdc_GetMixerSettings*********\r\n");
         printf("\n    Type: %d\n",Type);
         printf("\n    Tile_id: %d\n",Tile_Id);
         printf("\n    Block_id: %d\n",Block_Id);
         printf("    FREQ:              %fMHz\r\n", Mixer_Settings.Freq);
         printf("    PHASE OFFSET:      %f\r\n", Mixer_Settings.PhaseOffset);
         printf("    EVENT SOURCE:      %d\r\n", Mixer_Settings.EventSource);
         printf("    COARSE MIXER FREQ: %d\r\n", Mixer_Settings.CoarseMixFreq);
         printf("    MIXER MODE:   %d\r\n", Mixer_Settings.MixerMode);
		 printf("    Mixer Type: %d\n", Mixer_Settings.MixerType);
         printf("    FINE MIXER SCALE: %d\r\n",Mixer_Settings.FineMixerScale);
         printf("    *********************************\r\n");
      }
   }

   if (*status == SUCCESS) {
	   sprintf(Response," %d %d %d %f %f %d %d %d %d %d",Type,Tile_Id,Block_Id,Mixer_Settings.Freq, Mixer_Settings.PhaseOffset,Mixer_Settings.EventSource,Mixer_Settings.MixerType ,Mixer_Settings.CoarseMixFreq,Mixer_Settings.MixerMode,Mixer_Settings.FineMixerScale);
   	   strncat (txstrPtr,Response,BUF_MAX_LEN);
   }

   return;
}

/*
 * GetQMCSettings is a wrapper function used as a bridge with the command interface.
 * QMC settings are from the API returned back to the through this function
 */

void GetQMCSettings(convData_t *cmdVals, char *txstrPtr, int *status) {
	u32 Type;
	int Tile_Id;
	u32 Block_Id;
	char Response[BUF_MAX_LEN]={0};

	// QMC settings struct
	XRFdc_QMC_Settings QMC_Settings;

	Type                           = cmdVals[0].u;
	Tile_Id                        = cmdVals[1].i;
	Block_Id                       = cmdVals[2].u;

    *status = XRFdc_GetQMCSettings(&RFdcInst, Type, Tile_Id, Block_Id, &QMC_Settings);
    //int XRFdc_GetQMCSettings(XRFdc* InstancePtr, u32 Type, int Tile_Id,u32 Block_Id, XRFdc_QMC_Settings * QMC_Setting
    if (enTermMode) {
       if (*status != SUCCESS) {
		  printf("XRFdc_GetQMCSettings() failed\n\r");
	   } else {

		   printf("    **********XRFdc_GetQMCSettings***********\r\n");
       	   printf("    GainCorrectionFactor:   %f\r\n", QMC_Settings.GainCorrectionFactor);
       	   printf("    PhaseCorrectionFactor:  %f\r\n", QMC_Settings.PhaseCorrectionFactor);
       	   printf("    EnablePhase:            %d\r\n", QMC_Settings.EnablePhase);
       	   printf("    EnableGain:             %d\r\n", QMC_Settings.EnableGain);
       	   printf("    OffsetCorrectionFactor: %d\r\n", QMC_Settings.OffsetCorrectionFactor);
       	   printf("    EventSource:            %d\r\n", QMC_Settings.EventSource);
      }
    }

    if (*status == SUCCESS) {
    	sprintf(Response," %d %d %d %f %f %d %d %d %d",Type,Tile_Id,Block_Id,QMC_Settings.GainCorrectionFactor, QMC_Settings.PhaseCorrectionFactor,QMC_Settings.EnablePhase,QMC_Settings.EnableGain,QMC_Settings.OffsetCorrectionFactor,QMC_Settings.EventSource);
    	strncat(txstrPtr,Response,BUF_MAX_LEN);
    }

}
/*****************************************************************************/
/**
*
* The API resets the requested tile. It can reset all the tiles as well. In
* the process, all existing register settings are cleared and are replaced
* with the settings initially configured (through the GUI).
*

******************************************************************************/


void Reset (convData_t *cmdVals, char *txstrPtr, int *status){
	u32 Type;
	int Tile_Id;

    Type    = cmdVals[0].u;
	Tile_Id  = cmdVals[1].i;
	*status = XRFdc_Reset (&RFdcInst, Type, Tile_Id);

	if (enTermMode) {
	      if (*status != SUCCESS) {
	         printf("\nXRFdc_Reset failed\r\n");
	      } else {

			 printf("    **********XRFdc_Reset*********\r\n");
	         printf("\n    Type: %d\n",Type);
	         printf("\n    Tile_Id: %d\n",Tile_Id);
	         printf("    *********************************\r\n");
	      }
    }
}

/*
 *  Set DAC/ADC QMC Gain,Phase,Offset settings
 */

void SetQMCSettings(convData_t *cmdVals, char *txstrPtr, int *status) {
    	u32 Type;
    	int Tile_Id;
    	u32 Block_Id;

    	// QMC settings struct
    	XRFdc_QMC_Settings QMC_Settings;

    	Type                           = cmdVals[0].u;
    	Tile_Id                        = cmdVals[1].i;
    	Block_Id                       = cmdVals[2].u;
    	QMC_Settings.EnablePhase                    = cmdVals[3].u;
    	QMC_Settings.EnableGain                     = cmdVals[4].u;;
    	QMC_Settings.GainCorrectionFactor           = cmdVals[5].d;;
    	QMC_Settings.PhaseCorrectionFactor          = cmdVals[6].d;
    	QMC_Settings.OffsetCorrectionFactor         = cmdVals[7].i;;
    	QMC_Settings.EventSource                    = cmdVals[8].u;;
    	// execute driver
    	   *status = XRFdc_SetQMCSettings(&RFdcInst, Type, Tile_Id, Block_Id, &QMC_Settings);
    	   if(enTermMode) {
    		   if (*status != SUCCESS) {
    			   printf("\nXRFdc_SetQMCSettings failed\r\n");
    		   } else {
    			   printf("    **********XRFdc_SetQMCSettings*********\r\n");
    			   printf("\n    Type: %d\n",Type);
    			   printf("\n    Tile_id: %d\n",Tile_Id);
    			   printf("\n    Block_id: %d\n",Block_Id);
    			   printf("    EnablePhase: %d\n", QMC_Settings.EnablePhase);
    			   printf("    EnableGain:  %d\n", QMC_Settings.EnableGain);
    			   printf("    GainCorrectionFactor:      %f\n", QMC_Settings.GainCorrectionFactor);
    			   printf("    PhaseCorrectionFactor:   %f\n", QMC_Settings.PhaseCorrectionFactor);
    			   printf("    OffsetCorrectionFactor: %d\n", QMC_Settings.OffsetCorrectionFactor);
    			   printf("    EventSource: %d\n", QMC_Settings.EventSource);
    		       printf("    *********************************\r\n");
    	       }
    	   }
    	   return;
}

// CoarseDelay_Settings 
void GetCoarseDelaySettings(convData_t *cmdVals, char *txstrPtr, int *status) {
	u32 Type;
	int Tile_Id;
	u32 Block_Id;
	char Response[BUF_MAX_LEN]={0};
	Type                           = cmdVals[0].u;
	Tile_Id                        = cmdVals[1].i;
	Block_Id                       = cmdVals[2].u;
	XRFdc_CoarseDelay_Settings Settings;

    *status = XRFdc_GetCoarseDelaySettings(&RFdcInst, Type, Tile_Id, Block_Id, &Settings);
    
    if (enTermMode) {
       printf("enTermMode help no done yet\n\r");
       if (*status != SUCCESS) {
            printf("XRFdc_GetCoarseDelaySettings() failed\n\r");
    } else {
        printf("    **********XRFdc_GetCoarseDelaySettings***********\r\n");
        printf("    CoarseDelay: %d\r\n", Settings.CoarseDelay);
        printf("    EventSource:            %d\r\n", Settings.EventSource);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d %d %d",Type,Tile_Id,Block_Id,Settings.CoarseDelay, Settings.EventSource);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

void SetCoarseDelaySettings(convData_t *cmdVals, char *txstrPtr, int *status) {
        u32 Type;
        int Tile_Id;
        u32 Block_Id;
        XRFdc_CoarseDelay_Settings Settings;

        Type                           = cmdVals[0].u;
        Tile_Id                        = cmdVals[1].i;
        Block_Id                       = cmdVals[2].u;
        Settings.CoarseDelay                    = cmdVals[3].u;
        Settings.EventSource                    = cmdVals[8].u;;

        *status = XRFdc_SetCoarseDelaySettings(&RFdcInst, Type, Tile_Id, Block_Id, &Settings);
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("\nXRFdc_SetCoarseDelaySettings failed\r\n");
            } else {
                printf("    **********XRFdc_SetQMCSettings*********\r\n");
                printf("\n    Type: %d\n",Type);
                printf("\n    Tile_id: %d\n",Tile_Id);
                printf("\n    Block_id: %d\n",Block_Id);
                printf("    CoarseDelay: %d\n", Settings.CoarseDelay);
                printf("    EventSource: %d\n", Settings.EventSource);
                printf("    *********************************\r\n");
            }
        }
        return;
}

// Interpolation Factor 


void GetInterpolationFactor(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u32 InterpolationFactor;

    *status = XRFdc_GetInterpolationFactor(&RFdcInst, Tile_Id, Block_Id, &InterpolationFactor);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetInterpolationFactor() failed\n\r");
    } else {

        printf("    **********XRFdc_GetInterpolationFactor***********\r\n");
        printf("    InterpolationFactor:            %d\r\n", InterpolationFactor);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d",Tile_Id,Block_Id,InterpolationFactor);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

void SetInterpolationFactor(convData_t *cmdVals, char *txstrPtr, int *status) {

        int Tile_Id;
        u32 Block_Id;
        u32 InterpolationFactor;

        Tile_Id                        = cmdVals[0].i;
        Block_Id                       = cmdVals[1].u;
        InterpolationFactor            = cmdVals[2].u;

        *status = XRFdc_SetInterpolationFactor(&RFdcInst, Tile_Id, Block_Id, InterpolationFactor);
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("\nXRFdc_SetInterpolationFactor failed\r\n");
            } else {
                printf("    **********XRFdc_SetInterpolationFactor*********\r\n");
                printf("\n    Tile_id: %d\n",Tile_Id);
                printf("\n    Block_id: %d\n",Block_Id);
                printf("    Interpolation Factor: %d\n", InterpolationFactor);
                printf("    *********************************\r\n");
            }
        }
        return;
}

/*
 * Get ADC Decimation Factor
 */

void GetDecimationFactor(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u32 DecimationFactor;

    *status = XRFdc_GetDecimationFactor(&RFdcInst, Tile_Id, Block_Id, &DecimationFactor);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetDecimationFactor() failed\n\r");
    } else {

        printf("    **********XRFdc_GetDecimationFactor***********\r\n");
        printf("    Decimation Factor:            %d\r\n", DecimationFactor);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d",Tile_Id,Block_Id,DecimationFactor);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Set ADC Decimation Factor
 */

void SetDecimationFactor(convData_t *cmdVals, char *txstrPtr, int *status) {

        int Tile_Id;
        u32 Block_Id;
        u32 DecimationFactor;
		int ret = 0;

        Tile_Id                        = cmdVals[0].i;
        Block_Id                       = cmdVals[1].u;
        DecimationFactor               = cmdVals[2].u;

        *status = XRFdc_SetDecimationFactor(&RFdcInst, Tile_Id, Block_Id, DecimationFactor);
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("XRFdc_SetDecimationFactor failed\r\n");
            } else {
                printf("    **********XRFdc_SetDecimationFactor*********\r\n");
                printf("\n    Tile_id: %d\n",Tile_Id);
                printf("\n    Block_id: %d\n",Block_Id);
                printf("    Decimation Factor: %d\n", DecimationFactor);
                printf("    *********************************\r\n");
            }
        }

        return;
}

/*
 * Get DAC or ADC Nyquist Zone
 */

void GetNyquistZone(convData_t *cmdVals, char *txstrPtr, int *status) {
	u32 Type;
	int Tile_Id;
	u32 Block_Id;
	char Response[BUF_MAX_LEN]={0};
	Type                           = cmdVals[0].u;
	Tile_Id                        = cmdVals[1].i;
	Block_Id                       = cmdVals[2].u;
	u32 NyquistZone;

    *status = XRFdc_GetNyquistZone(&RFdcInst, Type, Tile_Id, Block_Id, &NyquistZone);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetNyquistZone() failed\n\r");
    } else {

        printf("    **********XRFdc_GetNyquistZone***********\r\n");
        printf("    NyquistZone:            %d\r\n", NyquistZone);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d %d",Type,Tile_Id,Block_Id,NyquistZone);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 *  Set DAC or ADC Nyquist Zone
 */

void SetNyquistZone(convData_t *cmdVals, char *txstrPtr, int *status) {

        u32 Type;
        int Tile_Id;
        u32 Block_Id;
        Type                           = cmdVals[0].u;
        Tile_Id                        = cmdVals[1].i;
        Block_Id                       = cmdVals[2].u;
        u32 NyquistZone                = cmdVals[3].u ;

        *status = XRFdc_SetNyquistZone(&RFdcInst, Type, Tile_Id, Block_Id, NyquistZone);
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("XRFdc_SetNyquistZone failed\r\n");
            } else {
                printf("    **********XRFdc_SetNyquistZone*********\r\n");
                printf("\n    Type: %d\n",Type);
                printf("\n    Tile_id: %d\n",Tile_Id);
                printf("\n    Block_id: %d\n",Block_Id);
                printf("    NyquistZone: %d\n", NyquistZone);
                printf("    *********************************\r\n");
            }
        }
        return;
}

/*
 * Get DAC Output Current
 */

void GetOutputCurr(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    int OutputCurr;

    *status = XRFdc_GetOutputCurr(&RFdcInst, Tile_Id, Block_Id, &OutputCurr);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetOutputCurr() failed\n\r");
    } else {

        printf("    **********XRFdc_GetOutputCurr***********\r\n");
        printf("    OutputCurr:            %d\r\n", OutputCurr);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d",Tile_Id,Block_Id,OutputCurr);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Get PLL Lock Status
 */

void GetPLLLockStatus(convData_t *cmdVals, char *txstrPtr, int *status) 
{
	u32 Type;
	int Tile_Id;
	char Response[BUF_MAX_LEN]={0};
	Type                           = cmdVals[0].u;
	Tile_Id                        = cmdVals[1].i;
	u32 LockStatus;

	*status = XRFdc_GetPLLLockStatus(&RFdcInst, Type, Tile_Id, &LockStatus);

	if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetPLLLockStatus() failed\n\r");
    } else {

        printf("    **********XRFdc_GetPLLLockStatus***********\r\n");
        printf("    LockStatus:            %d\r\n", LockStatus);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d",Type ,Tile_Id,LockStatus);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Get Clock Source
 */

void GetClockSource(convData_t *cmdVals, char *txstrPtr, int *status) {
        u32 Type;
        int Tile_Id;
        char Response[BUF_MAX_LEN]={0};

	Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;
    u32 ClockSource = 0;

    *status = XRFdc_GetClockSource(&RFdcInst, Type, Tile_Id, &ClockSource);
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetClockSource() failed\n\r");
    } else {

        printf("    **********XRFdc_GetPLLLockStatus***********\r\n");
        printf("    ClockSource:            %d\r\n", ClockSource);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d ",Type ,Tile_Id,ClockSource);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Dynamic PLL Configuration
 */
void GetPLLdividers (convData_t *cmdVals, char *txstrPtr, int *status) {
	u32 Type;
	int Tile_Id;
	Type                           = cmdVals[0].u;
	Tile_Id                        = cmdVals[1].i;
    /*placeholder*/

}


void DynamicPLLConfig(convData_t *cmdVals, char *txstrPtr, int *status) {
     u32 Type;
     int Tile_Id;
     Type                           = cmdVals[0].u;
     Tile_Id                        = cmdVals[1].i;
     u8 Source                      = cmdVals[2].u;
     double RefClkFreq              = cmdVals[3].d;
     double SamplingRate            = cmdVals[4].d;
     char Response[BUF_MAX_LEN]={0};
     u32 RefClkDivider = 0;
     u32 FeedbackDivider=0;
     u32 OutputDivider=0;

	 //XRFdc *RFdcInstPtr = &RFdcInst;
     *status = XRFdc_DynamicPLLConfig(&RFdcInst, Type, Tile_Id, Source, RefClkFreq, SamplingRate);

    
	if(Type == XRFDC_ADC_TILE) {
		RefClkDivider = RFdcInst.ADC_Tile[Tile_Id].PLL_Settings.RefClkDivider ;
		FeedbackDivider = RFdcInst.ADC_Tile[Tile_Id].PLL_Settings.FeedbackDivider ;
		OutputDivider =RFdcInst.ADC_Tile[Tile_Id].PLL_Settings.OutputDivider ;
	} else {
		RefClkDivider = RFdcInst.DAC_Tile[Tile_Id].PLL_Settings.RefClkDivider;
		FeedbackDivider = RFdcInst.DAC_Tile[Tile_Id].PLL_Settings.FeedbackDivider;
		OutputDivider = RFdcInst.DAC_Tile[Tile_Id].PLL_Settings.OutputDivider;
	}

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_DynamicPLLConfig() failed\n\r");
       } else {

          printf("    **********XRFdc_DynamicPLLConfig***********\r\n");
          printf("    Source:            %d\r\n", Source);
          printf("    RefClkFreq:            %fMHz\r\n", RefClkFreq);
          printf("    SamplingRate:          %fMHz\r\n", SamplingRate);

          printf("    RefClkDivider:            %d\r\n", RefClkDivider);
          printf("    FeedbackDivider:          %d\r\n", FeedbackDivider);
          printf("    OutputDivider:            %d\r\n", OutputDivider);
        }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d ",RefClkDivider ,FeedbackDivider,OutputDivider);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
    return;
}

/*
 * Set Fabric ClkOut Divide
 */

void SetFabClkOutDiv(convData_t *cmdVals, char *txstrPtr, int *status) {
    u32 Type;
    int Tile_Id;
    Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;
    u16 FabClkDiv                  = cmdVals[2].u;
	u32 RdWidth  = 0;
	u32 WrWidth  = 0;
	u32 Wdc      = 0;
	u32 Wpl      = 0;
	u32 Block_Id = 0;
	u32 Dec8     = 1;
	double FRatio = 0;
	u32 FRatio_previous  = 0;
	u32 DecimationFactor = 0;

	if ( FabClkDiv>5 || FabClkDiv <= 0 ) {
		*status =  FAIL;
		metal_log(METAL_LOG_ERROR, "\n FabClkDiv is out of bound, Max is 16 (0x5) %s \r\n", __func__);
		return;
	}

	for (Block_Id=0;Block_Id< 4>>(Type==ADC && RFdcInst.ADC4GSPS);Block_Id++) {

		if (XRFdc_IsBlockEnabled(&RFdcInst, Type, Tile_Id,Block_Id)) {

			XRFdc_GetFabRdVldWords(&RFdcInst, Type, Tile_Id,Block_Id, &RdWidth);
			XRFdc_GetFabWrVldWords(&RFdcInst, Type, Tile_Id,Block_Id, &WrWidth);
			if (Type==DAC) {
				Wdc=RdWidth;
				Wpl=WrWidth;
				Dec8 = 1;
			} else {
				Wdc=WrWidth;
				Wpl=RdWidth;
				XRFdc_GetDecimationFactor(&RFdcInst, Tile_Id, Block_Id, &DecimationFactor);
				if (!RFdcInst.ADC4GSPS && DecimationFactor == 8) {
					Dec8 = 2;
				} else {
					Dec8 = 1;
				}
			}
			FRatio       = (Wdc*(1<<(FabClkDiv-1)))/ Dec8 / Wpl;

			if (FRatio != (int)FRatio) {
				metal_log(METAL_LOG_WARNING, "\nTile_Id %d Block_Id %d has a non integer ratio %f between Fout and Fin, not supported in this version\r\n", Tile_Id, Block_Id,FRatio);
				*status 	|= ERROR_EXECUTE   		;
				FRatio  	=  FRatio_previous 		;

			} else if ((FRatio_previous!=0 && FRatio!=FRatio_previous)) {
				metal_log(METAL_LOG_WARNING, "\nTile_Id %d Block_Id %d has a different ratio %f between Fout and Fin, previous ratio %d will be used, try changing Decimation or Interpolation or IQ mode\r\n", Tile_Id, Block_Id,FRatio,FRatio_previous);
				*status |= ERROR_EXECUTE;
				FRatio 		= FRatio_previous 		;

			} else {
				FRatio_previous      = FRatio 		;

			}
		}
	}

    *status |= XRFdc_SetFabClkOutDiv(&RFdcInst, Type, Tile_Id,  FabClkDiv);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
           printf("XRFdc_SetFabClkOutDiv() failed\n\r");
    } else {
        printf("    **********XRFdc_SetFabClkOutDiv***********\r\n");
        printf("    FabClkDiv:            %d\r\n", FabClkDiv);
      }
    }
    return;
}

/*
 * Stop/start FIFO
 */

void SetupFIFO(convData_t *cmdVals, char *txstrPtr, int *status) {
    u32 Type;
    int Tile_Id;
    Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;
    u8 enable                      = cmdVals[2].u;

    *status = XRFdc_SetupFIFO(&RFdcInst, Type, Tile_Id,  enable);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_SetupFIFO() failed\n\r");
    } else {

        printf("    **********XRFdc_SetupFIFO***********\r\n");
        printf("    enable:            %d\r\n", enable);

      }
    }
    return;
}

/*
 * Get FIFO Status
 */

void GetFIFOStatus(convData_t *cmdVals, char *txstrPtr, int *status) {
        u32 Type;
        int Tile_Id;
        char Response[BUF_MAX_LEN]={0};
        Type                           = cmdVals[0].u;
        Tile_Id                        = cmdVals[1].i;
        u8 enable                     ;

    *status = XRFdc_GetFIFOStatus(&RFdcInst, Type, Tile_Id,  &enable);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetFIFOStatus() failed\n\r");
    } else {

        printf("    **********XRFdc_GetFIFOStatus***********\r\n");
        printf("    enable:            %d\r\n", enable);

      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d ",Type ,Tile_Id,enable);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Set Fabric Write Valid Words
 */

void SetFabWrVldWords(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u32 FabricDataRate             = cmdVals[2].u;

    *status = XRFdc_SetFabWrVldWords(&RFdcInst, Tile_Id,Block_Id,  FabricDataRate);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_SetFabWrVldWords() failed\n\r");
    } else {
        printf("    **********XRFdc_SetFabWrVldWords***********\r\n");
        printf("    FabricDataRate:            %d\r\n", FabricDataRate);
      }
    }
    return;
}

/*
 * Get Fabric Write Valid dWords
 */

void GetFabWrVldWords(convData_t *cmdVals, char *txstrPtr, int *status) {
    u32 Type;
    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
    Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;
    Block_Id                       = cmdVals[2].u;
    u32 FabricDataRate             ;


    *status = XRFdc_GetFabWrVldWords(&RFdcInst, Type, Tile_Id,Block_Id,  &FabricDataRate);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetFabWrVldWords() failed\n\r");
    } else {

        printf("    **********XRFdc_GetFabWrVldWords***********\r\n");
        printf("    FabricDataRate:            %d\r\n", FabricDataRate);

      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d %d ",Type ,Tile_Id,Block_Id,FabricDataRate);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * SetFabRdVldWords
 */

void SetFabRdVldWords(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u32 FabricDataRate             = cmdVals[2].u;

    *status = XRFdc_SetFabRdVldWords(&RFdcInst, Tile_Id,Block_Id,  FabricDataRate);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_SetFabRdVldWords() failed\n\r");
    } else {

        printf("    **********XRFdc_SetFabRdVldWords***********\r\n");
        printf("    FabricDataRate:            %d\r\n", FabricDataRate);

      }
    }
    return;
}

/*
 * Get Fab Read Valid dWords
 */

void GetFabRdVldWords(convData_t *cmdVals, char *txstrPtr, int *status) {
    u32 Type;
    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
    Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;
    Block_Id                       = cmdVals[2].u;
    u32 FabricDataRate             ;


    *status = XRFdc_GetFabRdVldWords(&RFdcInst, Type, Tile_Id,Block_Id,  &FabricDataRate);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetFabRdVldWords() failed\n\r");
    } else {

        printf("    **********XRFdc_GetFabRdVldWords***********\r\n");
        printf("    FabricDataRate:            %d\r\n", FabricDataRate);

      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d %d ",Type ,Tile_Id,Block_Id,FabricDataRate);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Set DAC Decoder Mode
 */

void SetDecoderMode(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;

    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u32 DecoderMode                = cmdVals[2].u;

    *status = XRFdc_SetDecoderMode(&RFdcInst, Tile_Id,Block_Id,  DecoderMode);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_SetDecoderMode() failed\n\r");
    } else {

        printf("    **********XRFdc_SetDecoderMode***********\r\n");
        printf("    DecoderMode:            %d\r\n", DecoderMode);

      }
    }
    return;
}

/*
 * Get DAC Decoder Mode
 */

void GetDecoderMode(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
   
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u32 DecoderMode             ;
 
    *status = XRFdc_GetDecoderMode(&RFdcInst, Tile_Id,Block_Id,  &DecoderMode);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetDecoderMode() failed\n\r");
    } else {

        printf("    **********XRFdc_GetDecoderMode***********\r\n");
        printf("    DecoderMode: %d\r\n", DecoderMode);

      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d ",Tile_Id,Block_Id,DecoderMode);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Request and NCO Phase reset
 */

void ResetNCOPhase(convData_t *cmdVals, char *txstrPtr, int *status) {
    u32 Type;
    int Tile_Id;
    u32 Block_Id;

    Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;
    Block_Id                       = cmdVals[2].u;

    *status = XRFdc_ResetNCOPhase(&RFdcInst,Type, Tile_Id,Block_Id);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_ResetNCOPhase() failed\n\r");
    } else {

        printf("    **********XRFdc_ResetNCOPhase***********\r\n");
        printf("    Type:            %d\r\n", Type);
        printf("    Tile_Id:            %d\r\n", Tile_Id);
        printf("    Block_Id:            %d\r\n", Block_Id);

      }
    }
    return;
}

/*
 * Dump DAC/ADC Registers value
 */

void DumpRegs(convData_t *cmdVals, char *txstrPtr, int *status) {
    u32 Type;
    int Tile_Id;
   
    Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;


    XRFdc_DumpRegs(&RFdcInst, Type, Tile_Id);
    
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_DumpRegs() failed\n\r");
    } else {

        printf("    **********XRFdc_DumpRegs***********\r\n");
        printf("    Type:            %d\r\n", Type);
        printf("    Tile_Id:            %d\r\n", Tile_Id);

      }
    }

    return;
}

/*
 * Update Event
 */

void UpdateEvent(convData_t *cmdVals, char *txstrPtr, int *status) {
    u32 Type;
    int Tile_Id;
    u32 Block_Id;
    u32 Event;

    Type                           = cmdVals[0].i;
    Tile_Id                        = cmdVals[1].i;
    Block_Id                       = cmdVals[2].u;
    Event                          = cmdVals[3].u;

    *status = XRFdc_UpdateEvent(&RFdcInst, Type, Tile_Id,Block_Id,  Event);

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_UpdateEvent() failed\n\r");
    } else {

        printf("    **********XRFdc_UpdateEvent***********\r\n");
        printf("    Event:            %d\r\n", Event);

      }
    }
    return;
}

/*
 * Get ADC calibration mode
 */

void GetCalibrationMode(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u8 CalibrationMode;

	*status = XRFdc_GetCalibrationMode(&RFdcInst, Tile_Id, Block_Id, &CalibrationMode);

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetCalibrationMode() failed\n\r");
    } else {

        printf("    **********XRFdc_GetCalibrationMode***********\r\n");
        printf("    CalibrationMode:            %d\r\n", CalibrationMode);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d",Tile_Id,Block_Id,CalibrationMode);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 *  Set ADC Calibration mode
 */

void SetCalibrationMode(convData_t *cmdVals, char *txstrPtr, int *status) {

        int Tile_Id;
        u32 Block_Id;
        u32 CalibrationMode;

        Tile_Id                        = cmdVals[0].i;
        Block_Id                       = cmdVals[1].u;
        CalibrationMode                = cmdVals[2].u;

        *status = XRFdc_SetCalibrationMode(&RFdcInst, Tile_Id, Block_Id, CalibrationMode);
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("XRFdc_SetCalibrationMode failed\r\n");
            } else {
                printf("    **********XRFdc_SetCalibrationMode*********\r\n");
                printf("\n    Tile_id: %d\n",Tile_Id);
                printf("\n    Block_id: %d\n",Block_Id);
                printf("      CalibrationMode: %d\n", CalibrationMode);
                printf("    *********************************\r\n");
            }
        }
        return;
}

/*
 * Get a data converter block status
 */

void GetBlockStatus(convData_t *cmdVals, char *txstrPtr, int *status) {
	u32 Type;
    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
    Type                           = cmdVals[0].i;
    Tile_Id                        = cmdVals[1].i;
    Block_Id                       = cmdVals[2].u;
    XRFdc_BlockStatus BlockStatus;


    *status = XRFdc_GetBlockStatus(&RFdcInst, Type, Tile_Id, Block_Id, &BlockStatus);

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetBlockStatus() failed\n\r");
    } else {

        printf("    **********XRFdc_GetBlockStatus***********\r\n");
        printf("    Type:            %d\r\n", Type);
        printf("    Tile_Id:         %d\r\n", Tile_Id);
        printf("    Block_Id:        %d\r\n", Block_Id);
        printf("    SamplingFreq:            			 %f\r\n",  BlockStatus.SamplingFreq);
        printf("    AnalogDataPathStatus:                %d\r\n",  BlockStatus.AnalogDataPathStatus);
        printf("    DigitalDataPathStatus:               %d\r\n", BlockStatus.DigitalDataPathStatus);
        printf("    DataPathClocksStatus:                %d\r\n", BlockStatus.DataPathClocksStatus);
        printf("    IsFIFOFlagsAsserted:                 %d\r\n", BlockStatus.IsFIFOFlagsAsserted);
        printf("    IsFIFOFlagsEnabled:                  %d\r\n", BlockStatus.IsFIFOFlagsEnabled);
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d %f %d %d %d %d %d", Type ,Tile_Id, Block_Id, BlockStatus.SamplingFreq, BlockStatus.AnalogDataPathStatus,
        		BlockStatus.DigitalDataPathStatus,BlockStatus.DataPathClocksStatus,BlockStatus.IsFIFOFlagsAsserted,
				BlockStatus.IsFIFOFlagsEnabled
        );
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Get ADC thresholds settings
 */

void GetThresholdSettings(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    XRFdc_Threshold_Settings ThresholdSettings;

    *status = XRFdc_GetThresholdSettings(&RFdcInst, Tile_Id, Block_Id, &ThresholdSettings);

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetThresholdSettings() failed\n\r");
    } else {

        printf("    **********XRFdc_GetThresholdSettings***********\r\n");
        printf("\n    Tile_id: %d\n",Tile_Id);
        printf("\n    Block_id: %d\n",Block_Id);
        printf("      UpdateThreshold: %d\n",      ThresholdSettings.UpdateThreshold);
        printf("      ThresholdMode[0]    : %d\n", ThresholdSettings.ThresholdMode[0]    );
        printf("      ThresholdMode[1]    : %d\n", ThresholdSettings.ThresholdMode[1]    );
        printf("      ThresholdAvgVal[0]  : %d\n", ThresholdSettings.ThresholdAvgVal[0]  );
        printf("      ThresholdAvgVal[1]  : %d\n", ThresholdSettings.ThresholdAvgVal[1]  );
        printf("      ThresholdUnderVal[0]: %d\n", ThresholdSettings.ThresholdUnderVal[0]);
        printf("      ThresholdUnderVal[1]: %d\n", ThresholdSettings.ThresholdUnderVal[1]);
        printf("      ThresholdOverVal[0] : %d\n", ThresholdSettings.ThresholdOverVal[0] );
        printf("      ThresholdOverVal[1] : %d\n", ThresholdSettings.ThresholdOverVal[1] );
        printf("    *********************************\r\n");
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d %d %d %d %d %d %d %d %d",Tile_Id,Block_Id,ThresholdSettings.UpdateThreshold, ThresholdSettings.ThresholdMode[0],
                                                      ThresholdSettings.ThresholdMode[1], ThresholdSettings.ThresholdAvgVal[0],  
                                                      ThresholdSettings.ThresholdAvgVal[1], ThresholdSettings.ThresholdUnderVal[0],
                                                      ThresholdSettings.ThresholdUnderVal[1], ThresholdSettings.ThresholdOverVal[0], 
                                                      ThresholdSettings.ThresholdOverVal[1] );
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Set ADC Thresholds settings
 */

void SetThresholdSettings(convData_t *cmdVals, char *txstrPtr, int *status) {

        int Tile_Id;
        u32 Block_Id;
        XRFdc_Threshold_Settings ThresholdSettings;

        Tile_Id                        = cmdVals[0].i;
        Block_Id                       = cmdVals[1].u;
        ThresholdSettings.UpdateThreshold      = cmdVals[2].u;
        ThresholdSettings.ThresholdMode[0]     = cmdVals[3].u;
        ThresholdSettings.ThresholdMode[1]      = cmdVals[4].u;
        ThresholdSettings.ThresholdAvgVal[0]      = cmdVals[5].u;
        ThresholdSettings.ThresholdAvgVal[1]      = cmdVals[6].u;
        ThresholdSettings.ThresholdUnderVal[0]      = cmdVals[7].u;
        ThresholdSettings.ThresholdUnderVal[1]      = cmdVals[8].u;
        ThresholdSettings.ThresholdOverVal[0]      = cmdVals[9].u;
        ThresholdSettings.ThresholdOverVal[1]      = cmdVals[10].u;

        *status = XRFdc_SetThresholdSettings(&RFdcInst, Tile_Id, Block_Id, &ThresholdSettings);
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("XRFdc_SetThresholdSettings failed\r\n");
            } else {
                printf("    **********XRFdc_SetThresholdSettings*********\r\n");
                printf("\n    Tile_id: %d\n",Tile_Id);
                printf("\n    Block_id: %d\n",Block_Id);
                printf("      UpdateThreshold: %d\n",      ThresholdSettings.UpdateThreshold);
                printf("      ThresholdMode[0]    : %d\n", ThresholdSettings.ThresholdMode[0]    );
                printf("      ThresholdMode[1]    : %d\n", ThresholdSettings.ThresholdMode[1]    );
                printf("      ThresholdAvgVal[0]  : %d\n", ThresholdSettings.ThresholdAvgVal[0]  );
                printf("      ThresholdAvgVal[1]  : %d\n", ThresholdSettings.ThresholdAvgVal[1]  );
                printf("      ThresholdUnderVal[0]: %d\n", ThresholdSettings.ThresholdUnderVal[0]);
                printf("      ThresholdUnderVal[1]: %d\n", ThresholdSettings.ThresholdUnderVal[1]);
                printf("      ThresholdOverVal[0] : %d\n", ThresholdSettings.ThresholdOverVal[0] );
                printf("      ThresholdOverVal[1] : %d\n", ThresholdSettings.ThresholdOverVal[1] );
                printf("    *********************************\r\n");
            }
        }
        return;
}

/*
 * Lookup Config of the RfDC
 */

void LookupConfig(convData_t *cmdVals, char *txstrPtr, int *status) {

    int i, j;
    char Response[BUF_MAX_LEN]={0};

    XRFdc_Config * Config;

    Config = XRFdc_LookupConfig(0/*XPAR_XRFDC_0_DEVICE_ID*/);
    *status = SUCCESS;

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_LookupConfig() failed\n\r");
    } else {

        printf("    **********XRFdc_LookupConfig***********\r\n");
        printf("      DeviceId: %d\n",      Config->DeviceId);
        printf("      BaseAddr    : %d\n", Config->BaseAddr);
        printf("      ADCType    : %d\n", Config->ADCType);
        printf("      MasterADCTile  : %d\n", Config->MasterADCTile);
        printf("      ADCSysRefSource[1]  : %d\n", Config->ADCSysRefSource);
        printf("      DACSysRefSource[0]: %d\n", Config->DACSysRefSource);
        for (i=0; i<=3; i++) {
            printf("      DAC Tile %d Enable: %d\n"         , i, Config->DACTile_Config[i].Enable        );
            printf("      DAC Tile %d PLLEnable: %d\n"      , i, Config->DACTile_Config[i].PLLEnable     );
            printf("      DAC Tile %d SamplingRate: %d\n"   , i, Config->DACTile_Config[i].SamplingRate  );
            printf("      DAC Tile %d RefClkFreq: %d\n"     , i, Config->DACTile_Config[i].RefClkFreq    );
            printf("      DAC Tile %d FabClkFreq: %d\n"     , i, Config->DACTile_Config[i].FabClkFreq    );
            for (j=0; j<=3; j++) {
                printf("      DAC Tile %d DAC %d BlockAvailable: %d\n"     , i, j, Config->DACTile_Config[i].DACBlock_Analog_Config[j].BlockAvailable     );
                printf("      DAC Tile %d DAC %d InvSyncEnable: %d\n"      , i, j, Config->DACTile_Config[i].DACBlock_Analog_Config[j].InvSyncEnable      );
                printf("      DAC Tile %d DAC %d MixMode: %d\n"            , i, j, Config->DACTile_Config[i].DACBlock_Analog_Config[j].MixMode            );
                printf("      DAC Tile %d DAC %d DecoderMode: %d\n"        , i, j, Config->DACTile_Config[i].DACBlock_Analog_Config[j].DecoderMode        );
                printf("      DAC Tile %d DAC %d DataType: %d\n"           , i, j, Config->DACTile_Config[i].DACBlock_Digital_Config[j].DataType          );
                printf("      DAC Tile %d DAC %d DataWidth: %d\n"          , i, j, Config->DACTile_Config[i].DACBlock_Digital_Config[j].DataWidth         );
                printf("      DAC Tile %d DAC %d InterpolationMode: %d\n"  , i, j, Config->DACTile_Config[i].DACBlock_Digital_Config[j].InterploationMode );
                printf("      DAC Tile %d DAC %d FifoEnable: %d\n"         , i, j, Config->DACTile_Config[i].DACBlock_Digital_Config[j].FifoEnable        );
                printf("      DAC Tile %d DAC %d AdderEnable: %d\n"        , i, j, Config->DACTile_Config[i].DACBlock_Digital_Config[j].AdderEnable       );
            }
            printf("      ADC Tile %d Enable: %d\n"         , i, Config->ADCTile_Config[i].Enable);
            printf("      ADC Tile %d PLLEnable: %d\n"      , i, Config->ADCTile_Config[i].PLLEnable);
            printf("      ADC Tile %d SamplingRate: %d\n"   , i, Config->ADCTile_Config[i].SamplingRate);
            printf("      ADC Tile %d RefClkFreq: %d\n"     , i, Config->ADCTile_Config[i].RefClkFreq);
            printf("      ADC Tile %d FabClkFreq: %d\n"     , i, Config->ADCTile_Config[i].FabClkFreq);
            for (j=0; j<=3; j++) {
                printf("      ADC Tile %d ADC %d BlockAvailable: %d\n" , i, j, Config->ADCTile_Config[i].ADCBlock_Analog_Config[j].BlockAvailable);
                printf("      ADC Tile %d ADC %d MixMode: %d\n"        , i, j, Config->ADCTile_Config[i].ADCBlock_Analog_Config[j].MixMode);
                printf("      ADC Tile %d ADC %d DataType: %d\n"       , i, j, Config->ADCTile_Config[i].ADCBlock_Digital_Config[j].DataType);
                printf("      ADC Tile %d ADC %d DataWidth: %d\n"      , i, j, Config->ADCTile_Config[i].ADCBlock_Digital_Config[j].DataWidth);
                printf("      ADC Tile %d ADC %d DecimationMode: %d\n" , i, j, Config->ADCTile_Config[i].ADCBlock_Digital_Config[j].DecimationMode);
                printf("      ADC Tile %d ADC %d FifoEnable: %d\n"     , i, j, Config->ADCTile_Config[i].ADCBlock_Digital_Config[j].FifoEnable);
            }
        }

        }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %u %lu %u %u %u %u",Config->DeviceId, Config->BaseAddr, Config->ADCType, Config->MasterADCTile, Config->ADCSysRefSource, Config->DACSysRefSource);
        for (i=0; i<=3; i++) {                                                             
            sprintf(Response," %u %u %f %f %f",    Config->DACTile_Config[i].Enable, Config->DACTile_Config[i].PLLEnable,
                                                   Config->DACTile_Config[i].SamplingRate, Config->DACTile_Config[i].RefClkFreq,
                                                   Config->DACTile_Config[i].FabClkFreq
												   );
            strncat(txstrPtr,Response,BUF_MAX_LEN);

            for (j=0; j<=3; j++) {
                sprintf(Response," %d %d %d %d %d %d %d %d %d ",
                                                     Config->DACTile_Config[i].DACBlock_Analog_Config[j].BlockAvailable      ,
                                                     Config->DACTile_Config[i].DACBlock_Analog_Config[j].InvSyncEnable       ,
                                                     Config->DACTile_Config[i].DACBlock_Analog_Config[j].MixMode             ,
                                                     Config->DACTile_Config[i].DACBlock_Analog_Config[j].DecoderMode         ,
                                                     Config->DACTile_Config[i].DACBlock_Digital_Config[j].DataType           ,
                                                     Config->DACTile_Config[i].DACBlock_Digital_Config[j].DataWidth          ,
                                                     Config->DACTile_Config[i].DACBlock_Digital_Config[j].InterploationMode  ,
                                                     Config->DACTile_Config[i].DACBlock_Digital_Config[j].FifoEnable         ,
                                                     Config->DACTile_Config[i].DACBlock_Digital_Config[j].AdderEnable        );
                strncat(txstrPtr,Response,BUF_MAX_LEN);
            }
            sprintf(Response," %d %d %f %f %f", Config->ADCTile_Config[i].Enable,
                                                Config->ADCTile_Config[i].PLLEnable,
                                                Config->ADCTile_Config[i].SamplingRate,
                                                Config->ADCTile_Config[i].RefClkFreq,
                                                Config->ADCTile_Config[i].FabClkFreq );
            strncat(txstrPtr,Response,BUF_MAX_LEN);
            for (j=0; j<=3; j++) {
                sprintf(Response," %d %d %d %d %d %d ",  
                                                     Config->ADCTile_Config[i].ADCBlock_Analog_Config[j].BlockAvailable,
                                                     Config->ADCTile_Config[i].ADCBlock_Analog_Config[j].MixMode,
                                                     Config->ADCTile_Config[i].ADCBlock_Digital_Config[j].DataType,
                                                     Config->ADCTile_Config[i].ADCBlock_Digital_Config[j].DataWidth,
                                                     Config->ADCTile_Config[i].ADCBlock_Digital_Config[j].DecimationMode,
                                                     Config->ADCTile_Config[i].ADCBlock_Digital_Config[j].FifoEnable );
               strncat(txstrPtr,Response,BUF_MAX_LEN);
            }

        }

    }

}


// RF_ReadReg RF_WriteReg
void RF_ReadReg32(convData_t *cmdVals, char *txstrPtr, int *status) {

    u32 offset;
    char Response[BUF_MAX_LEN]={0};

    u32 Value;
    offset                 = cmdVals[0].u;


    Value = XRFdc_ReadReg((&RFdcInst), 0, offset);

    *status = SUCCESS;

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_ReadReg() failed\n\r");
    } else {

        printf("    **********XRFdc_ReadReg***********\r\n");
        printf("    Offset:            %d\r\n", offset);
        printf("    Value :            %d\r\n", Value);
        printf("    *********************************\r\n");
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d ", Value);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

void RF_WriteReg32(convData_t *cmdVals, char *txstrPtr, int *status) {

    u32 offset;
    u32 Value;
    offset                 = cmdVals[0].u;
    Value                 = cmdVals[1].u;

    XRFdc_WriteReg((&RFdcInst), 0, offset, Value);

    *status = SUCCESS;
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("XRFdc_WriteReg failed\r\n");
            } else {
                printf("    **********XRFdc_WriteReg*********\r\n");
                printf("    Offset:            %d\r\n", offset);
                printf("    Value :            %d\r\n", Value);
                printf("    *********************************\r\n");
            }
        }
        return;
}


void RF_ReadReg16(convData_t *cmdVals, char *txstrPtr, int *status) {

    u32 offset;
    char Response[BUF_MAX_LEN]={0};

    u16 Value;
    offset                 = cmdVals[0].u;

    //Value = XRFdc_In16((&RFdcInst), XPAR_USP_RF_DATA_CONVERTER_0_BASEADDR + offset);
    Value = XRFdc_ReadReg16((&RFdcInst), 0, offset);
    //Value = Xil_In16(XPAR_USP_RF_DATA_CONVERTER_0_BASEADDR + offset);
    *status = SUCCESS;

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_ReadReg() failed\n\r");
    } else {

        printf("    **********XRFdc_ReadReg***********\r\n");
        printf("    Offset:            %d\r\n", offset);
        printf("    Value :            %d\r\n", Value);
        printf("    *********************************\r\n");
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d ", Value);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

void RF_WriteReg16(convData_t *cmdVals, char *txstrPtr, int *status) {

    u32 offset;
    u16 Value;
    offset                 = cmdVals[0].u;
    Value                 = cmdVals[1].u;

    //XRFdc_Out16((&RFdcInst), XPAR_USP_RF_DATA_CONVERTER_0_BASEADDR+offset, Value);
    XRFdc_WriteReg16((&RFdcInst), 0, offset, Value);
   
    *status = SUCCESS;
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("XRFdc_WriteReg failed\r\n");
            } else {
                printf("    **********XRFdc_WriteReg*********\r\n");
                printf("    Offset:            %d\r\n", offset);
                printf("    Value :            %d\r\n", Value);
                printf("    *********************************\r\n");
            }
        }
        return;
}

void RF_ReadReg8(convData_t *cmdVals, char *txstrPtr, int *status) {

    u32 offset;
    char Response[BUF_MAX_LEN]={0};

    u8 Value;
    offset                 = cmdVals[0].u;

    Value = XRFdc_ReadReg8((&RFdcInst), 0, offset);
    
    *status = SUCCESS;

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_ReadReg() failed\n\r");
    } else {

        printf("    **********XRFdc_ReadReg***********\r\n");
        printf("    Offset:            %d\r\n", offset);
        printf("    Value :            %d\r\n", Value);
        printf("    *********************************\r\n");
      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d ", Value);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

void RF_WriteReg8(convData_t *cmdVals, char *txstrPtr, int *status) {

    u32 offset;
    u8 Value;
    offset                 = cmdVals[0].u;
    Value                 = cmdVals[1].u;

    XRFdc_WriteReg8((&RFdcInst), 0, offset, Value);
    
    *status = SUCCESS;
        if(enTermMode) {
            if (*status != SUCCESS) {
                printf("XRFdc_WriteReg failed\r\n");
            } else {
                printf("    **********XRFdc_WriteReg*********\r\n");
                printf("    Offset:            %d\r\n", offset);
                printf("    Value :            %d\r\n", Value);
                printf("    *********************************\r\n");
            }
        }
        return;
}

/*
 * Birdge to API to used setup multiband configuration.
 */

void MultiBand(convData_t *cmdVals, char *txstrPtr, int *status) {
   u32 Type;
   int Tile_Id;

   u8  DigitalDataPathMask;
   u32 DataType;
   u32 DataConverterMask;


   Type                 = cmdVals[0].u;
   Tile_Id              = cmdVals[1].i;
   DigitalDataPathMask  = cmdVals[2].u;
   DataType             = cmdVals[3].u;
   DataConverterMask    = cmdVals[4].u;
   char Response[BUF_MAX_LEN]={0};

   if ((info.design_type == DAC1_ADC1) && (DataConverterMask != 1)) {
	   *status = SUCCESS;
	   goto suc;
   }
   *status = XRFdc_MultiBand(&RFdcInst,Type,Tile_Id,DigitalDataPathMask,DataType,DataConverterMask);

   if (enTermMode) {
      if (*status != SUCCESS) {
           printf("XRFdc_MultiBand() failed\n\r");
   } else {
       printf("    **********XRFdc_MultiBand***********\r\n");
       printf("    Type:                %d\r\n", Type);
       printf("    Tile_Id:             %d\r\n", Tile_Id);
       printf("    DigitalDataPathMask: %d\r\n", DigitalDataPathMask);
       printf("    DataType:            %d\r\n", DataType);
       printf("    DataConverterMask:            %d\r\n", DataConverterMask);
     }
   }

suc:
   if (*status == SUCCESS) {
       sprintf(Response," %d %d %d %d %d",Type, Tile_Id,DigitalDataPathMask,DataType,DataConverterMask);
       strncat(txstrPtr,Response,BUF_MAX_LEN);
   }

   return;
}

/*
 * Return the xrfdc revision
 */

void RfdcVersion (convData_t *cmdVals,char *txstrPtr, int *status) {

	char Response[BUF_MAX_LEN]={0};
	double version;
	version=XRFdc_GetDriverVersion();

	sprintf(Response," rfdc_v%.1f",version);
    strncat(txstrPtr,Response,BUF_MAX_LEN);

}

/*
 *  Get Data Converter connected for digital data path I and Q
 */

void GetConnectedData(convData_t *cmdVals, char *txstrPtr, int *status) {
   u32 Type;
   int Tile_Id;
   u32 Block_Id;
   int ConnectedIData;
   int ConnectedQData;
   char Response[BUF_MAX_LEN]={0};

   Type                 = cmdVals[0].u;
   Tile_Id              = cmdVals[1].i;
   Block_Id             = cmdVals[2].u;

   ConnectedIData = XRFdc_GetConnectedIData(&RFdcInst,Type,Tile_Id,Block_Id);
   ConnectedQData = XRFdc_GetConnectedQData(&RFdcInst,Type,Tile_Id,Block_Id);

   *status = SUCCESS;

   if (enTermMode) {
      if (*status != SUCCESS) {
           printf("GetConnectedIData() failed\n\r");
   } else {
       printf("    **********GetConnectedData***********\r\n");
       printf("    Type:                %d\r\n", Type);
       printf("    Tile_Id:             %d\r\n", Tile_Id);
       printf("    Block_Id: %d\r\n"           , Block_Id);
       printf("    ConnectedIData:            %d\r\n", ConnectedIData);
       printf("    ConnectedQData:            %d\r\n", ConnectedQData);
     }
   }

   if (*status == SUCCESS) {
       sprintf(Response," %d %d %d %d %d",Type, Tile_Id,Block_Id,ConnectedIData,ConnectedQData);
       strncat(txstrPtr,Response,BUF_MAX_LEN);
   }

   return;
}




// Set Inverse Sync Filter


void SetInvSincFIR(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u32 enable                     = cmdVals[2].u;

    *status = XRFdc_SetInvSincFIR(&RFdcInst, Tile_Id,Block_Id,  enable);

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_SetInvSincFIR() failed\n\r");
    } else {
        printf("    **********XRFdc_GetInvSincFIR***********\r\n");
        printf("    enable:            %d\r\n", enable);
      }
    }
    return;
}


// Get Inverse Sync Filter

void GetInvSincFIR(convData_t *cmdVals, char *txstrPtr, int *status) {

    int Tile_Id;
    u32 Block_Id;
    char Response[BUF_MAX_LEN]={0};

    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u16 enable;


    *status = XRFdc_GetInvSincFIR(&RFdcInst, Tile_Id,Block_Id,  &enable);

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetInvSincFIR() failed\n\r");
    } else {

        printf("    **********XRFdc_GetInvSincFIR***********\r\n");
        printf("    enable:            %d\r\n", enable);

      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d ",Tile_Id,Block_Id,enable);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}


// Interupt clear and Status


void IntrClr(convData_t *cmdVals, char *txstrPtr, int *status) {

	u32 Type;
    int Tile_Id;
    u32 Block_Id;
    Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;
    Block_Id                       = cmdVals[2].u;
    u32 IntrMask                   = cmdVals[3].u;

    XRFdc_IntrClr(&RFdcInst, Type, Tile_Id,Block_Id,  IntrMask);
    *status = SUCCESS;
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_IntrClr() failed\n\r");
    } else {
        printf("    **********XRFdc_IntrClr***********\r\n");
        printf("    IntrMask:            0x%x\r\n", IntrMask);
      }
    }
    return;
}


void GetIntrStatus(convData_t *cmdVals, char *txstrPtr, int *status) {

	u32 Type;
    int Tile_Id;
    u32 Block_Id;
    Type                           = cmdVals[0].u;
    Tile_Id                        = cmdVals[1].i;
    Block_Id                       = cmdVals[2].u;
    u32 IntrStatus ;

    char Response[BUF_MAX_LEN]={0};

    IntrStatus = XRFdc_GetIntrStatus(&RFdcInst, Type, Tile_Id,Block_Id);
    *status = SUCCESS;
    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetIntrStatus() failed\n\r");
    } else {

        printf("    **********XRFdc_GetIntrStatus***********\r\n");
        printf("    IntrStatus:            0x%x\r\n", IntrStatus);

      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d %d ",Type, Tile_Id,Block_Id,IntrStatus);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/*
 * Return the xrfdc revision
 */
void JtagIdcode (convData_t *cmdVals,char *txstrPtr, int *status) {

	char Response[BUF_MAX_LEN]={0};

	//u32 Idcode = Xil_In32(0xffca0040);
	u32 Idcode = 0xaa55;

	if (enTermMode) {
		printf("csu Idcode: %X \n", Idcode);
	}
	sprintf(Response," %d ",Idcode);
	strncat(txstrPtr,Response,BUF_MAX_LEN);

}

/* Get the VCM configuration */
void GetLinkCoupling(convData_t *cmdVals, char *txstrPtr, int *status) 
{

    u32 Tile_Id;
    u32 Block_Id;

    Tile_Id                        = cmdVals[0].i;
    Block_Id                       = cmdVals[1].u;
    u32 Mode ;

    char Response[BUF_MAX_LEN] = {0};

    *status = XRFdc_GetLinkCoupling(&RFdcInst, Tile_Id, Block_Id, &Mode);

    if (enTermMode) {
       if (*status != SUCCESS) {
            printf("XRFdc_GetLinkCoupling() failed\n\r");
    } else {

        printf("    **********XRFdc_GetLinkCoupling***********\r\n");
        printf("    Mode:            0x%x\r\n", Mode);

      }
    }

    if (*status == SUCCESS) {
        sprintf(Response," %d %d %d ",Tile_Id,Block_Id,Mode);
        strncat(txstrPtr,Response,BUF_MAX_LEN);
    }
}

/* Get PLL configuration */

void GetPLLConfig (convData_t *cmdVals, char *txstrPtr, int *status) 
{

   u32 Type;
   u32 Tile_Id;
   char Response[BUF_MAX_LEN] = {0};

   XRFdc_PLL_Settings PLLSettings;

   Type                           = cmdVals[0].u;
   Tile_Id                        = cmdVals[1].u;

    XRFdc_GetPLLConfig(&RFdcInst, Type, Tile_Id, &PLLSettings);
   *status = SUCCESS;
   if (enTermMode) {
      if (*status != SUCCESS) {
         printf("\nXRFdc_GetPLLConfig failed\r\n");
      } else {

		 printf("    **********XRFdc_GetPLLConfig*********\r\n");
         printf("\n    Type: %d\n",Type);
         printf("    Tile_id: %d\n",Tile_Id);
         printf("    Enabled: %d\r\n", PLLSettings.Enabled);
         printf("    RefClkFreq:      %f MHz\r\n", PLLSettings.RefClkFreq);
         printf("    SampleRate:      %f MHz\r\n", PLLSettings.SampleRate);
         printf("    RefClkDivider:   %d\r\n", PLLSettings.RefClkDivider);
         printf("    FeedbackDivider: %d\r\n", PLLSettings.FeedbackDivider);
         printf("    OutputDivider  : %d\r\n",PLLSettings.OutputDivider);
         printf("    *********************************\r\n");
      }
   }

   if (*status == SUCCESS) {
	   sprintf(Response," %d %d %d %f %f %d %d %d ",Type,Tile_Id,PLLSettings.Enabled,\
			   PLLSettings.RefClkFreq,PLLSettings.SampleRate,PLLSettings.RefClkDivider,PLLSettings.FeedbackDivider,PLLSettings.OutputDivider);
   	   strncat (txstrPtr,Response,BUF_MAX_LEN);
   }

   return;
}

void MultiConverter_Init (convData_t *cmdVals, char *txstrPtr, int *status) 
{
	u32 Type;
    int PLL_Code = 0;
    int T1_Codes = 0;

	XRFdc_MultiConverter_Sync_Config DAC_Sync_Config;
    XRFdc_MultiConverter_Sync_Config ADC_Sync_Config;

	Type                               = cmdVals[0].u;
    if (Type == 0) {
		XRFdc_MultiConverter_Init (&ADC_Sync_Config, &PLL_Code , &T1_Codes);
	} else {
	    XRFdc_MultiConverter_Init (&DAC_Sync_Config, &PLL_Code , &T1_Codes);
	}
	return;
}

void MultiConverter_Sync (convData_t *cmdVals, char *txstrPtr, int *status) 
{
	u32 Type;
	char Response[BUF_MAX_LEN]={0};
	//0x3 - DAC, 0xF ADC
	u32 Tiles = cmdVals[2].u;
	XRFdc_MultiConverter_Sync_Config DAC_Sync_Config;
	XRFdc_MultiConverter_Sync_Config ADC_Sync_Config;
	XRFdc_MultiConverter_Sync_Config sync_config;

	Type = cmdVals[0].u;
	if (Type == 0) {
		XRFdc_MultiConverter_Init(&ADC_Sync_Config, 0 , 0);
	} else {
		XRFdc_MultiConverter_Init(&DAC_Sync_Config, 0 , 0);
	}

	if (Type == 0) {
		ADC_Sync_Config.Tiles = Tiles;
		ADC_Sync_Config.Target_Latency = cmdVals[1].i;
		*status = XRFdc_MultiConverter_Sync(&RFdcInst, Type ,&ADC_Sync_Config);
		sync_config = ADC_Sync_Config;
	    sprintf(Response," %d %d %d %d %d %d %d %d %d %d %d %d ", *status, sync_config.Target_Latency,
	    		  sync_config.Offset[0], sync_config.Offset[1], sync_config.Offset[2], sync_config.Offset[3],
				  sync_config.Latency[0], sync_config.Latency[1], sync_config.Latency[2], sync_config.Latency[3],
				  sync_config.Marker_Delay,sync_config.SysRef_Enable);

	} else {
		DAC_Sync_Config.Tiles = Tiles;
		DAC_Sync_Config.Target_Latency = cmdVals[1].i;
		*status = XRFdc_MultiConverter_Sync(&RFdcInst, Type ,&DAC_Sync_Config);
		sync_config = DAC_Sync_Config;
		sprintf(Response," %d %d %d %d %d %d %d %d ", *status, sync_config.Target_Latency,
			    		  sync_config.Offset[0], sync_config.Offset[1],
						  sync_config.Latency[0], sync_config.Latency[1],
						  sync_config.Marker_Delay,sync_config.SysRef_Enable);
	}

	/*sync_config.RefTile				= cmdVals[1].u;
	sync_config.Tiles				= cmdVals[2].u;
	sync_config.Target_Latency		= cmdVals[3].i;
	sync_config.Offset[0]			= cmdVals[4].i;
	sync_config.Offset[1]			= cmdVals[5].i;
	sync_config.Offset[2]			= cmdVals[6].i;
	sync_config.Offset[3]			= cmdVals[7].i;
	sync_config.Latency[0]			= cmdVals[8].i;
	sync_config.Latency[1]			= cmdVals[9].i;
	sync_config.Latency[2]			= cmdVals[10].i;
	sync_config.Latency[3]			= cmdVals[11].i;
	sync_config.Marker_Delay		= cmdVals[12].i;
	sync_config.SysRef_Enable		= cmdVals[13].i;
	XRFdc_MTS_DTC_Settings DTC_Set_PLL;
	XRFdc_MTS_DTC_Settings DTC_Set_T1;
    u32 RefTile;
	u32 IsPLL;
	int Target[4];
	int Scan_Mode;
	int DTC_Code[4];
	int Num_Windows[4];
	int Max_Gap[4];
	int Min_Gap[4];
	int Max_Overlap[4];
	*/

    strncat(txstrPtr, Response, BUF_MAX_LEN);
    
	return;
}

void MTS_Sysref_Config (convData_t *cmdVals, char *txstrPtr, int *status) 
{
	u32 enable;
	u32 dac_tiles;
	u32 adc_tiles;
	char Response[BUF_MAX_LEN]={0};
	XRFdc_MultiConverter_Sync_Config DAC_Sync_Config;
	XRFdc_MultiConverter_Sync_Config ADC_Sync_Config;

	enable = cmdVals[0].u;
	dac_tiles = cmdVals[1].u;
	adc_tiles = cmdVals[2].u;

	DAC_Sync_Config.Tiles = dac_tiles;
	ADC_Sync_Config.Tiles = adc_tiles;

	*status = XRFdc_MTS_Sysref_Config(&RFdcInst, &DAC_Sync_Config, &ADC_Sync_Config, enable);

	sprintf(Response,"  %d %d %d %d %d %d %d %d %d %d %d %d ", *status, DAC_Sync_Config.Target_Latency,
			DAC_Sync_Config.Offset[0], DAC_Sync_Config.Offset[1], DAC_Sync_Config.Offset[2], DAC_Sync_Config.Offset[3],
			DAC_Sync_Config.Latency[0], DAC_Sync_Config.Latency[1], DAC_Sync_Config.Latency[2], DAC_Sync_Config.Latency[3],
			DAC_Sync_Config.Marker_Delay,DAC_Sync_Config.SysRef_Enable);
    strncat(txstrPtr, Response, BUF_MAX_LEN);

	sprintf(Response,"  %d %d %d %d %d %d %d %d %d %d %d %d ", *status, ADC_Sync_Config.Target_Latency,
			ADC_Sync_Config.Offset[0], ADC_Sync_Config.Offset[1], ADC_Sync_Config.Offset[2],ADC_Sync_Config.Offset[3],
			ADC_Sync_Config.Latency[0], ADC_Sync_Config.Latency[1], ADC_Sync_Config.Latency[2], ADC_Sync_Config.Latency[3],
			ADC_Sync_Config.Marker_Delay,ADC_Sync_Config.SysRef_Enable);
	strncat(txstrPtr, Response, BUF_MAX_LEN);

	return;
}

void MTS_Setup(convData_t *cmdVals, char *txstrPtr, int *status)
{
	u32 type;
	u32 enable;
	int ret, i;
	int multitile_gpio;
	int *reset_gpio;
	int *localstart_gpio;
	int *mts_enable;
	char Response[BUF_MAX_LEN]={0};
	u32 max_dac, max_adc;

	if (info.design_type == DAC1_ADC1) {
		max_dac = 1;
		max_adc = 1;
	} else {
		max_dac = MAX_DAC;
		max_adc = MAX_ADC;
	}
	type = cmdVals[0].u;
	enable = cmdVals[1].u;

	if (type == ADC) {
		reset_gpio = adc_reset_gpio;
		localstart_gpio = adc_localstart_gpio;
		multitile_gpio = ADC_MULTITILE_CTL_GPIO;
		mts_enable = &info.mts_enable_adc;
	} else if (type == DAC) {
		reset_gpio = dac_reset_gpio;
		localstart_gpio = dac_localstart_gpio;
		multitile_gpio = DAC_MULTITILE_CTL_GPIO;
		mts_enable = &info.mts_enable_dac;
	} else {
		printf("unsupported type\n");
		ret = INVAL_ARGS;
		goto err;
	}
	if (info.design_type != MTS) {
		ret = INVAL_ARGS;
		MetalLoghandler_firmware(ret,"Design doesnot support MTS\n");
		goto err;
	}
	if (enable) {
		/* disable rfdc fifo for all DAC's/ADC's */
		ret = config_all_fifo(type, FIFO_DIS);
		if(ret != SUCCESS) {
			printf("Failed to disable DAC FIFO\n");
			ret = EN_FIFO_FAIL;
			goto err;
		}

		/* disable local start gpio for all dac/adc's*/
		for (i = 0; i < max_dac; i++) {
			ret = set_gpio(localstart_gpio[i], 0);
			if(ret) {
				printf("unable to assert reset gpio value\n");
				ret = GPIO_SET_FAIL;
				goto err;
			}
		}

		/* assert reset fifo gpio for all dac/adc's*/
		for (i = 0; i < max_dac; i++) {
			ret = set_gpio(reset_gpio[i], 1);
			if(ret) {
				printf("unable to assert reset gpio value\n");
				ret = GPIO_SET_FAIL;
				goto err;
			}
		}
		/* configure clocks */
		ret = set_gpio(multitile_gpio, 1);
		if(ret) {
			printf("unable to set multitile ctrl gpio value\n");
			ret = GPIO_SET_FAIL;
			goto err;
		}

		/* De-Assert reset FIFO GPIO for all DAC/ADC's*/
		for (i = 0; i < max_dac; i++) {
			ret = set_gpio(reset_gpio[i], 0);
			if(ret) {
				printf("Unable to de-assert reset GPIO value\n");
				ret = GPIO_SET_FAIL;
				goto err;
			}
		}
		/* enable rfdc fifo for all DAC's/ADC's */
		ret = config_all_fifo(type, FIFO_EN);
		if(ret != SUCCESS) {
			printf("Failed to disable DAC FIFO\n");
			ret = EN_FIFO_FAIL;
			goto err;
		}
		(*mts_enable) = 1;
	} else {
		(*mts_enable) = 0;
		/* configure clocks */
		ret = set_gpio(multitile_gpio, 0);
		if(ret) {
			printf("unable to reset multitile ctrl gpio value\n");
			ret = GPIO_SET_FAIL;
			goto err;
		}
		/* disable local start gpio for all dac/adc's*/
		for (i = 0; i < max_dac; i++) {
			ret = set_gpio(localstart_gpio[i], 0);
			if(ret) {
				printf("unable to assert reset gpio value\n");
				ret = GPIO_SET_FAIL;
				goto err;
			}
		}
		/* disable rfdc fifo for all DAC's/ADC's */
		ret = config_all_fifo(type, FIFO_DIS);
		if(ret != SUCCESS) {
			printf("Failed to disable DAC FIFO\n");
			ret = EN_FIFO_FAIL;
			goto err;
		}
	}

	*status = SUCCESS;
	return;
err:
	if(enTermMode) {
		printf("cmd = MTS_Setup\n"
				"type = %d\n",
				"enable = %d\n\n", type, enable);
	}

	/* Append Error code */
	strncat(txstrPtr,Response,BUF_MAX_LEN);
	*status = FAIL;
}



void GetCalFreeze(convData_t *cmdVals, char *txstrPtr, int *status) {
	int Tile_Id;
	u32 Block_Id;
	char Response[BUF_MAX_LEN]={0};
	u32 Val = 0;
	u32 Enable = 0;
	int fd;
	void *vaddr;
	void *base;

	Tile_Id = cmdVals[0].i;
	Block_Id = cmdVals[1].u;
	/* open memory file */
	fd = open("/dev/mem", O_RDWR | O_NDELAY);
	if (fd < 0) {
		printf("file /dev/mem open failed\n");
		*status = FAIL;
		return;
	}
	/* map size of memory */
	base =  mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, (CAL_FROZEN & ~MAP_MASK));
	if (base == MAP_FAILED) {
		printf("Error mmapping the file\n");
		perror("map");
		*status = FAIL;
		close(fd);
		return;
	}
	vaddr = base + (CAL_FROZEN & MAP_MASK);

	Val = *(u32*) (vaddr);
	Enable = (Val >> (2*Tile_Id + Block_Id)) & 0x01;
	*status = SUCCESS;
	sprintf(Response," %d %d %d ", Tile_Id, Block_Id, Enable);
	strncat(txstrPtr,Response,BUF_MAX_LEN);
	if(munmap(base, MAP_SIZE) == -1)
		printf("unmap failed\n");
	return;
}

void SetCalFreeze(convData_t *cmdVals, char *txstrPtr, int *status) {
	int Tile_Id;
	u32 Block_Id;
	u32 Enable;
	u32 Val = 0;
	u32 Mask;
	int fd;
	void *vaddr;
	void *base;

	Tile_Id = cmdVals[0].i;
	Block_Id = cmdVals[1].u;
	Enable = cmdVals[2].u;

	Mask = 0x1 << (2*Tile_Id + Block_Id);

	/* open memory file */
	fd = open("/dev/mem", O_RDWR | O_NDELAY);
	if (fd < 0) {
		printf("file /dev/mem open failed\n");
		return;
	}
	/* map size of memory */
	base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, (CAL_FREEZE & ~MAP_MASK));
	if (base == MAP_FAILED) {
		perror("map");
		printf("Error mmapping the file\n");
		close(fd);
		return;
	}
	vaddr = base + (CAL_FREEZE & MAP_MASK);
	Val = *(u32*) (vaddr);
	Val &= ~Mask;
	Val |= Enable << (2*Tile_Id + Block_Id);
	*(u32*) vaddr = Val;
	if(munmap(base, MAP_SIZE) == -1)
		printf("unmap failed\n");
	return;
}
