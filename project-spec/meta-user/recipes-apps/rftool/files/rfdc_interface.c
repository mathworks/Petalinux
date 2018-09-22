/******************************************************************************
*
* Copyright (C) 2017 Xilinx, Inc.  All rights reserved.
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

int rfdc_inst_init(u16 rfdc_id)
{
	XRFdc_Config *ConfigPtr;
	XRFdc *RFdcInstPtr = &RFdcInst;
	int Status;
	struct metal_device *device;
	struct metal_io_region *io;
	int ret = 0;
	struct metal_init_params init_param = { \
		.log_handler    = MetalLoghandler,  \
		.log_level      = METAL_LOG_INFO,   \
	};

	if (metal_init(&init_param)) {
		sendString("ERROR: metal_init METAL_LOG_INIT_FAILURE", 40);
		return XRFDC_FAILURE;
	}

	/* Initialize the RFdc driver. */
	ConfigPtr = XRFdc_LookupConfig(rfdc_id);
	Status = XRFdc_CfgInitialize(RFdcInstPtr, ConfigPtr);
	if (Status != XRFDC_SUCCESS) {
		sendString("ERROR: XRFdc_CfgInitialize RFDC_CFG_INIT_FAILURE", 48);
		return XRFDC_FAILURE;
	}

	ret = metal_device_open(BUS_NAME, RFDC_DEV_NAME, &device);
	if (ret) {
		sendString("ERROR: metal_device_open METAL_DEV_OPEN_FAILURE", 47);
		return XRFDC_FAILURE;
	}

	/* Map RFDC device IO region */
	io = metal_device_io_region(device, 0);
	if (!io) {
		sendString("ERROR: metal_device_io_region METAL_IO_REGION_FAILURE", 53);
		return XRFDC_FAILURE;
	}
	RFdcInstPtr->device = device;
	RFdcInstPtr->io = io;

    return XRFDC_SUCCESS;
}


