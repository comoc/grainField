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
 *  Framebuffer.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/08/25.
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
