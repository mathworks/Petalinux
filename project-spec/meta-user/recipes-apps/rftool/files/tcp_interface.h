/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.  All rights reserved.
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
#ifndef TCP_INTERFACE_H
#define TCP_INTERFACE_H
/***************************** Include Files *********************************/
/************************** Constant Definitions *****************************/
#define BUF_MAX_LEN 16384
/**************************** Type Definitions *******************************/
enum sock_type {
	COMMAND = 1,
	DATA = 2
};

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
/****************************************************************************/
/**
*
* This function is used to initialise command socket.
*
* @param	None	
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int tcpServerInitialize();

/****************************************************************************/
/**
*
* This function is used to initialise data socket.
*
* @param	None	
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int DataServerInitialize(void);

/****************************************************************************/
/**
*
* This function is used to accept command socket connection.
*
* @param	None	
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
void acceptConnection(void);

/****************************************************************************/
/**
*
* This function is used to accept data socket connection.
*
* @param	None	
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
void acceptdataConnection(void);

/****************************************************************************/
/**
*
* This function is used to get "\n" terminates string from control command socket.
*
* @param	Command string
* @param	length of string
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int getString(char*,int len);

/****************************************************************************/
/**
*
* This function is used to get "\n" terminates string from data command socket.
*
* @param	Command string
* @param	length of string
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int getdataString(char*,int len);

/****************************************************************************/
/**
*
* This function is used to send the response through control command socket.
*
* @param	Response string
* @param	length of string
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int sendString(char * resp,int len);

/****************************************************************************/
/**
*
* This function is used to send the response through data command socket.
*
* @param	Response string
* @param	length of string
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int senddataString(char * resp,int len);

/****************************************************************************/
/**
*
* This function is used to get data from client through data socket.
*
* @param	data
* @param	length of data
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
unsigned int getSamples(unsigned char *buf, unsigned int len);

/****************************************************************************/
/**
*
* This function is used to send data to client through data socket.
*
* @param	data
* @param	length of data
*
* @return	SUCCESS/FAILURE.
*
* @note		None.
*
******************************************************************************/
int sendSamples(signed char * resp, int len);

/****************************************************************************/
/**
*
* This function is used to close the specified socket.
*
* @param	socket id
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void shutdown_sock(int sock_id);

#endif
