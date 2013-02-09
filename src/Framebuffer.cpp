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
 *  Framebuffer.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/08/25.
 *
 */

#include "Framebuffer.h"
#include <iostream>

using namespace std;

namespace
{
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
    GLenum attachments[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1
    };
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

    GLenum attachments[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1
    };
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

