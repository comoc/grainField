/*
 Copyright (c) 2013 Akihiro Komori
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */


/*
 *  Server.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 11/01/15.
 *
 */

#include "Server.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>

Server::Server()
: callbackFunc(0), callbackArg(0), port(0), isRunning(false)
{
}

Server::~Server()
{
	stop();
}

void Server::start(int port, void* (*func)(void*, void*, size_t), void* arg)
{
	if (isRunning)
		return;
	
	isRunning = true;
	
	callbackFunc = func;
	callbackArg = arg;	
	this->port = port;
	
	pthread_t worker;	
	if (pthread_create(&worker, NULL, acceptThread, (void *)this) != 0)
		isRunning = false;
	pthread_detach(worker);
}

void Server::stop()
{
	if (isRunning)
		isRunning = false;
}

void* Server::acceptThread(void* arg)
{
	Server* ths = (Server*)arg;
	
	if (!ths->isRunning)
		pthread_exit(NULL);
	
	int listen_fd;
	int conn_fd;
	struct sockaddr_in saddr;
	int opt = 1;
	char buf[1024];
	
	// create
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		pthread_exit(NULL);
	}
	
	// set reuse addr
	opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

	// bind
	bzero((char *)&saddr, sizeof(saddr));
	saddr.sin_family        = PF_INET;
	saddr.sin_addr.s_addr   = INADDR_ANY;
	saddr.sin_port          = htons(ths->port);
	if (bind(listen_fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
	{
		close(listen_fd);
		listen_fd = -1;
		pthread_exit(NULL);
	}
	
	// listen
	if (listen(listen_fd, 1) != 0)
	{
		close(listen_fd);
		listen_fd = -1;
		pthread_exit(NULL);
	}
	
	// accept
	struct sockaddr_in caddr;
	socklen_t len = sizeof(struct sockaddr_in);
	if ((conn_fd = accept(listen_fd, (struct sockaddr *)&caddr, &len)) < 0)
	{
		close(listen_fd);
		listen_fd = -1;
		pthread_exit(NULL);
	}
	
	// set non-blocking
	opt = 1;
	ioctl(listen_fd, FIONBIO, &opt);
	
	while (ths->isRunning)
	{
		int rsize = recv(conn_fd, buf, sizeof(buf), 0);
		if (rsize < 1)
		{
			if (errno == EAGAIN)
			{
				pthread_yield_np();
			}
			else
			{
				perror("recv");
				break;
			}
		}
		else
		{
			if (ths->callbackFunc != 0)
				ths->callbackFunc(ths->callbackArg, buf, rsize);
		}		
	}
	
	if (conn_fd >= 0)
	{
		close(conn_fd);
		conn_fd =-1;
	}
	if (listen_fd >= 0)
	{
		close(listen_fd);
		listen_fd = -1;
	}
	
	return NULL;
}
