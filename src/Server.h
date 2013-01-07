/*
 *  Server.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 11/01/15.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SERVER_H
#define _SERVER_H

#include <netinet/in.h>

class Server
{
public:
	Server();
	~Server();
	void start(int port, void* (*func)(void*, void*, size_t), void* arg);
	void stop();
private:
	static void* acceptThread(void* arg);
	void* (*callbackFunc)(void*, void*, size_t); // callback(void* arg, void* buffer, size_t length);
	void* callbackArg;
	int port;
	volatile bool isRunning;
};

#endif