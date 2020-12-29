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
#include "tcp_interface.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

/***************** Macros (Inline Functions) Definitions *********************/
#define PORT 8081
#define DATA_PORT 8082

/************************** Variable Definitions *****************************/
int new_socket, server_fd, addrlen;
int new_data_socket, data_server_fd;

struct sockaddr_in address;
struct sockaddr_in data_address;

/*****************************   Functions  **********************************/
/* initialize data socket */
void DataServerInitialize()
{
	int opt = 1;

	addrlen = sizeof(data_address);

	/* Creating socket file descriptor */
	if ((data_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("opening data socket failed");
		exit(EXIT_FAILURE);
	}

	/* set socket options */
	if (setsockopt(data_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		       &opt, sizeof(opt))) {
		perror("setsockopt data");
		exit(EXIT_FAILURE);
	}

	data_address.sin_family = AF_INET;
	data_address.sin_addr.s_addr = INADDR_ANY;
	data_address.sin_port = htons(DATA_PORT);

	/* bind socket to port */
	if (bind(data_server_fd, (struct sockaddr *)&data_address,
		 sizeof(data_address)) < 0) {
		perror("data bind failed");
		exit(EXIT_FAILURE);
	}

	/* listen socket */
	if (listen(data_server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("starting data server...\n");
}

/* close socket */
void shutdown_sock(int sock_id)
{
	if (sock_id & COMMAND)
		close(new_socket);

	if (sock_id & DATA)
		close(new_data_socket);
}

/* Accept data connection */
void acceptdataConnection(void)
{
	if ((new_data_socket =
		     accept(data_server_fd, (struct sockaddr *)&data_address,
			    (socklen_t *)&addrlen)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	(void)fcntl(new_data_socket, F_SETFL, O_NONBLOCK);
}

/* initialize command socket */
void tcpServerInitialize(void)
{
	int opt = 1;
	int opt_ndelay = 1;
	int opt_quickack = 1;

	addrlen = sizeof(address);

	/* Creating socket file descriptor */
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("opening socket failed");
		exit(EXIT_FAILURE);
	}

	/* set socket options */
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
		       sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_ndelay,
		       sizeof(opt_ndelay))) {
		perror("setsockopt nodelay failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(server_fd, IPPROTO_TCP, TCP_QUICKACK,
		       (void *)&opt_quickack, sizeof(opt_quickack))) {
		perror("setsockopt quickack failed");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	/* bind socket with port */
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	/* Listen socket */
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("starting server...\n");
}

/* accept command socket */
void acceptConnection(void)
{
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
				 (socklen_t *)&addrlen)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
}

/*
 * helper function to get "\n" terminates string from
 * TCP socket
 */
int getdataString(char *buf, int len)
{
	int valread = 0;
	char val;
	int rbytes = 0;
	struct timeval tv;
	fd_set fdread;
	int selectStatus = 0;

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	FD_ZERO(&fdread);
	FD_SET(new_data_socket, &fdread);

	do {
		selectStatus =
			select(new_data_socket + 1, &fdread, NULL, NULL, &tv);
		if (selectStatus == 0)
			return valread;

		if (FD_ISSET(new_data_socket, &fdread)) {
			valread += read(new_data_socket, &val, 1);
			buf[rbytes++] = val;
			if (valread == 0) {
				break;
			}
		}
	} while ((val != '\n') && (valread < len) && (rbytes < len));

	return valread;
}

/*
 * helper function to get "\n" terminates string from
 * TCP socket
 */
int getString(char *buf, int len)
{
	int valread = 0;
	char val;
	unsigned int rbytes = 0;

	do {
		valread += read(new_socket, &val, 1);
		buf[rbytes++] = val;
		if (valread == 0) {
			break;
		}
	} while ((val != '\n') && (valread < len));

	return valread;
}

/*
 * helper function remove char from a string
 */
void removeChar(char *str, char charToRm)
{
	char *src, *dst;

	for (src = dst = str; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != charToRm)
			dst++;
	}
	*dst = '\0';
}

/* sendString */
int sendString(char *resp, int len)
{
	int ret;
	char p[] = { 0xd, 0xa };
	int cntChar = 0; /* cnt the number of char */
	char newChar = '|'; /* new char used to replace "\n" */
	int bufLen1; /* buffer length minus 1 */

	/* input buffer length minus 1 */
	bufLen1 = (strlen(resp) - 1);

	while (resp[cntChar] != '\0') {
		/* check for character '\n' and replace by '|' except for
		 the last character replace by '\r' */
		if (resp[cntChar] == '\n') {
			if (cntChar != bufLen1) {
				resp[cntChar] = newChar;
			} else {
				resp[cntChar] = '\r';
			}
		}
		cntChar++;
	}

	/* remove '\r' from the string */
	removeChar(resp, '\r');

	/* Send buffer */
	ret = send(new_socket, resp, len, 0);
	/* send "\r\n" */
	(void)send(new_socket, p, sizeof(p), 0);

	return ret;
}

/* sendString */
int senddataString(char *resp, int len)
{
	int ret;
	char p[] = { 0xd, 0xa };

	/* Send buffer */
	ret = send(new_data_socket, resp, len, 0);

	/* send "\r\n" */
	(void)send(new_data_socket, p, sizeof(p), 0);

	return ret;
}

/* non blocking read logic */
unsigned int getSamples(unsigned char *buf, unsigned int len)
{
	unsigned int valread = 0, ret;

	int flags = fcntl(new_data_socket, F_GETFL, 0);
	flags = flags & (~O_NONBLOCK);
	(void)fcntl(new_data_socket, F_SETFL, flags);

	do {
		ret = read(new_data_socket, buf + valread, len);
		if (ret <= 0) {
			break;
		}
		valread += ret;
		len -= ret;
	} while (len);

	(void)fcntl(new_data_socket, F_SETFL, flags | O_NONBLOCK);
	return valread;
}

/*
 * helper function to send data samples
 */
int sendSamples(signed char *resp, int len)
{
	int ret;
	char p[] = { 0xd, 0xa };

	int flags = fcntl(new_data_socket, F_GETFL, 0);
	flags = flags & (~O_NONBLOCK);
	(void)fcntl(new_data_socket, F_SETFL, flags);

	/* Send buffer */
	ret = send(new_data_socket, resp, len, 0);

	/* send "\r\n" */
	(void)send(new_data_socket, p, sizeof(p), 0);

	(void)fcntl(new_data_socket, F_SETFL, flags | O_NONBLOCK);

	return ret;
}
/*
 * function to display the ip address of the board
 */
void DisplayIpAddress(void)
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == fd) {
		perror("Error creating socket");
		return;
	}
	/* Get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;
	/* Get IP address attached to "eth0" */
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	/* display result */
	printf("IP Address of the board: %s\n",
	       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	return;
}
