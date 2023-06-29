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
#include <fcntl.h>
#include <stdio.h>
/**************************** Type Definitions *******************************/
/***************** Macros (Inline Functions) Definitions *********************/
/************************** Variable Definitions *****************************/
/************************** Function Definitions *****************************/
int config_axi_switch(unsigned int phy_addr, unsigned int offset, int enable,
		      unsigned int slave)
{
	int fd;
	void *base_addr;
	void *vaddr;

	fd = open("/dev/mem", O_RDWR | O_NDELAY);
	if (fd < 0) {
		printf("%s: /dev/mem open failed\n", __func__);
		goto err;
	}
	base_addr = mmap(NULL, (MAP_SIZE * 2), PROT_READ | PROT_WRITE,
			 MAP_SHARED, fd, ((phy_addr) & ~MAP_MASK));
	if (base_addr == MAP_FAILED) {
		perror("map");
		printf("%s: Error mmapping the AXI SWITCH:%x \n", __func__,
		       phy_addr);
		close(fd);
		goto err;
	}
	vaddr = base_addr + (phy_addr & MAP_MASK);
	if (enable)
		*(u32 *)(vaddr + offset) = slave;
	else
		*(u32 *)(vaddr + offset) = 0x80000000;

	*(u32 *)(vaddr) = 0x2;

	if (munmap(base_addr, MAP_SIZE) == -1)
		printf("unmap failed\n");

	close(fd);
	return SUCCESS;
err:
	return FAIL;
}
