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
#include "iic_interface.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/*
 * The API is a wrapper function used as a bridge with the command interface.
 * The function
 * write data to iic slave.
 */
void iic_write_data(convData_t *cmdVals, char *txstrPtr, int *status) {
  u32 iic_inst;
  u32 slave_addr, bytes, reg_off;
  u32 size, data;
  u32 tx_array[5] = {0};

  iic_inst = cmdVals[0].u;
  slave_addr = cmdVals[1].u;
  reg_off = cmdVals[2].u;
  size = cmdVals[3].u;
  data = cmdVals[4].u;
  *status = iic_device_write(iic_inst, slave_addr, reg_off, size, data);
}

/*
 * The API is a wrapper function used as a bridge with the command interface.
 * The function
 * read data from iic.
 */
void iic_read_data(convData_t *cmdVals, char *txstrPtr, int *status) {
  u32 slave_addr, iic_inst, bytes;

  iic_inst = cmdVals[0].b[0];

  slave_addr = cmdVals[1].b[0];
  bytes = cmdVals[2].b[0];
  printf("cmd = iic_read_data\n"
         "iic_inst = %lu\n"
         "slave_addr = %lu\n"
         "bytes = %lu\n\n",
         iic_inst, slave_addr, bytes);

  *status = SUCCESS;
}
