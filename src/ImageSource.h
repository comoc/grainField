/*
 *  ImageSource.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/23.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
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