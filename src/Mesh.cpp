/*
 *  Mesh.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/30.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Mesh.h"
#include <cstdio>
#include <cstdlib>

using namespace std;

//#defile ENABLE_CHECK_GL_ERROR

#ifdef __cplusplus
#define BUFFER_OFFSET(bytes) ((GLubyte *)0 + (bytes))
#else
#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))
#endif

namespace
{
bool CheckGLError(int line)
{
#ifdef ENABLE_CHECK_GL_ERROR
	GLenum err = glGetError();
	switch (glGetError())
	{
	case GL_NO_ERROR:          return true;
	case GL_INVALID_ENUM:      printf("[%d] GL_INVALID_ENUM \n", line);		break;
	case GL_INVALID_VALUE:     printf("[%d] GL_INVALID_VALUE \n", line);		break;
	case GL_INVALID_OPERATION: printf("[%d] GL_INVALID_OPERATION \n", line);	break;
	case GL_STACK_OVERFLOW:    printf("[%d] GL_STACK_OVERFLOW \n", line);		break;
	case GL_STACK_UNDERFLOW:   printf("[%d] GL_STACK_UNDERFLOW \n", line);		break;
	case GL_OUT_OF_MEMORY:     printf("[%d] GL_OUT_OF_MEMORY \n", line);		break;
	default:                   printf("[%d] Unknown error \n", line);	break;
	}
	return false;
#else
	(void)line;
	return true;
#endif
}
}

Mesh::Mesh(int nvx, int nvy)
{
	if (nvx < 2)
		nvx = 2;
	if (nvy < 2)
		nvy = 2;
	/*
	 if (nvx < 2 || nvy < 2)
	 isFace = false;
	 else
	 isFace = true;
	 */
	this->nvx = nvx;
	this->nvy = nvy;
	
	glGenBuffers(1, &vertexBuf);

	
	int num = 2 + 3 + 3;
	int vbCount = nvx * nvy * num;
	GLfloat* vertexBufferData = new float[vbCount];
	GLsizeiptr vertexBufSize = (GLsizeiptr)(vbCount * sizeof(GLfloat));
	int counter = 0;
	for (int y = 0; y < nvy; y++)
	{
		for (int x = 0; x < nvx; x++)
		{
			float fx = (float)x / (float)(nvx - 1);
			float fy = (float)y / (float)(nvy - 1);
			
			// T
			vertexBufferData[counter++] = (fx);
			vertexBufferData[counter++] = (1.0f - fy);
			
			// N
			vertexBufferData[counter++] = (0);
			vertexBufferData[counter++] = (0);
			vertexBufferData[counter++] = (1);
			
			// V
//			float dx = (2.0f * (float)rand() / (float)RAND_MAX - 1.0f) / (float)nvx;
//			float dy = (2.0f * (float)rand() / (float)RAND_MAX - 1.0f) / (float)nvy;
//			vertexBufferData[counter++] = (fx * 2.0f - 1.0f) + dx;
//			vertexBufferData[counter++] = (1.0f - fy * 2.0f) + dy;
			vertexBufferData[counter++] = (fx * 2.0f - 1.0f);
			vertexBufferData[counter++] = (1.0f - fy * 2.0f);
			vertexBufferData[counter++] = (0.0f);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, vertexBufSize, vertexBufferData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (vertexBufferData != 0)
		delete[] vertexBufferData;
	

	/*
	 0 2 4 6 ... nvx
	 1 3 5 7 ... nvx + 1
	 */
	/*
	 1 3 5 7 ... nvx + 1
	 0 2 4 6 ... nvx * 2
	 */
	/*
	 0 2 4 6 ... nvx * 2
	 1 3 5 7 ... nvx * 2 + 1
	 */
	// buffer ids
	indexBufsCount = nvy - 1;
	indexBufs = new GLuint[indexBufsCount];
	glGenBuffers(indexBufsCount, indexBufs);

	// buffer data
	indexCount = nvx * 2;
	GLuint* indexBufferData = new GLuint[indexCount];
	GLsizeiptr indexBufferSize = (GLsizeiptr)(indexCount * sizeof(GLuint));
	for (int line = 0; line < indexBufsCount; line++)
	{
		counter = 0;
		for (int x = 0; x < nvx; x++)
		{
			indexBufferData[counter++] = x + line * nvx;
			indexBufferData[counter++] = x + (line + 1) * nvx;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufs[line]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indexBufferData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	if (indexBufferData != 0)
		delete[] indexBufferData;
	
	CheckGLError(__LINE__);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vertexBuf);
	glDeleteBuffers(indexBufsCount, indexBufs);
}

void Mesh::draw(DrawMode mode)
{
	glBindBuffer(GL_ARRAY_BUFFER_ARB, vertexBuf);
	CheckGLError(__LINE__);
	glInterleavedArrays(GL_T2F_N3F_V3F, 0, BUFFER_OFFSET(0));
	CheckGLError(__LINE__);
	
	GLenum dmode;
	if (mode == DRAW_MODE_POINT)
	{
		dmode = GL_POINTS;
		for (int line = 0; line < indexBufsCount; line++)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufs[line]);
			CheckGLError(__LINE__);
			glDrawElements(dmode, indexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
			CheckGLError(__LINE__);
		}
	}	
	else if (mode == DRAW_MODE_FILL)
	{
		dmode = GL_TRIANGLE_STRIP;
		for (int line = 0; line < indexBufsCount; line++)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufs[line]);
			glDrawElements(dmode, indexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		}
	}
	else
	{
		dmode = GL_LINES;
		for (int line = 0; line < indexBufsCount; line++)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufs[line]);
			glDrawElements(dmode, indexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		}
	}		

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
