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

#ifndef SRC_POWER_INTERFACE_
#define SRC_POWER_INTERFACE_

/***************************** Include Files *********************************/
#include "board.h"
#include "board.h"
#include "iic_interface.h"
#include "rfdc_functions_w.h"
#include "xrfdc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PMIC_PATH                                                              \
  "/sys/devices/platform/axi/ff020000.i2c/i2c-0/i2c-4/4-0045/voltage"
#define DAC_AVTT_SLV_PATH                                                      \
  "/sys/devices/platform/axi/ff020000.i2c/i2c-0/i2c-2/2-004a/hwmon/hwmon9/"   \
  "power1_input"
#define DAC_AVCCAUX_SLV_PATH                                                   \
  "/sys/devices/platform/axi/ff020000.i2c/i2c-0/i2c-2/2-004b/hwmon/hwmon10/"  \
  "power1_input"
#define DAC_AVCC_SLV_PATH                                                      \
  "/sys/devices/platform/axi/ff020000.i2c/i2c-0/i2c-2/2-004e/hwmon/hwmon13/"  \
  "power1_input"
#define ADC_AVCCAUX_SLV_PATH                                                   \
  "/sys/devices/platform/axi/ff020000.i2c/i2c-0/i2c-2/2-004d/hwmon/hwmon12/"  \
  "power1_input"
#define ADC_AVCC_SLV_PATH                                                      \
  "/sys/devices/platform/axi/ff020000.i2c/i2c-0/i2c-2/2-004c/hwmon/hwmon11/"  \
  "power1_input"

/************************** Constant Definitions *****************************/

enum current_value { current_20mA, current_32mA, CURRENT_MAX };

/***************** Macros (Inline Functions) Definitions *********************/
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
/****************************************************************************/
/**
*
* This function configures DAC output current.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SetDACPowerMode(convData_t *cmdVals, char *txstrPtr, int *status);

/****************************************************************************/
/**
*
* This function gets DAC power value from the driver.
*
* @param	structure contains input arguments sent from client
* @param	contains response string
* @param	contains execution status
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void GetDACPower(convData_t *cmdVals, char *txstrPtr, int *status);

#endif /* SRC_POWER_INTERFACE_ */
