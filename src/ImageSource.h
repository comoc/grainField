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