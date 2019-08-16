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

#include "cmd_interface.h"
#include "error_interface.h"
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

int cmdParseArg(char *strPtr, int cmdIdx );
int data_cmdParseArg(char *strPtr, int cmdIdx );
/************************** Variable Definitions *****************************/
int enTermMode  = 1;

convData_t cmdArrgs[MAX_CMD_VALS];       /* array of argument of typedef  convData_t */
convData_t data_cmdArrgs[MAX_CMD_VALS];       /* array of argument of typedef  convData_t */

/* Buffer message from metal_log, The handler append new message to the buffer */
/* The buffer is cleared following a read log command */
char metalMsgBuf[BUF_MAX_LEN] = {0};

/* Increments whenever a new log message is appended and get cleared following a read metal log command */
int metalMsgIdx = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

CMDSTRUCT data_cmdtab[] = {
	{"WriteDataToMemory"   , "<Tile_id> <Block_id> <number_of_bytes> <interleaved_pair>","iuuu", *WriteDataToMemory},
	{"ReadDataFromMemory"   , "<Tile_id> <Block_id> <number_of_bytes> <interleaved_pair>","iuuu", *ReadDataFromMemory},
};

/*Cmd Interface */
CMDSTRUCT cmdtab[] = {
	{"SetMixerSettings"   , "<Type> <Tile_id> <Block_id> <Freq> <PhaseOffset> <EventSource> <FineMixerMode> <CoarseMixerFreq> CoarseMixMode <FineMixerScale> |-> SetMixerSettings"                                                       ,"uiudduuuuu", *SetMixerSettings },
	{"GetMixerSettings"   , "<Type> <Tile_id> <Block_id> |-> GetMixerSettings <Type> <Tile_id> <Block_id>  <Freq> <PhaseOffset> <EventSource> FineMixerMode <CoarseMixFreq> CoarseMixMode <FineMixerScale>"                                            ,"uiu"      , *GetMixerSettings },
	{"GetQMCSettings"     , "<Type> <Tile_id> <Block_id> |-> GetQMCSettings <Type> <Tile_id> <Block_id>  <GainCorrectionFactor> <PhaseCorrectionFactor> <EnablePhase> <EnableGain> <OffsetCorrectionFactor>,<EventSource>" ,"uiu"      , *GetQMCSettings },
	/* board settings */
	{"SetExtParentclk"    , "<Board_id> <Freq> |-> SetExtParentclk <Board_id> <Freq>","uu", *SetExtParentclk},
	{"iic_write"   	      , "<iic_inst> <slave_addr> <reg_offset> <size> <data>","uuuuu", *iic_write_data},
	{"iic_read"   		  , "<iic_inst> <slave_addr> <size>","uuu", *iic_read_data},
	{"SetExtPllClkRate"   , "<Board_id> <Pll_Src> <Freq> |-> SetExtPllClkRate <Board_id> <Pll_Src> <Freq>","uuu", *SetExtPllClkRate},
	{"SetDACPowerMode"   , "<Board_id> <Tile_id> <Block_id> <Output_Current> |-> SetDACPowerMode <Board_id> <Tile_id> <Block_id> <Output_Current>","uiuu", *SetDACPowerMode },
	{"GetDACPower"   , "<Board_id> <Tile_id> |-> GetDACPower <board_id> <tile_id> <DAC_AVTT> <DAC_AVCC_AUX> <DAC_AVCC> <ADC_AVCC_AUX> <ADC_AVCC>","ui", *GetDACPower},
	{"StartUp"            , "<Type> <Tile_id> |-> StartUp"                                                                                                                                                                 ,"ui"       , *StartUp},
	{"Shutdown"           , "<Type> <Tile_id> |-> Shutdown"                                                                                                                                                                ,"ui"       , *Shutdown},
	{"RfdcVersion"        , "<Void> |-> RfdcVersion revision"                                                                                                                                                              ,""         , *RfdcVersion},
	{"Version"            , "<Void> |-> Version revision"                                                                                                                                                                  ,""         , *Version},
	{"JtagIdcode"            , "<Void> | JtagIdcode-> Idcode"                                                                                                                                                                         ,""         , *JtagIdcode},
	{"GetIPStatus"        , "<Void> |->  GetIPStatus DACTileStatus ADCTileStatus State"                                                                              ,""        , *GetIPStatus},
	{"Reset"              , "<Type> <Tile_id> |-> Reset"                                                               ,"ui"         , *Reset},
	{"GetPLLConfig"      , "<Type> <Tile_id> |-> GetPLLConfig Enabled RefClkFreq SampleRate RefClkDivider FeedbackDivider OutputDivider                     ","uu" , *GetPLLConfig },
	{"GetLinkCoupling"      ,"<Tile_Id> <Block_Id> |-> GetLinkCoupling <Block_Id> <Mode> ","uu",*GetLinkCoupling},
	{"SetQMCSettings"     , "<Type> <Tile_id> <Block_id> <EnablePhase> <EnableGain> <GainCorrectionFactor> <PhaseCorrectionFactor> <OffsetCorrectionFactor> <EventSource>  |-> SetQMCSettings"                ,"uiuuuddiu"   , *SetQMCSettings },
	{"GetCoarseDelaySettings"  , "<Type> <Tile_id> <Block_id> |-> GetCoarseDelaySettings <Type> <Tile_id> <Block_id> <CoarseDelay> <EventSource>"  	,"uiu"   , *GetCoarseDelaySettings },
	{"SetCoarseDelaySettings"  , "<Type> <Tile_id> <Block_id> <CoarseDelay> <EventSource> |->  SetCoarseDelaySettings"  	,"uiuuu" , *SetCoarseDelaySettings },
	{"GetInterpolationFactor"  , "<Tile_id> <Block_id> |-> GetInterpolationFactor <Tile_id> <Block_id> <InterpolationFactor>","iu"    , *GetInterpolationFactor },
	{"SetInterpolationFactor"  , "<Tile_id> <Block_id> <InterpolationFactor> |-> SetInterpolationFactor "                ,"iuu"   , *SetInterpolationFactor },
	{"GetDecimationFactor"  , "<Tile_id> <Block_id> |-> GetDecimationFactor <Tile_id> <Block_id> <DecimationFactor>"                			,"iu"    , *GetDecimationFactor },
	{"SetDecimationFactor"  , "<Tile_id> <Block_id> DecimationFactor |-> SetDecimationFactor"                			,"iuu"   , *SetDecimationFactor },
	{"GetNyquistZone"  , " <Type> <Tile_id> <Block_id> |-> GetNyquistZone <Type> <Tile_id> <Block_id> <NyquistZone>"            ,"uiu"   , *GetNyquistZone },
	{"SetNyquistZone"  , "<Type> <Tile_id> <Block_id> <NyquistZone> |-> SetNyquistZone"                					,"uiuu"  , *SetNyquistZone },
	{"GetOutputCurr"  , "<Tile_id> <Block_id> |-> GetOutputCurr <Tile_id> <Block_id> <OutputCurr>"                     ,"iu"    , *GetOutputCurr },
	{"GetPLLLockStatus"  , "<Type> <Tile_id>  |-> GetPLLLockStatus <Type> <Tile_id> <LockStatus>"                							,"ui"    , *GetPLLLockStatus },
	{"GetClockSource"  , "<Type> <Tile_id>  |-> GetClockSource <Type> <Tile_id> <ClockSource>"                					            ,"ui"    , *GetClockSource },
	{"DynamicPLLConfig"  , "<Type> <Tile_id> <Source> <RefClkFreq> <SamplingRate> |-> DynamicPLLConfig <RefClkDivider> <FeedbackDivider> <OutputDivider>"                        ,"uiudd" , *DynamicPLLConfig },
	{"SetFabClkOutDiv"  , "<Type> <Tile_id> FabClkDiv |-> DynamicPLLConfig"                                              ,"uiu"   , *SetFabClkOutDiv },
	{"SetupFIFO"             , "<Type> <Tile_id> <enable> |-> SetupFIFO"                                                   ,"uiu"   , *SetupFIFO },
	{"GetFIFOStatus"  , "<Type> <Tile_id>  |-> GetFIFOStatus Type <Tile_id> enable"                                      ,"ui"    , *GetFIFOStatus },
	{"SetFabWrVldWords"  , "<Tile_id> <Block_id> FabricDataRate |-> SetFabWrVldWords"                               ,"iuu"   , *SetFabWrVldWords },
	{"GetFabWrVldWords"  , "<Type> <Tile_id> <Block_id>  |-> GetFabWrVldWords ype <Tile_id> Block_id FabricDataRate "      ,"uiu"   , *GetFabWrVldWords },
	{"SetFabRdVldWords"  , "<Tile_id> <Block_id> <FabricDataRate> |-> SetFabRdVldWords"                               ,"iuu"   , *SetFabRdVldWords },
	{"GetFabRdVldWords"  , "<Type> <Tile_id> Block_id  |-> GetFabRdVldWords FabricDataRate "                             ,"uiu"   , *GetFabRdVldWords },
	{"SetDecoderMode"  , "<Tile_id> <Block_id> <DecoderMode> |-> SetDecoderMode"                						    ,"iuu"   , *SetDecoderMode },
	{"GetDecoderMode"  , "<Tile_id> <Block_id>  |-> GetDecoderMode <Tile_id> <Block_id> <DecoderMode>"                							,"iu"    , *GetDecoderMode },
	{"ResetNCOPhase"  , "<Type> <Tile_id> <Block_Id> |-> ResetNCOPhase"                									,"uiu"   , *ResetNCOPhase },
	{"DumpRegs"  , "<Type> <Tile_id>  |-> DumpRegs"                														,"ui"    , *DumpRegs },
	{"UpdateEvent"  , "<Type> <Tile_id> <Block_id> <Event> |-> UpdateEvent"                						            ,"uiuu"   , *UpdateEvent },
	{"GetCalibrationMode"  , "<Tile_id> <Block_id> |-> GetCalibrationMode <Tile_id> <Block_id> CalibrationMode"                             ,"iu"    , *GetCalibrationMode },
	{"SetCalibrationMode"  , "<Tile_id> <Block_id> CalibrationMode |-> SetCalibrationMode "                           ,"iuu"   , *SetCalibrationMode },
	{"GetBlockStatus"  , "<Type> <Tile_id> <Block_id> |-> GetBlockStatus Type <Tile_id> <Block_id> SamplingFreq AnalogDataPathStatus DigitalDataPathStatus DataPathClocksStatus IsFIFOFlagsAsserted IsFIFOFlagsEnabled"  ,"uiu"    , *GetBlockStatus },
	{"GetThresholdSettings"  , "<Tile_id> <Block_id> |-> GetThresholdSettings <Tile_id> <Block_id> UpdateThreshold ThresholdMode[2] ThresholdAvgVal[2] ThresholdUnderVal[2] ThresholdOverVal[2]"                             ,"iu"    , *GetThresholdSettings },
	{"SetThresholdSettings"  , "<Tile_id> <Block_id>  <UpdateThreshold> <ThresholdMode[2]> <ThresholdAvgVal[2]> <ThresholdUnderVal[2]> <ThresholdOverVal[2]>  |-> SetThresholdSettings "                           ,"iuuuuuuuuuu"   , *SetThresholdSettings },
	{"RF_ReadReg32"       , "<Address_Offset> |-> RF_ReadReg32 Value"                             ,"u"    , *RF_ReadReg32 },
	{"RF_WriteReg32"      , "<Address_Offset> <Value> |-> RF_WriteReg32 "                           ,"uu"   , *RF_WriteReg32 },
	{"RF_ReadReg16"       , "<Address_Offset> |-> RF_ReadReg16 Value"                             ,"u"    , *RF_ReadReg16 },
	{"RF_WriteReg16"      , "<Address_Offset> <Value> |-> RF_WriteReg16 "                           ,"uu"   , *RF_WriteReg16 },
	{"RF_ReadReg8"        , "<Address_Offset> |-> RF_ReadReg8 <Value>"                              ,"u"    , *RF_ReadReg8 },
	{"RF_WriteReg8"  	   , "<Address_Offset> <Value> |-> RF_WriteReg8 "                            ,"uu"   , *RF_WriteReg8 },
	{"MultiBand"          ,"<Type> <Tile_Id> <DigitalDataPathMask> <DataType> <DataConverterMask> |-> MultiBand","uiuuu",*MultiBand},
	{"GetConnectedData"   ,"<Type> <Tile_Id> <Block_Id> |-> GetConnectedIData <Type> <Tile_Id> <Block_Id> <ConnectedIData> <ConnectedIData>","uiu",*GetConnectedData},
	{"GetInvSincFIR"      ,"<Tile_Id> <Block_Id> |-> GetInvSincFIR <Tile_Id> <Block_Id> <Enable>","iu",*GetInvSincFIR},
	{"SetInvSincFIR"      ,"<Tile_Id> <Block_Id> |-> SetInvSincFIR <Tile_Id> <Block_Id> <Enable>","iuu",*SetInvSincFIR},
	{"MultiConverter_Init"     , "<Type> |-> MultiConverter_Init "                           ,"u"   , *MultiConverter_Init },
	{"MultiConverter_Sync"     , "<Type> <Target_Latency> <Tiles> |-> MultiConverter_Sync status .....  "                           ,"uiu"   , *MultiConverter_Sync },
	{"MTS_Sysref_Config"       , "<enable> <dac_tiles> <adc_tiles> |-> MTS_Sysref_Config  status ....."                           ,"uuu"   , *MTS_Sysref_Config },
	{"MTS_Setup"               , "Type Enable  |-> MTS_Setup <text with variable length> "                                             ,"uu"   , *MTS_Setup },
	{"IntrClr"      ,"<Type> <Tile_Id> <Block_Id> <Interrupt Mask> |-> IntrClr <Tile_Id> <Block_Id>","uiuu",*IntrClr},
	{"GetIntrStatus"      ,"<Type> <Tile_Id> <Block_Id> |-> GetIntrStatus <Tile_Id> <Block_Id> <Interupt Status> ","uiu",*GetIntrStatus},
	{"TermMode"           , "<Enable> |-> TermMode"                                         ,"i"        ,*TermMode},
	{"?"                  , "<Void> |-> DoHelp <This Menu>"                                 ,""         , *DoHelp},
	{"help"               , "<Void> |-> DoHelp <This Menu>"                                 ,""         , *DoHelp},
	{"GetLog"             , "<Void> |-> GetLog <metal_log_string"                           ,""         , *GetLog},
	{"disconnect"   , "<void> |-> disconnect ","", *disconnect},
	{"GUI_Title"   , "<void> |-> GUI_Title ","", *GUI_Title},
	{"GetExtPllFreqList"   , "<Board_Id> <PLL_Src> |-> GetExtPllFreqList <Board_Id> <PLL_Src>","uu", *GetExtPllFreqList},
	{"GetExtPllConfig"   , "<Board_Id> <PLL_Src> |-> GetExtPllConfig <Board_Id> <PLL_Src>","uu", *GetExtPllConfig},
	{"LocalMemInfo"            , "<stim_cap> |-> LocalMemInfo <stim_cap> <AXI base address> <NumTiles> <NumMems> <MemChSize> <mem_enable> <mem_clksel>"   ,"u"        , *LocalMemInfo},
	{"LocalMemTrigger"         , "<stim_cap> <clk_sel> <numsamples> <channels> |-> LocalMemTrigger"                                                       ,"uuuu"      , *LocalMemTrigger},
	{"SetCalFreeze"            , "<Tile_id> <Block_id> <Enable> |-> SetCalFreeze ", "iuu", *SetCalFreeze},
	{"GetCalFreeze"            , "<Tile_id> <Block_id>  |-> GetCalFreeze Tile_id Block_id Enable", "iu", *GetCalFreeze},
	{"SetMemtype"              , "<mem_type>  |-> SetMemtype memtype", "u", *SetMemtype},
	{"GetMemtype"              , "<void>  |-> GetMemtype memtype", "", *GetMemtype},
	{"SetFabSSR"     , "<Type> <Enable> |-> SetFabSSR ", "uu", *SetFabSSR},
	{"GetFabSSR"     , "<Type> |-> GetFabSSR ", "u", *GetFabSSR},
	{"LocalMemAddr"            , "", "uiu"   , *LocalMemAddr},
	{"SetLocalMemSample"       , "","uiuu"   , *SetLocalMemSample},
};


/************************** Function Definitions *****************************/

int data_cmdParse(char *strPtr, char *txstrPtr) {

	char token[] = " ,\t\n\r";  /* character used to split the string into substrings */
	char *cmdPtr;               /* Pointer to the command */
	int cmdIdx;                 /* index of the received command in the array cmdtab */
	int foundCmd;               /* Set if a defined command is found */
	char strLocal[BUF_MAX_LEN]; /* local copy of input string */
	int exeStatus;              /* status returned by the function that has been executed */
	int validNumArg;            /* set to 1 if the the number of arguents received match the expected number of argument for current command */
	void (*func)();             /* pointer to the  function to execute */

	/* init */
	validNumArg = 0;            /* initialize valid number of argument to zero */
	cmdIdx = 0;                 /* initialize command index to 0 */
	foundCmd = 0;               /* initialize command index to 0 */

	/* make local copy of strPtr */
	strcpy(strLocal,strPtr);

	/* command extracted from the input string (local copy of the string is used to preserve the input string strPtr) */
	cmdPtr     = strtok(strLocal, token );

	/* check if the received command is valid and the number of arguments received is as expected by the command */
	if( cmdPtr != NULL ) {
		/* update txstrPtr with the received command name */
		strcat(txstrPtr, cmdPtr);

		for(int i=0; i < 2; i++) {
			/* check for valid command name */
			if( !strcasecmp(cmdPtr, data_cmdtab[i].cmdString) ) {
				func   = data_cmdtab[i].cmdFunc;
				foundCmd = 1;  /* found a defined command */
				cmdIdx = i;    /* record command id, index location of the received command in cmdtab */
			}
		}

		/* check if a command is found */
		if(foundCmd) {
			/* parse arguments and check for valid number of arguments */
			validNumArg = data_cmdParseArg(strPtr,cmdIdx);

			/* check is a valid number of arguments is received */
			if (validNumArg) {
				/* execute command */
				(*func)(data_cmdArrgs,txstrPtr, &exeStatus);
				/* return status of command execution */
				if (exeStatus != SUCCESS) {
					return ERROR_EXECUTE;
				} else {
					return SUCCESS;
				}
			} else {
				/* return invalid number of arguments as a status */
				return ERROR_NUM_ARGS;
			}
		} else {
			/* command not found */
			return ERROR_CMD_UNDEF;
		}
	} else {
		return SUCCESS;
	}
}

/*
 *  Parse command arguments and detect if the number of argument is as as expected by the received command
 */

int data_cmdParseArg(char *strPtr, int cmdIdx ) {

	char argType[MAX_CMD_VALS];                /* local copy of cmdtab[cmdIdx].argType */
	char token[] = " ,\t\n\r";                 /* token used by strtok to split string */
	strcpy(argType,data_cmdtab[cmdIdx].argType);    /* local copy size is ensured to match within the parent function cmdParse */

	/* Remove command name from strPtr and preserve arguments and \r\n or \n */
	strPtr = strtok(strPtr,token);
	strPtr = strtok( NULL,"");

	/* Parse the arguments based on the expected data type from this functions 'type_string' */
	for(int idx=0; idx< strlen(argType); idx++) {
		/* Next characters following the last argument is expected to be \r\n or \n */
		if (strPtr == NULL ||((*strPtr == '\r')||(*strPtr == '\n'))) {
			/* 0 indicates invalid number of arguments to the parent function */
			return 0;
		} else {
			if(argType[idx]=='i')  data_cmdArrgs[idx].i = strtol(strPtr, &strPtr, 0);
			if(argType[idx]=='u')  data_cmdArrgs[idx].u = strtoul(strPtr, &strPtr,0);
			if(argType[idx]=='d')  data_cmdArrgs[idx].d = strtod(strPtr, &strPtr);
		}
	}
	/* Next characters following the last argument is expected to be \r\n or \n */
	if ((*strPtr == '\r')|| (*strPtr == '\n')) {
		/* 1 indicates a valid number of arguments to the parent function */
		return 1;
	} else {
		/* 0 indicates an invalid number of arguments */
		return 0;
	}
}

/*
 *  Parse the Command, the function take a string containing a command followed by N arguments
 *  return XST_SUCCESS if empty tab is sent as command
 *  return XST_SUCCESS if a valid command with a valid number of arguments is executed with success
 *    Results is stored in txstrPtr = "cmd val1 valn\r\n"  if no value is expected it return txstrPtr = "cmd\r\n";
 *  return ERROR_CMD_UNDEF if an undefined command is received
 *  return ERROR_NUM_ARGS if a command received has the wrong number of arguments
 *    *txstrPtr = "cmd\n\r"
 *  return ERROR_EXECUTE if the command executed return XST_DAILURE
 *    *txstrPtr = "cmd\n\r"
 *
 */
int cmdParse(char *strPtr, char *txstrPtr) 
{
	char token[] = " ,\t\n\r";  /* character used to split the string into substrings */
	char *cmdPtr;               /* Pointer to the command */
	int cmdIdx;                 /* index of the received command in the array cmdtab */
	int foundCmd;               /* Set if a defined command is found */
	char strLocal[BUF_MAX_LEN]; /* local copy of input string */
	int exeStatus;              /* status returned by the function that has been executed */
	int validNumArg;            /* set to 1 if the the number of arguents received match the expected number of argument for current command */
	void (*func)();             /* pointer to the  function to execute */

	validNumArg = 0;            /* initialize valid number of argument to zero */
	cmdIdx = 0;                 /* initialize command index to 0 */
	foundCmd = 0;               /* initialize command index to 0 */

	/* make local copy of strPtr */
	strcpy(strLocal,strPtr);

	/* command extracted from the input string (local copy of the string is used to preserve the input string strPtr) */
	cmdPtr     = strtok(strLocal, token );


	/* check if the received command is valid and the number of arguments received is as expected by the command */
	if( cmdPtr != NULL ) {
		/* update txstrPtr with the received command name */
		strcat(txstrPtr, cmdPtr);

		for(int i=0; *cmdtab[i].cmdString; i++) {
			/* check for valid command name */
			if( !strcasecmp(cmdPtr, cmdtab[i].cmdString) ) {
				func   = cmdtab[i].cmdFunc;
				foundCmd = 1;  /* found a defined command */
				cmdIdx = i;    /* record command id, index location of the received command in cmdtab */
			}
		}
		/* check if a command is found */
		if(foundCmd) {
			/* parse arguments and check for valid number of arguments */
			validNumArg = cmdParseArg(strPtr,cmdIdx);

			/* check is a valid number of arguments is received */
			if (validNumArg) {
				/* execute command */
				(*func)(cmdArrgs,txstrPtr, &exeStatus);
				/* return status of command execution */
				if (exeStatus != SUCCESS) {
					return ERROR_EXECUTE;
				} else {
					return SUCCESS;
				}
			} else {
				/* return invalid number of arguments as a status */
				return ERROR_NUM_ARGS;
			}
		} else {
			/* command not found */
			return ERROR_CMD_UNDEF;
		}
	} else {
		return SUCCESS;
	}
}

/*
 *  Parse command arguments and detect if the number of argument is as as expected by the received command
 */

int cmdParseArg(char *strPtr, int cmdIdx ) {

	char argType[MAX_CMD_VALS];                /* local copy of cmdtab[cmdIdx].argType */
	char token[] = " ,\t\n\r";                 /* token used by strtok to split string */
	strcpy(argType,cmdtab[cmdIdx].argType);    /* local copy size is ensured to match within the parent function cmdParse */

	/* Remove command name from strPtr and preserve arguments and \r\n or \n */
	strPtr = strtok(strPtr,token);
	strPtr = strtok( NULL,"");

	/* Parse the arguments based on the expected data type from this functions 'type_string' */
	for(int idx=0; idx< strlen(argType); idx++) {
		/* Next characters following the last argument is expected to be \r\n or \n */
		if (strPtr == NULL ||((*strPtr == '\r')||(*strPtr == '\n'))) {
			/* 0 indicates invalid number of arguments to the parent function */
			return 0;
		} else {
			if(argType[idx]=='i')  cmdArrgs[idx].i = strtol(strPtr, &strPtr, 0);
			if(argType[idx]=='u')  cmdArrgs[idx].u = strtoul(strPtr, &strPtr,0);
			if(argType[idx]=='d')  cmdArrgs[idx].d = strtod(strPtr, &strPtr);
			if(argType[idx]=='l')  cmdArrgs[idx].l = strtoull(strPtr, &strPtr,0);
		}
	}
	/* Next characters following the last argument is expected to be \r\n or \n */
	if ((*strPtr == '\r')|| (*strPtr == '\n')) {
		/* 1 indicates a valid number of arguments to the parent function */
		return 1;
	} else {
		/* 0 indicates an invalid number of arguments */
		return 0;
	}
}

/*
 * Help function for command interface
 */
void DoHelp(convData_t *cmdVals,char *txstrPtr, int *status)
{
	int i;

	if(enTermMode) {
		printf("Help on commands:\r\n");
		for(i=0; *cmdtab[i].cmdString; i++) {
			printf("    %s %s\r\n", cmdtab[i].cmdString, cmdtab[i].cmdHelp);
		}
	}
	*status = SUCCESS;
}

/*
 * Enable terminal mode. additional debug print are enabled and sent back to the console
 */

void TermMode(convData_t *cmdVals,char *txstrPtr, int *status)
{
	enTermMode = cmdVals[0].i;
	*status = SUCCESS;
}

/*
 * Return the xrfdc revision
 */
void Version (convData_t *cmdVals,char *txstrPtr, int *status) 
{
	char Response[BUF_MAX_LEN]={0};

	sprintf(Response," 1.3");
	strncat (txstrPtr,Response,BUF_MAX_LEN);
	*status = SUCCESS;
}

void GUI_Title (convData_t *cmdVals,char *txstrPtr, int *status) 
{
	char Response[BUF_MAX_LEN]={0};

	sprintf(Response," (TRD v1.3) ");
	strncat (txstrPtr,Response,BUF_MAX_LEN);

	*status = SUCCESS;
}

/*
 * libmetal logger handler
 * The function append the log message from the metal_log to a global buffer
 * The global buffer is then cleared by a dedicated command following a read log command
 */

void MetalLoghandler(enum metal_log_level level, const char *format, ...)
{
	char msgLocal[BUF_MAX_LEN/64];

	va_list args;
	static const char *level_strs[] = {
		"metal: emergency: ",
		"metal: alert:     ",
		"metal: critical:  ",
			"metal: error:     ",
			"metal: warning:   ",
			"metal: notice:    ",
			"metal: info:      ",
			"metal: debug:     ",
		};
		pthread_mutex_lock(&mutex);
		va_start(args, format);
		vsnprintf(msgLocal, sizeof(msgLocal), format, args);
		va_end(args);

		if (level <= METAL_LOG_EMERGENCY || level > METAL_LOG_DEBUG)
			level = METAL_LOG_EMERGENCY;

		/* buffer msgLocal into metalMsgBuf */
		strncat(metalMsgBuf,level_strs[level],(BUF_MAX_LEN - 1));
		strncat(metalMsgBuf,msgLocal,(BUF_MAX_LEN - 1));
		/* Increment idx by 1 */
		metalMsgIdx++;
		pthread_mutex_unlock(&mutex);
}

/*
 * libmetal logger handler
 * The function append the log message from the metal_log to a global buffer
 * The global buffer is then cleared by a dedicated command following a read log command
 */

void MetalLoghandler_firmware(int log_level, const char *format, ...)
{
	char msgLocal[BUF_MAX_LEN/64];
	int level = (log_level < 0) ? -(log_level): (log_level);

	va_list args;
	static const char *level_strs[] = {
		"success ",
		"fail: ",
		"invalid: args ",
		"fail: mem disable ",
		"fail: mem enable ",
		"fail: mem init ",
		"fail: mem init ",
		"fail: enable fifo ",
		"fail: reset fifio ",
		"fail: DMA RX ",
		"fail: DMA TX ",
		"fail: disable fifo ",
		"fail: enable IL Config ",
		"fail: send sample ",
		"fail: receive sample ",
		"fail: GPIO setting ",
		"fail: MTS setting ",
		};

		pthread_mutex_lock(&mutex);
		va_start(args, format);
		vsnprintf(msgLocal, sizeof(msgLocal), format, args);
		va_end(args);

		/* buffer msgLocal into metalMsgBuf */
		strncat(metalMsgBuf,level_strs[level],(BUF_MAX_LEN - 1));
		strncat(metalMsgBuf,msgLocal,(BUF_MAX_LEN - 1));
		/* Increment idx by 1 */
		metalMsgIdx++;
		pthread_mutex_unlock(&mutex);
}



/*
 * The function returns the metal log global buffer (metalMsgBuf) to the host
 * once a copy of the log message is returned the function clears the metalMsgBuf
 * and reset metalMsgIdx to 0
 */
void GetLog(convData_t *cmdVals,char *txstrPtr, int *status) {
	/* append metalMsgBuf to txstrPtr */
	strncat (txstrPtr," ",(BUF_MAX_LEN - 1));
	strncat (txstrPtr,metalMsgBuf,(BUF_MAX_LEN - 1));

	/* clear metalMsgBuf and metalMsgIdx */
	memset(metalMsgBuf, 0, sizeof(metalMsgBuf));
	metalMsgIdx = 0;

	*status = SUCCESS;
}

void SetFabSSR(convData_t *cmdVals, char *txstrPtr, int *status) {
	u32 Type;
	u32 Enable;
	u32 gpio;
	u32 *val;
	int ret;

	Type = cmdVals[0].u;
	Enable = cmdVals[1].u;

	if (info.design_type != DAC1_ADC1) {
		MetalLoghandler_firmware(INVAL_ARGS,"SSR commands are only supported by SSR IP design\n");
		*status = FAIL;
		goto err;
    }

	if (Type == ADC) {
		gpio = ADC_SSR_CTL_GPIO;
		val = &info.adc_ssrgpio_val;
	} else {
		gpio = DAC_SSR_CTL_GPIO;
		val = &info.dac_ssrgpio_val;
	}

	ret = set_gpio(gpio, Enable);
	if(ret) {
		MetalLoghandler_firmware(ret,"Unable to set select GPIO value\n");
		goto err;
	}
	*status = SUCCESS;
	(*val) = Enable;
	return;
err:
	if(enTermMode) {
		printf("cmd = SetFabSSR\n"
				"type = %d\n",
				"enable = %d\n\n", Type, Enable);
	}

	*status = FAIL;
}

void GetFabSSR(convData_t *cmdVals, char *txstrPtr, int *status) {
	u32 Type;
	u32 Enable;
	char Response[BUF_MAX_LEN]={0};

	Type = cmdVals[0].u;

	if (info.design_type != DAC1_ADC1) {
		MetalLoghandler_firmware(INVAL_ARGS,"SSR commands are only supported by SSR IP design\n");
		*status = FAIL;
		return;
	}

	if (Type == ADC)
		Enable =  info.adc_ssrgpio_val;
	else
		Enable = info.dac_ssrgpio_val;

	*status = SUCCESS;
	sprintf(Response," %d %d ",Type, Enable);
	strncat(txstrPtr,Response,BUF_MAX_LEN);
}
