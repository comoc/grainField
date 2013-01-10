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
 *  Server.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 11/01/15.
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