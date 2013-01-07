/*
 *  GLShader.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/14.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GLSHADER_H
#define _GLSHADER_H

#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

class GLShader
{
public:
	GLShader();
	~GLShader();
	bool create(const char* vertexShaderFilePath, const char* fragmentShaderFilePath);
	void destroy();
	GLuint activate();
	void inactivate();
private:
	GLuint program;
};

#endif