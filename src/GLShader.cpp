/*
 *  GLShader.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/14.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GLShader.h"
#include <cstdio>
#include <cstdlib>

using namespace std;

namespace {
	int loadShader(GLuint shader, const char *file)
	{
		FILE *fp;
		const GLchar *source;
		GLsizei length;
		int ret;
		
		fp = fopen(file, "rb");
		if (fp == NULL) {
			perror(file);
			return -1;
		}
		
		fseek(fp, 0L, SEEK_END);
		length = ftell(fp);
		
		source = (GLchar *)malloc(length);
		if (source == NULL) {
			fprintf(stderr, "Could not allocate read buffer.\n");
			return -1;
		}
		
		fseek(fp, 0L, SEEK_SET);
		ret = fread((void *)source, 1, length, fp) != (size_t)length;
		fclose(fp);
		
		if (ret)
			fprintf(stderr, "Could not read file: %s.\n", file);
		else
			glShaderSource(shader, 1, &source, &length);
		
		free((void *)source);
		
		return ret;
	}

	void printShaderInfoLog(GLuint shader)
	{
		GLsizei bufSize;
		
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
		
		if (bufSize > 1) {
			GLchar *infoLog = (GLchar *)malloc(bufSize);
			if (infoLog != NULL) {
				GLsizei length;
				
				glGetShaderInfoLog(shader, bufSize, &length, infoLog);
				printf("glGetShaderInfoLog:\n%s\n", infoLog);
				free(infoLog);
			}
			else
				fprintf(stderr, "Could not allocate InfoLog buffer.\n");
		}
	}
	
	void printProgramInfoLog(GLuint program)
	{
		GLsizei bufSize;
		
		glGetProgramiv(program, GL_INFO_LOG_LENGTH , &bufSize);
		
		if (bufSize > 1) {
			GLchar *infoLog = (GLchar *)malloc(bufSize);
			if (infoLog != NULL) {
				GLsizei length;
				
				glGetProgramInfoLog(program, bufSize, &length, infoLog);
				printf("glGetProgramInfoLog:\n%s\n", infoLog);
				free(infoLog);
			}
			else
				printf("Could not allocate InfoLog buffer.\n");
		}
	}	
}

GLShader::GLShader()
: program(0)
{
}

GLShader::~GLShader()
{
}

bool GLShader::create(const char* vertexShaderFilePath, const char* fragmentShaderFilePath)
{
    GLuint vtxShader;
    GLuint frgShader;
    GLuint prog;
    GLint status;
	
    vtxShader = glCreateShader(GL_VERTEX_SHADER);
    frgShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (loadShader(vtxShader, vertexShaderFilePath) < 0)
		goto ERROR;
	
    if (loadShader(frgShader, fragmentShaderFilePath) < 0)
		goto ERROR;
	
	glCompileShader(vtxShader);
	printShaderInfoLog(vtxShader);
    glGetProgramiv(prog, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
	{
		goto ERROR;
	}
	
	glCompileShader(frgShader);
	printShaderInfoLog(frgShader);
    glGetProgramiv(prog, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
	{
		goto ERROR;
	}
	
    prog = glCreateProgram();
	
    glAttachShader(prog, vtxShader);
    glAttachShader(prog, frgShader);

    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
	{
		printProgramInfoLog(prog);
		glDeleteProgram(prog);
		goto ERROR;
	}
    program = prog;

ERROR:
    glDeleteShader(vtxShader);
    glDeleteShader(frgShader);
	
    return program != 0;
}

void GLShader::destroy()
{
	if (program != 0)
	{
		glDeleteProgram(program);
	}
}

GLuint GLShader::activate()
{
	if (program != 0)
	{
		glUseProgram(program);
		return program;
	}
	return 0;
}

void GLShader::inactivate()
{
	glUseProgram(0);
}

