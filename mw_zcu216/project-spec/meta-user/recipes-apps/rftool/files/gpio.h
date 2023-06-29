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
#ifndef SRC_GPIO_
#define SRC_GPIO_

#define GPIO_PATH "/sys/class/gpio/"
#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_REL "/sys/class/gpio/unexport"

/****************************************************************************/
/**
*
* This function releases the specified GPIO.
*
* @param	GPIO number
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int disable_gpio(int gpio);
/****************************************************************************/
/**
*
* This function exports the specified GPIO.
*
* @param	GPIO number
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int enable_gpio(int gpio);

/****************************************************************************/
/**
*
* This function configures GPIO as output.
*
* @param	GPIO number
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int config_gpio_op(int gpio);
/****************************************************************************/
/**
*
* This function configures GPIO as input.
*
* @param	GPIO number
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int config_gpio_in(int gpio);
/****************************************************************************/
/**
*
* This function sets the GPIO with the specified value.
*
* @param	GPIO number
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int set_gpio(int gpio, int value);
/****************************************************************************/
/**
*
* This function gets the GPIO with the specified value.
*
* @param	GPIO number, address of the value variable to be returned
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int get_gpio(int gpio, int *value);

#endif /* SRC_GPIO_ */
