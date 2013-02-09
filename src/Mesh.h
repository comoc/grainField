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
 *  Mesh.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/30.
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
