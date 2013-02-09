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
 *  ImageSource.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/23.
 *
 */

#include "ImageSource.h"
#include <cstring>
using namespace std;

const char* ImageSource::PATH_LIVE_CAMERA = "LIVE_CAMERA";

void ofLog(ofLogLevel l, char const* f, ...)
{
}



ImageSource::ImageSource()
    : type(TYPE_UNKNOWN), image(0)
{
    path[0] = '\0';
}

ImageSource::~ImageSource()
{
    if (image != 0)
    {
        if (type == TYPE_VIDEO)
            delete ((ofVideoPlayer*)image);
        else if (type == TYPE_LIVE_CAMERA)
            delete ((ofVideoGrabber*)image);
        else if (type == TYPE_IMAGE)
            delete ((ofImage*)image);
        image = 0;
    }
}

bool ImageSource::initialize(const char* path)
{
    if (path == 0)
        return false;

    if (image != 0)
    {
        if (type == TYPE_VIDEO)
            delete ((ofVideoPlayer*)image);
        else if (type == TYPE_LIVE_CAMERA)
            delete ((ofVideoGrabber*)image);
        else if (type == TYPE_IMAGE)
            delete ((ofImage*)image);
        image = 0;
    }

    strncpy(this->path, path, PATH_MAX);
    this->path[PATH_MAX - 1] = '\n';
    return true;
}

void ImageSource::play()
{
    if (image != 0)
        return;

    if (strcmp(path, PATH_LIVE_CAMERA) == 0)
    {
        ofVideoGrabber* vg = new ofVideoGrabber();
        if (!vg->initGrabber(CAMERA_WIDTH, CAMERA_HEIGHT, false))
        {
            delete vg;
            return;
        }

        image = vg;
        type = TYPE_LIVE_CAMERA;
    }
    else
    {
        size_t len = strlen(path);
        if (len == 0)
            return;

        int lastDotPos = -1;
        for (size_t i = len - 1; i != 0; i--)
        {
            if (path[i] == '.')
            {
                lastDotPos = i;
                break;
            }
        }
        if (lastDotPos <= 1)
            return;

        char ext[6];
        strncpy(ext, path + lastDotPos, 5);
        ext[5] = '\0';
        for (size_t i = strlen(ext) - 1; i != 0; i--)
        {
            ext[i] = tolower(ext[i]);
        }

        printf("[%s] %s\n", __PRETTY_FUNCTION__, path);

        if (strcmp(ext, ".3gpp") == 0
                || strcmp(ext, ".mov") == 0
                || strcmp(ext, ".mp4") == 0
                || strcmp(ext, ".wmv") == 0
                || strcmp(ext, ".avi") == 0
                || strcmp(ext, ".dv") == 0
           )
        {
            ofVideoPlayer* vp = new ofVideoPlayer();
            vp->setUseTexture(false);
            vp->loadMovie(path);
            vp->setVolume(0.0f);
            image = vp;
            type = TYPE_VIDEO;
        }
        else if (strcmp(ext, ".bmp") == 0
                 || strcmp(ext, ".jpg") == 0
                 || strcmp(ext, ".jpeg") == 0
                 || strcmp(ext, ".png") == 0
                 || strcmp(ext, ".tif") == 0
                 || strcmp(ext, ".tiff") == 0
                 || strcmp(ext, ".gif") == 0
                 || strcmp(ext, ".sgi") == 0
                 || strcmp(ext, ".pict") == 0
                 || strcmp(ext, ".pct") == 0
                 || strcmp(ext, ".png") == 0
                )
        {
            ofImage* img = new ofImage();
            img->loadImage(path);
            image = img;
            type = TYPE_IMAGE;
        }
    }

    if (image != 0 && type == TYPE_VIDEO)
        ((ofVideoPlayer*)image)->play();
}

void ImageSource::stop()
{
    //if (type == TYPE_VIDEO)
    //	((ofVideoPlayer*)image)->stop();

    if (image != 0 && type == TYPE_VIDEO)
    {
        ((ofVideoPlayer*)image)->stop();
        if (type == TYPE_VIDEO)
            delete ((ofVideoPlayer*)image);
        else if (type == TYPE_LIVE_CAMERA)
            delete ((ofVideoGrabber*)image);
        else if (type == TYPE_IMAGE)
            delete ((ofImage*)image);

        image = 0;
        type = TYPE_UNKNOWN;
    }
}

void ImageSource::update()
{
    if (image == 0)
        return;

    if (type == TYPE_VIDEO || type == TYPE_LIVE_CAMERA)
        ((ofBaseVideo*)image)->update();
}

int ImageSource::getWidth()
{
    if (image == 0)
        return 0;

    if (type == TYPE_VIDEO)
        return ((ofVideoPlayer*)image)->width;
    else if (type == TYPE_LIVE_CAMERA)
        return ((ofVideoGrabber*)image)->width;
    else if (type == TYPE_IMAGE)
        return ((ofImage*)image)->width;
    return 0;
}

int ImageSource::getHeight()
{
    if (image == 0)
        return 0;

    if (type == TYPE_VIDEO)
        return ((ofVideoPlayer*)image)->height;
    else if (type == TYPE_LIVE_CAMERA)
        return ((ofVideoGrabber*)image)->height;
    else if (type == TYPE_IMAGE)
        return ((ofImage*)image)->height;
    return 0;
}

int ImageSource::getBytesPerPixel()
{
    if (image == 0)
        return 0;

    if (type == TYPE_VIDEO)
        return 3;//((ofVideoPlayer*)image)->bpp;
    else if (type == TYPE_LIVE_CAMERA)
        return 3;//((ofVideoGrabber*)image)->bpp;
    else if (type == TYPE_IMAGE)
    {
        int bytepp = ((ofImage*)image)->bpp / 8;
        return bytepp;
    }
    return 0;
}

unsigned char* ImageSource::getPixels()
{
    if (image == 0)
        return 0;

    if (type == TYPE_VIDEO)
        return ((ofVideoPlayer*)image)->getPixels();
    else if (type == TYPE_LIVE_CAMERA)
        return ((ofVideoGrabber*)image)->getPixels();
    else if (type == TYPE_IMAGE)
        return ((ofImage*)image)->getPixels();

    return 0;
}

