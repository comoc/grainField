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
