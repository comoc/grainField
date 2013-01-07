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
 *  ImageSource.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/23.
 *
 */

#ifndef _IMAGE_SOURCE_H
#define _IMAGE_SOURCE_H

#include <climits>

#include "ofImage.h"
#include "ofVideoGrabber.h"
#include "ofVideoPlayer.h"

class ImageSource
{
public:
	ImageSource();
	~ImageSource();
	bool initialize(const char* path);
	void play();
	void stop();
	void update();
	int getWidth();
	int getHeight();
	int getBytesPerPixel();
	unsigned char* getPixels();
public:
	static const char* PATH_LIVE_CAMERA;
private:
	enum Type
	{
		TYPE_UNKNOWN,
		TYPE_IMAGE,
		TYPE_VIDEO,
		TYPE_LIVE_CAMERA,
	};
	enum Camera
	{
		CAMERA_WIDTH = 320,
		CAMERA_HEIGHT = 240,
	};
	char path[PATH_MAX];
	Type type;
	void* image;
};

#endif