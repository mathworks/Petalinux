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
#ifndef SRC_COMMON_
#define SRC_COMMON_

#include "xrfdc.h"

#define SUCCESS 0
#define FAIL 1

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

/**************************** Type Definitions *******************************/
typedef union {
	int i;
	u32 u;
	u64 l;
	double d;
	char b[8];
} convData_t;

/****************************************************************************/
/**
*
* This function updates sysfs path with the requested value.
*
* @param	sysfs path
* @param	value to update in the path
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int write_to_file(char *path, unsigned int val);

/****************************************************************************/
/**
*
* This function reads the value from sysfs path
*
* @param	sysfs path
* @param	address location where the value to be returned
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int read_from_file(char *path, int *val);

#endif /* SRC_COMMON_ */
