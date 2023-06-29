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
#ifndef SRC_POWER_INTERFACE_
#define SRC_POWER_INTERFACE_

/***************************** Include Files *********************************/
#include "common.h"

#define PMIC_PATH                                                              \
	"/sys/devices/platform/amba/ff020000.i2c/i2c-0/i2c-5/5-0045/voltage"
#define DAC_AVTT_SLV_PATH                                                      \
	"/sys/devices/platform/amba/ff020000.i2c/i2c-0/i2c-3/3-004a/hwmon/"    \
	"hwmon9/power1_input"
#define DAC_AVCCAUX_SLV_PATH                                                   \
	"/sys/devices/platform/amba/ff020000.i2c/i2c-0/i2c-3/3-004b/hwmon/"    \
	"hwmon10/power1_input"
#define DAC_AVCC_SLV_PATH                                                      \
	"/sys/devices/platform/amba/ff020000.i2c/i2c-0/i2c-3/3-004e/hwmon/"    \
	"hwmon13/power1_input"
#define ADC_AVCCAUX_SLV_PATH                                                   \
	"/sys/devices/platform/amba/ff020000.i2c/i2c-0/i2c-3/3-004d/hwmon/"    \
	"hwmon12/power1_input"
#define ADC_AVCC_SLV_PATH                                                      \
	"/sys/devices/platform/amba/ff020000.i2c/i2c-0/i2c-3/3-004c/hwmon/"    \
	"hwmon11/power1_input"

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
