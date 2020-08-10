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

/***************************** Include Files *********************************/

#include "common.h"
#include <stdio.h>

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
/************************** Function Definitions *****************************/
int write_to_file(char *path, unsigned int val)
{
	FILE *fp;
	int ret;

	fp = fopen(path, "wb");
	if (fp == NULL) {
		printf("Error opening file : %s\n", path);
		return FAIL;
	}

	ret = fprintf(fp, "%d\n", val);
	if (ret <= 0) {
		printf("Unable to set clock value\n");
		fclose(fp);
		return FAIL;
	}

	fclose(fp);

	return SUCCESS;
}
int read_from_file(char *path, int *val)
{
	FILE *fp;
	int ret;

	fp = fopen(path, "rb");
	if (NULL == fp) {
		printf("Error opening file : %s\n", path);
		return FAIL;
	}

	ret = fscanf(fp, "%d\n", val);
	if (ret <= 0) {
		printf("Unable to get value\n");
		fclose(fp);
		return FAIL;
	}

	fclose(fp);

	return SUCCESS;
}
