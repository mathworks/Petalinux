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

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern int enTermMode; /* Enable printing in terminal mode */
extern XRFdc RFdcInst;
/*
 *  Get local Mem Info bridge
 */

void LocalMemInfo (convData_t *cmdVals,char *txstrPtr, int *status) 
{
    int numTiles;/* value of parameter tiles set on the stimulus gen or data capture IPI */
    int numMem;	/* number of stim gen or data capture memory channels */
    int memSize;	/* stim gen or data capture memory channel size */
    int numWords;/* streaming interface number of words per clk cycle */
    u32 memBaseAddr; /* AXI Base address */
    int mem_enable;
    int mem_clksel;
	char Response[BUF_MAX_LEN]={0};/* hold command response to return to the host */

	if (cmdVals[0].i == 0) {
		numTiles = 4;
		memBaseAddr = 82000000;
		mem_enable = 2;
		mem_clksel = 0;
	} else  {
		numTiles = 2;
		memBaseAddr = 86000000;
		mem_enable = 128;
		mem_clksel = 1;
	}
	numMem = 8;
	memSize = (128 * 1024);
	numWords = 16;

   if (*status == XRFDC_SUCCESS) {
		sprintf(Response," %d %08X %d %d %d %d %d %d",cmdVals[0].i, memBaseAddr,
				numTiles, numMem*2, memSize,numWords, mem_enable, mem_clksel);
		strncat(txstrPtr,Response,BUF_MAX_LEN);
   }


}

/*
 *  Function is a bridge that calls localMemTriggerHw
 *
 */
void LocalMemTrigger (convData_t *cmdVals,char *txstrPtr, int *status) 
{
	u32 memBaseAddr;	/* AXI Base address */

	/* select either stim gen or data capture base address based on cmd argument from the host */
	memBaseAddr = STIMULUS_GEN_BASEADDR;
	if(cmdVals[0].i) {
		memBaseAddr = DATA_CAPTURE_BASEADDR;
	}

	*status = LocalMemTriggerHw(memBaseAddr, cmdVals[1].u, cmdVals[2].u, cmdVals[3].u);

	 if (enTermMode) {
		 if (*status != XRFDC_SUCCESS) {
			 printf("localMemTrigger() failed\n\r");
		 } else {
			    printf("    **********localMemTrigger()***********\r\n");
				printf("    Local mem type (input from host):   %d\r\n", cmdVals[0].i);
				printf("    Local mem clk sel:   %d\r\n", cmdVals[1].u);
				printf("    Local mem num samples:  %d\r\n", cmdVals[2].u);
				printf("    Local mem channel enabled:   %d\r\n", cmdVals[3].u);
	        }
	 }
}

/*
 * Sets the end-address (size) of the data to run,
 * then enables  the list of channels (bitmask) and runs
 * for either the Stim/Capture memorie
 */

int LocalMemTriggerHw(u32 mem, u8 clk_sel, u32 len, u32 mem_ids) 
{
#if 0
	u32 reg_addr,end_addr,i;
	int num_tiles = 0;	/* value of parameter tiles set on the stimulus gen or data capture IPI */
	int num_mem = 0;	/* number of stim gen or data capture memory channels */
	int mem_size = 0;	/* stim gen or data capture memory channel size */
	int num_words = 0;	/* streaming interface number of words per clk cycle */

	/* Check that it ndont violate the hardware MemInfo */
	GetMemInfoHw(mem, &num_tiles, &num_mem, &mem_size, &num_words);

	/* len must be a multiple of num_words */
	end_addr = (len / num_words) - 1;

	/* check if valid input */
	if((clk_sel > num_tiles -1) | (len > mem_size)) {
		if (enTermMode) {
		  printf("localMemHw() failed invalid argument\n");
		}
		return XRFDC_FAILURE;
	} else {
		for(i=0; i<16; i++) {
			if((mem_ids >> i) & 0x1) {
				reg_addr = mem + LMEM0_ENDADDR + (i<<2);
				lmem_wr32(RFdcInst.io, reg_addr, end_addr);
				if (enTermMode) {
					printf("Mem %d End: %d -> %08X\n", i, end_addr, reg_addr);
				}
			}
		}

		/* set clk_sel */
		reg_addr = mem + LMEM_CLKSEL;
		lmem_wr32(RFdcInst.io, reg_addr, clk_sel);

		if (enTermMode) {
			printf("Mem clk sel: %d -> %08X\n", clk_sel, reg_addr);
		}

		/* Enable memories based on mem_ids bit-mask (clear then set) */
		reg_addr = mem + LMEM_ENABLE;
		lmem_wr32(RFdcInst.io, reg_addr, 0);
		lmem_wr32(RFdcInst.io, reg_addr, mem_ids);
		if (enTermMode) {
			printf("Mem Enables: %d -> %08X\n", mem_ids, reg_addr);
		}

		/* Issue the trigger */
		reg_addr = mem + LMEM_TRIGGER;
		lmem_wr32(RFdcInst.io, reg_addr, 1);
		if (enTermMode) {
			printf("Mem Trigger: %d -> %08X\n", 1, reg_addr);
		}
		return XRFDC_SUCCESS;
	}
#endif
	return XRFDC_SUCCESS;
}

/*
 *  Get local Mem Info hw
 */
void GetMemInfoHw(u32 mem, int *num_tiles, int *num_mem, int *mem_size, int *num_words) 
{
	u32 data;

	data = lmem_rd32(RFdcInst.io, (mem + LMEM_INFO));

	if(num_words != NULL){ *num_words = (data)	&	0x03F;  }	/* 6 bit wide result (5:0) */
	if(mem_size != NULL) { *mem_size = (data >> 6) & 0x7FFFF; }	/* 19 bit wide result (24:6) */
	if(num_mem  != NULL) { *num_mem  = (data >> 25)& 0x07;    }	/*  3 bit wide result (27:25) */
	if(num_tiles!= NULL) { *num_tiles= (data >> 28)& 0x07;    }	/*  3 bit wide result (30:28) */
}
