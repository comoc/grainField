/*
 *  Mesh.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/30.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MESH_H
#define _MESH_H

#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

class Mesh
{
public:
	enum DrawMode
	{
		DRAW_MODE_POINT,
		DRAW_MODE_LINE,
		DRAW_MODE_FILL,
	};
    Mesh(int nvx, int nvy);
    ~Mesh();
    void draw(DrawMode mode);
private:
    int nvx;
    int nvy;
    int indexCount;
    GLuint vertexBuf;
    GLuint* indexBufs;
	GLsizei indexBufsCount;
};

#endif /* _MESH_H */
