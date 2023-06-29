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
#ifndef SRC_VERSION_
#define SRC_VERSION_

#define RFTOOL_VERSION "2020.2"
#define RFTOOL_INTERNAL_VERSION "2.4"

#ifdef XPS_BOARD_ZCU208
#ifdef XPS_BOARD_ES1
#define IDCODE 0x047FB093 /* ZCU208 Engineering Sample */
#else
#define IDCODE 0x147FB093 /* ZCU208 Production */
#endif
#else
#ifdef XPS_BOARD_ES1
#define IDCODE 0x047FE093 /* ZCU216 Engineering Sample */
#else
#define IDCODE 0x147FE093 /* ZCU216 Production */
#endif
#endif

#endif /* SRC_VERSION_ */
