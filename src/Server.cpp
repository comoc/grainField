/*
 
 Copyright (c) 2013, Akihiro Komori
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the developer nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY DAMIAN STEWART ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL DAMIAN STEWART BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
