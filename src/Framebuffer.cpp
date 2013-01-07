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
 *  Framebuffer.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/08/25.
 *
 */

#include "Framebuffer.h"
#include <iostream>

using namespace std;

namespace {
	bool checkFramebufferStatus()
	{
        // check FBO status
        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
        switch(status)
        {
			case GL_FRAMEBUFFER_COMPLETE:
                std::cout << "Framebuffer complete." << std::endl;
                return true;
				
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
                return false;
				
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
                return false;
				
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
                return false;
				
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
                return false;
				
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
                return false;
				
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
                return false;
				
			case GL_FRAMEBUFFER_UNSUPPORTED:
                std::cout << "[ERROR] Unsupported by FBO implementation." << std::endl;
                return false;
				
			default:
                std::cout << "[ERROR] Unknow error." << std::endl;
                return false;
        }
	}	
}

Framebuffer::Framebuffer(GLsizei width, GLsizei height, bool hasDepth)
: width(0)
, height(0)
{
	fbo_id = 0;
#ifdef FRAMEBUFFER_ENABLE_MRT
	fbo_tex_rgb[0] = 0;
	fbo_tex_rgb[1] = 0;
#else
	fbo_tex_rgb = 0;
#endif
	
#ifdef FRAMEBUFFER_ENABLE_DEPTH_TEXTURE
	fbo_tex_depth = 0;
#else
	fbo_rb_depth = 0;	
#endif
	
	create(width, height, hasDepth);
}

Framebuffer::~Framebuffer()
{
	destroy();
}

void Framebuffer::attach()
{
	glBindFramebuffer( GL_FRAMEBUFFER, fbo_id );
	
#ifdef FRAMEBUFFER_ENABLE_MRT
	GLenum attachments[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2,attachments);	
#endif
}

void Framebuffer::detach()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

#ifdef FRAMEBUFFER_ENABLE_MRT
GLuint Framebuffer::getColorTextureName0()
{
	return fbo_tex_rgb[0];
}

GLuint Framebuffer::getColorTextureName1()
{
	return fbo_tex_rgb[1];
}
#else
GLuint Framebuffer::getColorTextureName()
{
	return fbo_tex_rgb;
}
#endif

#ifdef FRAMEBUFFER_ENABLE_DEPTH_TEXTURE
GLuint Framebuffer::getDepthTextureName()
{
	return fbo_tex_depth;
}
#endif

void Framebuffer::create(GLsizei width, GLsizei height, bool hasDepth)
{
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

#ifdef FRAMEBUFFER_ENABLE_MRT
	glGenTextures( 2, fbo_tex_rgb );
	glBindTexture( GL_TEXTURE_2D, fbo_tex_rgb[0] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	glBindTexture( GL_TEXTURE_2D, fbo_tex_rgb[1] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	GLenum attachments[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2,attachments);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[0], GL_TEXTURE_2D, fbo_tex_rgb[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[1], GL_TEXTURE_2D, fbo_tex_rgb[1], 0);
#else
	glGenTextures(1, &fbo_tex_rgb);
	glBindTexture(GL_TEXTURE_2D, fbo_tex_rgb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex_rgb, 0);
#endif
	
	if (hasDepth)
	{
#ifdef FRAMEBUFFER_ENABLE_DEPTH_TEXTURE
		glGenTextures(1, &fbo_tex_depth);
		glBindTexture(GL_TEXTURE_2D, fbo_tex_depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo_tex_depth, 0);	
#else
		glGenRenderbuffers(1, &fbo_rb_depth);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo_rb_depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_rb_depth);	
		glBindRenderbuffer(GL_RENDERBUFFER, 0);	
#endif
	}

	checkFramebufferStatus();
	
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	
}

void Framebuffer::destroy()
{
	glDeleteFramebuffers(1, &fbo_id);
	fbo_id = 0;
	
#ifdef FRAMEBUFFER_ENABLE_MRT
	glDeleteTextures(2, fbo_tex_rgb);
	fbo_tex_rgb[0] = fbo_tex_rgb[1] = 0;
#else
	glDeleteTextures(1, &fbo_tex_rgb);
	fbo_tex_rgb = 0;
#endif
	
#ifdef FRAMEBUFFER_ENABLE_DEPTH_TEXTURE
	if (fbo_tex_depth != 0)
	{
		glDeleteTextures(1, &fbo_tex_depth);
		fbo_tex_depth = 0;
	}
#else
	if (fbo_rb_depth != 0)
	{
		glDeleteRenderbuffers(1, &fbo_rb_depth);
		fbo_rb_depth = 0;
	}
#endif

	width = height = 0;
}

GLsizei Framebuffer::getWidth()
{
	return width;
}

GLsizei Framebuffer::getHeight()
{
	return height;
}

