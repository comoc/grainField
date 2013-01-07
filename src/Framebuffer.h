/*
 *  Framebuffer.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/08/25.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

//#define FRAMEBUFFER_ENABLE_MRT
#define FRAMEBUFFER_ENABLE_DEPTH_TEXTURE

class Framebuffer
{
public:
	Framebuffer(GLsizei width, GLsizei height, bool hasDepth);
	~Framebuffer();
	void attach();
	void detach();
#ifdef FRAMEBUFFER_ENABLE_MRT
	GLuint getColorTextureName0();
	GLuint getColorTextureName1();
#else
	GLuint getColorTextureName();	
#endif

	GLuint getDepthTextureName();
	GLsizei getWidth();
	GLsizei getHeight();
private:
	void create(GLsizei width, GLsizei height, bool hasDepth);
	void destroy();
private:
	GLuint fbo_id;
#ifdef FRAMEBUFFER_ENABLE_MRT
	GLuint fbo_tex_rgb[2];
#else
	GLuint fbo_tex_rgb;
#endif

#ifdef FRAMEBUFFER_ENABLE_DEPTH_TEXTURE
	GLuint fbo_tex_depth;
#else
	GLuint fbo_rb_depth;	
#endif
	GLsizei width;
	GLsizei height;
};

#endif //_FRAMEBUFFER_H
