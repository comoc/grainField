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

#include "testApp.h"

#if defined(TARGET_WIN32)
#include <windows.h>
#else
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#if defined(__APPLE__)
#include <GLUT/glut.h>
#endif

#include <list>

#include "Mesh.h"
#include "GLshader.h"
#include "Framebuffer.h"

#include "MultiTouch.h"

const float testApp::PEAK_DECAY_MAX = 0.99f;
const float testApp::PEAK_DECAY_MIN = 0.1f;
const float testApp::PEAK_DECAY_DEFAULT = 0.98f;

const int testApp::SAMPLES = 256;

const float testApp::PARAMETER_STEP = 0.1f;
const float testApp::KEY_REPEAT_PROCESS_SKIP_INTERVAL_SEC = 2.f;

#if 1
namespace
{

bool ofKeyAlt()
{
    return (glutGetModifiers() & GLUT_ACTIVE_ALT);
}

bool ofKeyShift()
{
    return (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
}

bool ofKeyControl()
{
    return (glutGetModifiers() & GLUT_ACTIVE_CTRL);
}

string GetDataDirectory()
{
    string dataDir;
#ifdef TARGET_OSX
    // Get the absolute location of the executable file in the bundle.
    CFBundleRef appBundle     = CFBundleGetMainBundle();
    CFURLRef   executableURL = CFBundleCopyExecutableURL(appBundle);
    char execFile[4096];
    if (CFURLGetFileSystemRepresentation(executableURL, TRUE, (UInt8 *)execFile, 4096))
    {
        // Strip out the filename to just get the path
        string strExecFile = execFile;
        int found = strExecFile.find_last_of("/");
        string strPath = strExecFile.substr(0, found);

        // Change the working directory to that of the executable
        struct stat st;

        if (stat(strPath.c_str(), &st) != 0 || S_ISDIR(st.st_mode) == 0)
        {
            ofLog(OF_LOG_ERROR, "Unable to change working directory to executable's directory.");
        }
        else
        {

            // Change the working directory to to that of the data
            dataDir = strPath;
            dataDir += "/../../../data";
            if(stat(strPath.c_str(), &st) != 0 || S_ISDIR(st.st_mode) == 0)
            {
                ofLog(OF_LOG_ERROR, "Unable to change working directory to data directory.");
                dataDir = "";
            }
        }

    }
    else
    {
        ofLog(OF_LOG_ERROR, "Unable to identify executable's directory.");
    }
    CFRelease(executableURL);
#else
#error TBD
#endif
    return dataDir;
}

void ScanDirectorty(const char* path, void(*callback)(void*, bool, const char*), void* param, bool isRecursive, bool isSkipDotFile = true)
{
#if defined(TARGET_WIN32)

    WIN32_FIND_DATAA fd;
    char fileName[_MAX_PATH];

    if (strlen(path) == 0)
        return;

    strcpy_s(fileName, _MAX_PATH, path);
    if (fileName[strlen(fileName) - 1] != '\\')
        strcat_s(fileName, _MAX_PATH, "\\");
    strcat_s(fileName, _MAX_PATH, "*");

    HANDLE h = FindFirstFileA(fileName, &fd);

    if (h == INVALID_HANDLE_VALUE)
        return;

    char fullPath[_MAX_PATH];
    do
    {
        if (isSkipDotFile && fd.cFileName[0] == '.')
        {
            goto SKIP;
        }

        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {

            if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
            {

                if (callback != NULL)
                {

                    strcpy_s(fullPath, _MAX_PATH, fileName);
                    strcat_s(fullPath, _MAX_PATH, fd.cFileName);
                    callback(param, true, fullPath);
                }

                if (isRecursive)
                {
                    strcpy_s(fullPath, _MAX_PATH, fileName);
                    strcat_s(fullPath, _MAX_PATH, fd.cFileName);
                    strcat_s(fullPath, _MAX_PATH, "\\");
                    ScanDirectorty(fullPath, callback, param, isRecursive);
                }
            }
        }
        else
        {
            if (callback != NULL)
            {
                strcpy_s(fullPath, _MAX_PATH, fileName);
                strcat_s(fullPath, _MAX_PATH, fd.cFileName);
                callback(param, false, fullPath);
            }

        }
SKIP:
    }
    while (FindNextFileA( h, &fd));

    FindClose(h);

#elif defined(TARGET_LINUX) || defined(TARGET_OSX)
    struct stat st;
    char pathName[PATH_MAX];

    DIR *dir = opendir(path);
    if (dir == NULL)
        return;

    dirent* entry = readdir(dir);
    while (entry != NULL)
    {
        if (isSkipDotFile && entry->d_name[0] == '.')
        {
            goto SKIP;
        }

        if (strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ))
        {
            strncpy(pathName, path, PATH_MAX);
            pathName[PATH_MAX - 1] = '\0';

            strncat(pathName, "/", PATH_MAX);
            pathName[PATH_MAX - 1] = '\0';

            strncat(pathName, entry->d_name, PATH_MAX);
            pathName[PATH_MAX - 1] = '\0';

            if (stat(pathName, &st) != 0)
                break;

            if (S_ISDIR(st.st_mode) != 0)
            {
                if (callback != NULL)
                    callback(param, true, pathName);//entry->d_name);

                if (isRecursive)
                    ScanDirectorty(pathName, callback, param, isRecursive);
            }
            else
            {
                if (callback != NULL)
                    callback(param, false, pathName);//entry->d_name);
            }
        }
SKIP:
        entry = readdir( dir );
    }

    closedir( dir );

#endif
}
}

namespace
{
void ScanDirCallback(void* param, bool isDir, const char* path)
{
    if (!isDir)
    {
        list<string>* l = (list<string>*)param;
        l->push_back(path);
    }
}
}
#endif


namespace
{
void IncrementPosition(ControllableParameter* p)
{
    p->setPosition(p->getPosition() + 1);

}
void DecrementPosition(ControllableParameter* p)
{
    p->setPosition(p->getPosition() - 1);

}
}

testApp::testApp()
    : imageSourceA(0)
    , imageSourceB(0)
    , imageSourceCamera(0)
    , imageSourceNoise(0)
    , imageSourcePointSprite(0)

    , imageSourceIndexA(0)
    , imageSourceIndexB(0)
    , imageSourceIndexNoise(0)
    , imageSourceIndexPointSprite(0)

    , imageSourceAspectRatio(1.0f)

    , frameByframe(0)

    , textureIds(0)
    , textureIdsNum(0)
    , mesh(0)

    , left(0)
    , right(0)
    , peakLeft(0)
    , peakRight(0)
    , peakLeftRight(0)
    , outLeft(0)
    , outRight(0)
    , planLeft(0)
    , planRight(0)
    , fftPeakLeft(0)
    , fftPeakRight(0)

    , meshNumXOld(0)
    , meshNumYOld(0)

    , width(0)
    , height(0)
    , pmouseX(0)
    , pmouseY(0)
    , fbo(0)
    , fboFeedback(0)
    , shader(0)
    , shaderDof(0)

    , isShiftPressed(false)
    , isControlPressed(false)
    , isAltPressed(false)


    , activePresetParamIndexA(-1)
    , activePresetParamIndexB(-1)

    , overlayImageAlpha(1.0f)
    , forceBeat(false)
    , noiseImage(0)
    , noiseFeedback(0.5f)

    , lastParameterFade(0)
{
}


//--------------------------------------------------------------
void testApp::setup()
{
    InitMultiTouch();

    noiseImage = new GLuint[256 * 256];

    beatDetector.reset();
    beatTimer.setLoop(true);
    forceBeat = false;

    overlayImageAlpha = 0.0f;

    ofSetBackgroundAuto(false);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("%s\n", (const char*)gluErrorString(err));
    }


    string dataDir = GetDataDirectory();

    updateWindowParameters();

    resetControllableParameters();

    fbo = new Framebuffer(512, 512, true);
    fboFeedback = new Framebuffer(512, 512, false);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("%s\n", (const char*)gluErrorString(err));
    }

    meshNumXOld = (int)meshNumX.get();
    meshNumYOld = (int)meshNumY.get();

    mesh = new Mesh((int)meshNumX.get(), (int)meshNumY.get());

    //mesh = new Mesh(3, 3);

    ofBackground(0, 0, 0);
    ofHideCursor();

    frameByframe = false;


    {
        string vertexShader(dataDir);
        vertexShader += "/shaders/vert.glsl";
        string fragmentShader(dataDir);
        fragmentShader += "/shaders/frag.glsl";

        shader = new GLShader();

        if (shader->create(vertexShader.c_str(), fragmentShader.c_str()))
        {

        }
    }

    {
        string vertexShader(dataDir);
        vertexShader += "/shaders/vertfb.glsl";
        string fragmentShader(dataDir);
        fragmentShader += "/shaders/fragfb.glsl";

        shaderDof = new GLShader();

        if (shaderDof->create(vertexShader.c_str(), fragmentShader.c_str()))
        {

        }
    }



    string controllerAssign(dataDir);
    controllerAssign += "/controller/assignment.json";
    if (controller.initialize(controllerAssign.c_str()))
    {
    }

#if 1
    ImageSource* is = new ImageSource;
    if (is->initialize(ImageSource::PATH_LIVE_CAMERA))
    {
        imageSourceCamera = is;
        imageSourceCamera->play();
        printf("Live camera is available\n");
    }
    else
    {
        delete is;
        printf("Live camera is not available\n");
    }
#else
    ImageSource* is = new ImageSource;
    string movieSource(dataDir);
    movieSource += "/movies/station.mov";
    if (is->initialize(movieSource.c_str()))
    {
        imageSourceCamera = is;
        imageSourceCamera->play();
        printf("Live camera is available\n");
    }
    else
    {
        delete is;
        printf("Live camera is not available\n");
    }
#endif


    list<string> files;
    string movieDir(dataDir);
    movieDir += "/movies";
    ScanDirectorty(movieDir.c_str(), ScanDirCallback, &files, true);
    for (list<string>::iterator it = files.begin(); it != files.end(); it++)
    {
        ImageSource* is = new ImageSource;
        if (is->initialize((*it).c_str()))
        {
            is->stop();
            imageSources.push_back(is);
            printf("Loaded: %s\n", (*it).c_str());
        }
        else
        {
            delete is;
            printf("Not loaded: %s\n", (*it).c_str());
        }
    }
    files.clear();


    movieDir = dataDir;
    movieDir += "/secondary_movies";
    ScanDirectorty(movieDir.c_str(), ScanDirCallback, &files, true);
    for (list<string>::iterator it = files.begin(); it != files.end(); it++)
    {
        ImageSource* is = new ImageSource;
        if (is->initialize((*it).c_str()))
        {
            is->stop();
            secondaryImageSources.push_back(is);
            printf("Loaded: %s\n", (*it).c_str());
        }
        else
        {
            delete is;
            printf("Not loaded: %s\n", (*it).c_str());
        }
    }
    files.clear();


    string noiseDir(dataDir);
    noiseDir += "/noise_movies";
    ScanDirectorty(noiseDir.c_str(), ScanDirCallback, &files, true);
    for (list<string>::iterator it = files.begin(); it != files.end(); it++)
    {
        ImageSource* is = new ImageSource;
        if (is->initialize((*it).c_str()))
        {
            is->stop();
            noiseImageSources.push_back(is);
            printf("Loaded: %s\n", (*it).c_str());
        }
        else
        {
            delete is;
            printf("Not loaded: %s\n", (*it).c_str());
        }
    }
    files.clear();

#if 0
    string pointSpriteDir(dataDir);
    pointSpriteDir += "/point_sprite_movies";
    ScanDirectorty(pointSpriteDir.c_str(), ScanDirCallback, &files, true);
    for (list<string>::iterator it = files.begin(); it != files.end(); it++)
    {
        ImageSource* is = new ImageSource;
        if (is->initialize((*it).c_str()))
        {
            is->stop();
            pointSpriteImageSources.push_back(is);
            printf("Loaded: %s\n", (*it).c_str());
        }
        else
        {
            delete is;
            printf("Not loaded: %s\n", (*it).c_str());
        }
    }
    files.clear();
#endif

    imageSourceA = imageSourceB = imageSourceNoise = 0;
    imageSourceIndexA = imageSourceIndexB = imageSourceIndexNoise = -1;

    if (imageSources.size() > 0)
    {
        imageSourceIndexA = 0;
        imageSourceA = imageSources[imageSourceIndexA];
        imageSourceA->play();
    }

    if (secondaryImageSources.size() > 0)
    {
        imageSourceIndexB = 0;
        imageSourceB = secondaryImageSources[imageSourceIndexB];
        imageSourceB->play();
    }

    if (noiseImageSources.size() > 0)
    {
        imageSourceIndexNoise = 0;
        imageSourceNoise = noiseImageSources[imageSourceIndexNoise];
        imageSourceNoise->play();
    }

    if (pointSpriteImageSources.size() > 0)
    {
        imageSourceIndexPointSprite = 0;
        imageSourcePointSprite = pointSpriteImageSources[imageSourceIndexPointSprite];
        imageSourcePointSprite->play();
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

//	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &textureIdsNum);
    textureIdsNum = 16;
    textureIds = new GLuint[textureIdsNum];
    glGenTextures(textureIdsNum, textureIds);
    
    pmouseX = pmouseY = 0;

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0, 1, 0);
    glScalef(1, -1, 1);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    //glEnable(GL_POINT_SPRITE);


    //glPointSize(2);

    glEnable(GL_NORMALIZE);

#if 0
    glDepthFunc(GL_ALWAYS);
#endif

    glDisable(GL_CULL_FACE);

    glClearColor(0, 0, 0, 0);

    fboFeedback->attach();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fboFeedback->detach();


    // audio, FFT
    ofSoundStreamSetup(0,2, this, 44100, SAMPLES, 4);
    left = new float[SAMPLES];
    right = new float[SAMPLES];

    peakLeft = 0.0f;
    peakRight = 0.0f;
    peakLeftRight = 0.0f;

    outLeft = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * SAMPLES);
    outRight = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * SAMPLES);
    planLeft = fftwf_plan_dft_r2c_1d(SAMPLES, left, outLeft, FFTW_ESTIMATE);
    planRight = fftwf_plan_dft_r2c_1d(SAMPLES, right, outRight, FFTW_ESTIMATE);

    fftPeakLeft = new float[SAMPLES];
    fftPeakRight = new float[SAMPLES];
    for (int i = 0; i < SAMPLES; i++)
        fftPeakLeft[i] = fftPeakRight[i] = 0.0f;
    
    for (int i = 0; i < PEAK_HISTORY_COUNT; i++)
        peakHistory[i] = 0.0f;
    glGenTextures(1, &texturePeakId);
    glBindTexture(texturePeakId, GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, PEAK_HISTORY_COUNT, 1, 0, GL_LUMINANCE, GL_FLOAT, peakHistory);
    
    // osc
    receiver.setup( IN_PORT );

#ifdef USE_MIDI_IN
    // midi
    midiIn.listPorts();
    midiIn.openPort();
    midiIn.addListener(this);
#endif

#if 0
    // camera capture
    cameraCapture = cvCreateCameraCapture(-1);
    cameraCaptureBuffer = 0;
    cameraCaptureBufferSize = 0;
#endif
}

void testApp::exit()
{
    ofSoundStreamClose();

#ifdef USE_MIDI_IN
    midiIn.removeListener(this);
#endif
    if (shader != 0)
    {
        shader->destroy();
        delete shader;
        shader = 0;
    }

    if (shaderDof != 0)
    {
        shaderDof->destroy();
        delete shaderDof;
        shaderDof = 0;
    }

    if (imageSourceCamera != 0)
    {
        delete imageSourceCamera;
        imageSourceCamera = 0;
    }


    for (vector<ImageSource*>::iterator it = imageSources.begin(); it != imageSources.end(); it++)
    {
        delete (*it);
    }
    imageSources.clear();

    for (vector<ImageSource*>::iterator it = secondaryImageSources.begin(); it != secondaryImageSources.end(); it++)
    {
        delete (*it);
    }
    secondaryImageSources.clear();

    for (vector<ImageSource*>::iterator it = noiseImageSources.begin(); it != noiseImageSources.end(); it++)
    {
        delete (*it);
    }
    noiseImageSources.clear();

    for (vector<ImageSource*>::iterator it = pointSpriteImageSources.begin(); it != pointSpriteImageSources.end(); it++)
    {
        delete (*it);
    }
    pointSpriteImageSources.clear();

    if (textureIds != 0)
    {
        glDeleteTextures(textureIdsNum, textureIds);
        delete[] textureIds;
        textureIds = 0;
        textureIdsNum = 0;
    }
    
    glDeleteTextures(1, &texturePeakId);

    if (mesh != 0)
    {
        delete mesh;
        mesh = 0;
    }

    if (fbo != 0)
    {
        delete fbo;
        fbo = 0;
    }
    if (left != 0)
    {
        delete[] left;
        left = 0;
    }
    if (right == 0)
    {
        delete[] right;
        right = 0;
    }

    fftwf_destroy_plan(planLeft);
    fftwf_destroy_plan(planRight);

    fftwf_free(outLeft);
    fftwf_free(outRight);
    if (fftPeakLeft != 0)
    {
        delete[] fftPeakLeft;
        fftPeakLeft = 0;
    }
    if (fftPeakRight != 0)
    {
        delete[] fftPeakRight;
        fftPeakRight = 0;
    }

    if (noiseImage != 0)
        delete[] noiseImage;
}

//--------------------------------------------------------------
void testApp::update()
{

    for (vector<ImageSource*>::iterator it = imageSources.begin(); it != imageSources.end(); it++)
    {
        (*it)->update();
    }
    for (vector<ImageSource*>::iterator it = secondaryImageSources.begin(); it != secondaryImageSources.end(); it++)
    {
        (*it)->update();
    }
    for (vector<ImageSource*>::iterator it = noiseImageSources.begin(); it != noiseImageSources.end(); it++)
    {
        (*it)->update();
    }
    for (vector<ImageSource*>::iterator it = pointSpriteImageSources.begin(); it != pointSpriteImageSources.end(); it++)
    {
        (*it)->update();
    }
    if (imageSourceCamera != 0)
        imageSourceCamera->update();
}

//-------------	-------------------------------------------------
void testApp::draw()
{
    UpdateMultiTouch();

    updateWindowParameters();
    updateOscMessage();
#ifdef USE_MIDI_IN
    parseMidiData();
#endif
    
    
//	for (int ny = 0; ny < 256; ny++) {
//		int iy = ny * 256 * 2;
//		for (int nx = 0; nx < 256; nx++) {
//			int indexU = 2 * nx + iy;
//			int indexV = indexU + 1;
//			noiseImage[indexU] = (1.0f - noiseFeedback) * noiseImage[indexU] + noiseFeedback * (float)rand() / (float)RAND_MAX;
//			noiseImage[indexV] = (1.0f - noiseFeedback) * noiseImage[indexV] + noiseFeedback * (float)rand() / (float)RAND_MAX;
//		}
//	}

    int nfbshift = (int)(noiseFeedback * 8);
    GLuint* ptr = noiseImage;
    for (int ny = 0; ny < 256; ny++)
    {
        int iy = 256 * ny;
        for (int nx = 0; nx < 256; nx++)
        {
            int index = iy + nx;
            GLubyte* r = (GLubyte*)ptr;
            GLubyte* g = r + 1;
            GLubyte* b = g + 1;
            GLubyte* a = b + 1;
            *r = (rand() % 0xff) >> nfbshift;
            *g = (rand() % 0xff) >> nfbshift;
            *b = (rand() % 0xff) >> nfbshift;
            *a = (rand() % 0xff) >> nfbshift;
            ptr++;

        }
    }
    glColor4f(1, 1, 1, 1);


    fbo->attach();
    GLsizei ww = fbo->getWidth();
    GLsizei wh = fbo->getHeight();

    if (meshNumXOld != (int)meshNumX.get()
            || meshNumYOld != (int)meshNumY.get())
    {
        if (mesh != 0)
            delete mesh;
        mesh = new Mesh((int)meshNumX.get(), (int)meshNumY.get());
        meshNumXOld = (int)meshNumX.get();
        meshNumYOld = (int)meshNumY.get();
    }

    if ((int)pointSmoothMode.get() == 0)
        glDisable(GL_POINT_SMOOTH);
    else
        glEnable(GL_POINT_SMOOTH);

    float aspect = (float)ofGetWidth() / (float)ofGetHeight();

    printf("aspect=%f\n", imageSourceAspectRatio);
    //float pixelAspect = (((float)ww / (float)width) / ((float)wh / (float)height));
    //aspect = pixelAspect / 1.33f;
    //aspect = pixelAspect / aspect;


    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, ww, wh);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const int STEP = 24;
    float low = 0.0f, mid = 0.0f, high = 0.0f;
//    int sposy = 20;
    for (int i = 0; i < SAMPLES / 4; i+=STEP)
    {
        float foutLeft = 0.0f;
        float foutRight = 0.0f;
        float fout = 0.0f;
        for (int j = 0; j < STEP; j++)
        {
            if (foutLeft < fftPeakLeft[i + j])
                foutLeft = fftPeakLeft[i + j];
            if (foutRight < fftPeakRight[i + j])
                foutRight = fftPeakRight[i + j];
        }
        if (foutLeft > foutRight)
        {
            fout = foutLeft;
        }
        else
        {
            fout = foutRight;
        }

//        int n = (int)(fout * 10.0f + 0.5f);
//        string out;
//        for (int k = 0; k < n; k++)
//            out += "*";
        int l = i / STEP;
        if (l == 0)
            low = fout;
        else if (l == 1)
            mid = fout;
        else
            high = fout;

        low *= 0.1;
        high *= 10.0;

//        ofDrawBitmapString("[" + ofToString(l) + "]" + out, 20, sposy);
//        sposy += 20;
    }


    GLuint program = shader->activate();
    if (program != 0)
    {
        glUniform1f(glGetUniformLocation(program, "colorGain"), colorGain.get());
        glUniform1f(glGetUniformLocation(program, "displacementGain"), displacementGain.get());
        glUniform1f(glGetUniformLocation(program, "high"), high);
        glUniform1f(glGetUniformLocation(program, "mid"), mid);
        glUniform1f(glGetUniformLocation(program, "low"), low);
        glUniform1f(glGetUniformLocation(program, "peakLeftRight"), peakLeftRight * peakLumiGain.get());
        glUniform1f(glGetUniformLocation(program, "pointSize"), 2);
        glUniform1f(glGetUniformLocation(program, "pointSize"), paramPointSize.get() + peakLeftRight * peakPointSizeGain.get());
        glUniform1f(glGetUniformLocation(program, "textureBlend"), textureBlend.get());
        glUniform1f(glGetUniformLocation(program, "noiseGain"), noiseGain.get());
        glUniform1f(glGetUniformLocation(program, "zClip"), zClip.get());
        glUniform1f(glGetUniformLocation(program, "zOffset"), zOffset.get());

        glUniform1f(glGetUniformLocation(program, "timeFraction"), ofGetElapsedTimef());
//        glUniform1f(glGetUniformLocation(program, "aspectRatio"), 1);
        glUniform1f(glGetUniformLocation(program, "aspectRatio"), imageSourceAspectRatio);
        

        glUniform1f(glGetUniformLocation(program, "soundGain"), soundGain.get());
        glUniform1f(glGetUniformLocation(program, "soundPeakGain"), soundPeakGain.get());

//		glUniform1f(glGetUniformLocation(program, "width"), ww);
//		glUniform1f(glGetUniformLocation(program, "height"), wh);

    }

    GLenum texFilter;
    if (textureFilter.get() > 0.5f)
        texFilter =  GL_LINEAR;
    else
        texFilter =  GL_NEAREST;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    //    gluPerspective(90, aspect, 0.1, 100.0);
//	gluPerspective(45 + paramPointSize.get(), aspect, 0.1, 100.0);
    glOrtho(-aspect, aspect, -1, 1, -1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int nt = 0;

    if (imageSourceCamera != 0)
    {
        unsigned char * pixels = imageSourceCamera->getPixels();
        if (pixels != 0)
        {
            int mw = imageSourceCamera->getWidth();
            int mh = imageSourceCamera->getHeight();
            if (mh > 0)
                imageSourceAspectRatio = (float)mw / (float)mh;
            
            int bpp = imageSourceCamera->getBytesPerPixel();
            glActiveTexture(GL_TEXTURE0 + nt);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureIds[nt]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            GLenum format = GL_LUMINANCE;
            if (bpp == 1)
                format = GL_LUMINANCE;
            else if (bpp == 2)
                format = GL_LUMINANCE_ALPHA;
            else if (bpp == 3)
                format = GL_RGB;
            else if (bpp == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, mw, mh, 0, format, GL_UNSIGNED_BYTE, pixels);
            if (program != 0)
            {
                string textureUnitName("texture");
                textureUnitName += ofToString(nt);
                GLuint location = glGetUniformLocation(program, textureUnitName.c_str());
                glUniform1i(location, nt);
            }
            nt++;
        }
    }

    if (imageSourceA != 0)
    {
        unsigned char * pixels = imageSourceA->getPixels();
        if (pixels == 0)
        {
            imageSourceA->play();
            pixels = imageSourceA->getPixels();
        }

        if (pixels != 0)
        {
            int mw = imageSourceA->getWidth();
            int mh = imageSourceA->getHeight();
            
            int bpp = imageSourceA->getBytesPerPixel();
            glActiveTexture(GL_TEXTURE0 + nt);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureIds[nt]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            GLenum format = GL_LUMINANCE;
            if (bpp == 1)
                format = GL_LUMINANCE;
            else if (bpp == 2)
                format = GL_LUMINANCE_ALPHA;
            else if (bpp == 3)
                format = GL_RGB;
            else if (bpp == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, mw, mh, 0, format, GL_UNSIGNED_BYTE, pixels);
            if (program != 0)
            {
                string textureUnitName("texture");
                textureUnitName += ofToString(nt);
                GLuint location = glGetUniformLocation(program, textureUnitName.c_str());
                glUniform1i(location, nt);
            }
            nt++;
        }
    }

#if 0
    if (imageSourceNoise != 0)
    {
        unsigned char * pixels = imageSourceNoise->getPixels();
        if (pixels == 0)
        {
            imageSourceNoise->play();
            pixels = imageSourceNoise->getPixels();
        }

        if (pixels != 0)
        {
            int mw = imageSourceNoise->getWidth();
            int mh = imageSourceNoise->getHeight();
            int bpp = imageSourceNoise->getBytesPerPixel();
            glActiveTexture(GL_TEXTURE0 + nt);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureIds[nt]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            GLenum format = GL_LUMINANCE;
            if (bpp == 1)
                format = GL_LUMINANCE;
            else if (bpp == 2)
                format = GL_LUMINANCE_ALPHA;
            else if (bpp == 3)
                format = GL_RGB;
            else if (bpp == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, mw, mh, 0, format, GL_UNSIGNED_BYTE, pixels);
            if (program != 0)
            {
                string textureUnitName("texture");
                textureUnitName += ofToString(nt);
                GLuint location = glGetUniformLocation(program, textureUnitName.c_str());
                glUniform1i(location, nt);
            }
            nt++;
        }
    }
#else
    // Noise texture
    {
        glActiveTexture(GL_TEXTURE0 + nt);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureIds[nt]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, noiseImage);
        if (program != 0)
        {
            string textureUnitName("texture");
            textureUnitName += ofToString(nt);
            GLuint location = glGetUniformLocation(program, textureUnitName.c_str());
            glUniform1i(location, nt);
        }
        nt++;
    }
#endif

    // peak texture
    {
        for (int i = 1; i < PEAK_HISTORY_COUNT; i++)
            peakHistory[i - 1] = peakHistory[i];
        peakHistory[PEAK_HISTORY_COUNT - 1] = peakLeftRight * 2.0f;
        
        glActiveTexture(GL_TEXTURE0 + nt);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(texturePeakId, GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, PEAK_HISTORY_COUNT, 1, 0, GL_LUMINANCE, GL_FLOAT, peakHistory);
        if (program != 0)
        {
            string textureUnitName("texture");
            textureUnitName += ofToString(nt);
            GLuint location = glGetUniformLocation(program, textureUnitName.c_str());
            glUniform1i(location, nt);
        }
        nt++;
    }

    glEnable(GL_BLEND);

    int bmode = (int)floorf(blendingMode.get() + 0.5f);

    switch (bmode)
    {
    case 0:
        glBlendEquationEXT(GL_FUNC_ADD_EXT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthFunc(GL_ALWAYS);
        break;

#if 0
    case 2:
        glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        break;
    case 3:
        glBlendEquationEXT(GL_FUNC_ADD_EXT);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
        break;
    case 4:
        glBlendEquationEXT(GL_FUNC_ADD_EXT);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        break;
    case 5:
        glBlendEquationEXT(GL_FUNC_ADD_EXT);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
        break;
#endif
    default:
        glBlendEquationEXT(GL_FUNC_ADD_EXT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LEQUAL);
        break;
    }


    glTranslatef(0, 0, -distance.get());

    glTranslatef(rotationCenterX.get(), rotationCenterY.get(), 0);

    glRotatef(-rotationY.get(), 0, 1, 0);
    glRotatef(rotationX.get(), 1, 0, 0);


    autoRotationTimer.update(ofGetElapsedTimef());
    float angle = (autoRotationTimer.getFraction()) * 360.0f;
    glRotatef(angle, 1, 1, 1);

    GLfloat sx = scaleX.get();
    sx *= sx;
    GLfloat sy = scaleY.get();
    sy *= sy;
    glScalef(sx, sy, 1.0f);
    glScalef(imageSourceAspectRatio, 1.0f, 1.0f);


    int nl = (int)(layerCount.get());
    for (int l = 0; l < nl; l++)
    {
        glPushMatrix();
        if (l % 2 == 1)
            glScalef(1, 1, -1);
        if ((int)drawMode.get() == 0)
            mesh->draw(Mesh::DRAW_MODE_POINT);
        else if ((int)drawMode.get() == 1)
            mesh->draw(Mesh::DRAW_MODE_LINE);
        else
            mesh->draw(Mesh::DRAW_MODE_FILL);
        glPopMatrix();
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glActiveTexture(GL_TEXTURE2);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);

    shader->inactivate();

    /*
        fingerMovie.draw(20,20);
        ofSetColor(0x000000);
        unsigned char * pixels = fingerMovie.getPixels();
        // let's move through the "RGB" char array
        // using the red pixel to control the size of a circle.
        for (int i = 4; i < 320; i+=8){
            for (int j = 4; j < 240; j+=8){
                unsigned char r = pixels[(j * 320 + i)*3];
                float val = 1 - ((float)r / 255.0f);
                ofCircle(400 + i,20+j,10*val);
            }
        }


        ofSetColor(0x000000);
    	ofDrawBitmapString("press f to change",20,320);
        if(frameByframe) ofSetColor(0xCCCCCC);
        ofDrawBitmapString("mouse speed position",20,340);
        if(!frameByframe) ofSetColor(0xCCCCCC); else ofSetColor(0x000000);
        ofDrawBitmapString("keys <- -> frame by frame " ,190,340);
        ofSetColor(0x000000);
    */

#if 0
//	ofSetColor(0xFFFFFF);
    ofDrawBitmapString("frame: " + ofToString(fingerMovie.getCurrentFrame()) + "/"+ofToString(fingerMovie.getTotalNumFrames()),20,380);
    ofDrawBitmapString("duration: " + ofToString(fingerMovie.getPosition()*fingerMovie.getDuration(),2) + "/"+ofToString(fingerMovie.getDuration(),2),20,400);
    ofDrawBitmapString("speed: " + ofToString(fingerMovie.getSpeed(),2),20,420);

    if(fingerMovie.getIsMovieDone())
    {
        ofSetColor(0xFF0000);
        ofDrawBitmapString("end of movie",20,440);
    }
#endif


    pmouseX = mouseX;
    pmouseY = mouseY;

    fbo->detach();

    //-------------------------------------
    // 2nd pass
    //-------------------------------------
//	{
//		fboFeedback->attach();
//		GLsizei ww = fboFeedback->getWidth();
//		GLsizei wh = fboFeedback->getHeight();
//		glViewport(0, 0, ww, wh);
//	}
    glViewport(0, 0, width, height);


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquationEXT(GL_FUNC_ADD_EXT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    program = shaderDof->activate();
    if (program != 0)
    {
        {
            string textureUnitName("texture0");
            GLuint location = glGetUniformLocation(program, textureUnitName.c_str());
            glUniform1i(location, 0);
        }
        {
            string textureUnitName("texture1");
            GLuint location = glGetUniformLocation(program, textureUnitName.c_str());
            glUniform1i(location, 1);
        }

        glUniform1f(glGetUniformLocation(program, "width"), width);
        glUniform1f(glGetUniformLocation(program, "height"), height);
        glUniform1f(glGetUniformLocation(program, "focalOffset"), focalOffset.get());
    }


    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fbo->getColorTextureName());

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fbo->getDepthTextureName());

    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 1);
    glVertex2f(-1, 1);
    glTexCoord2f(0, 0);
    glVertex2f(-1, -1);
    glTexCoord2f(1, 0);
    glVertex2f(1, -1);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glEnd();

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);

    shaderDof->inactivate();

//	fboFeedback->detach();

#if 0
    //-------------------------------------
    // 3rd pass
    //-------------------------------------
    glViewport(0, 0, width, height);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fboFeedback->getColorTextureName());

    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 1);
    glVertex2f(-1, 1);
    glTexCoord2f(0, 0);
    glVertex2f(-1, -1);
    glTexCoord2f(1, 0);
    glVertex2f(1, -1);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glEnd();

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
#endif

    //-------------------------------------
    // the last pass
    //-------------------------------------

    glEnable(GL_BLEND);
    glBlendEquationEXT(GL_FUNC_ADD_EXT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Draw overlay
    IntervalTimer::Status bts = beatTimer.getStatus();
    beatTimer.update(ofGetElapsedTimef());
    float btf = forceBeat ? 0.0 : beatTimer.getFraction();
    float oiaf = 1.0f - btf;
//	if (oiaf > 0.5)
//		oiaf = 1.0f;
//	else
//		oiaf = 0.0f;

    float alpha = overlayImageAlpha * oiaf;
    alpha = alpha * alpha * alpha * alpha;
    if (bts != IntervalTimer::STOPPED && alpha >= numeric_limits<float>::epsilon())
    {
//		printf("fraction: %f, alpha:%f\n", oiaf, alpha);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(-1, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (imageSourceB != 0)
        {
            unsigned char * pixels = imageSourceB->getPixels();
            if (pixels == 0)
            {
                imageSourceB->play();
                pixels = imageSourceB->getPixels();
            }

            if (pixels != 0)
            {
                int mw = imageSourceB->getWidth();
                int mh = imageSourceB->getHeight();
                int bpp = imageSourceB->getBytesPerPixel();
                glActiveTexture(GL_TEXTURE0);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureIds[nt]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
//				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

                GLenum format = GL_LUMINANCE;
                if (bpp == 1)
                    format = GL_LUMINANCE;
                else if (bpp == 2)
                    format = GL_LUMINANCE_ALPHA;
                else if (bpp == 3)
                    format = GL_RGB;
                else if (bpp == 4)
                    format = GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, format, mw, mh, 0, format, GL_UNSIGNED_BYTE, pixels);
            }
            nt++;
        }

        glEnable(GL_BLEND);
        glBlendEquationEXT(GL_FUNC_ADD_EXT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, alpha);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 1);
        glVertex2f(-1, 1);
        glTexCoord2f(0, 0);
        glVertex2f(-1, -1);
        glTexCoord2f(1, 0);
        glVertex2f(1, -1);
        glTexCoord2f(1, 1);
        glVertex2f(1, 1);
        glEnd();

        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    isShiftPressed = ofKeyShift();
    isControlPressed = ofKeyControl();
    isAltPressed = ofKeyAlt();
    
    if (isalnum(key))
        printf("[%s] key:'%c'\n", __PRETTY_FUNCTION__, (char)key);
    else
        printf("[%s] key:%d\n", __PRETTY_FUNCTION__, key);

    switch(key)
    {
    case OF_KEY_LEFT:
        if (isShiftPressed)
        {
            if (!isControlPressed)
            {
                overlayImageAlpha -= 0.1f;
                if (overlayImageAlpha < 0.0f)
                    overlayImageAlpha = 0.0f;
            }
            else
            {
                overlayImageAlpha = 0.0f;
            }
            return;
        }

        if (imageSources.size() > 0)
        {
            imageSourceIndexA -= 1;
            if (imageSourceIndexA < 0)
                imageSourceIndexA = imageSources.size() - 1;
            if (imageSourceA != 0)
                imageSourceA->stop();
            imageSourceA = imageSources[imageSourceIndexA];
            imageSourceA->play();
            printf("[%s] imageSourceIndexA:%d\n", __PRETTY_FUNCTION__, imageSourceIndexA);
        }
        return;
    case OF_KEY_RIGHT:
        if (isShiftPressed)
        {
            if (!isControlPressed)
            {
                overlayImageAlpha += 0.1f;
                if (overlayImageAlpha > 1.0f)
                    overlayImageAlpha = 1.0f;
            }
            else
            {
                overlayImageAlpha = 1.0f;
            }
            return;
        }

        if (imageSources.size() > 0)
        {
            imageSourceIndexA += 1;
            if (imageSourceIndexA >= imageSources.size())
                imageSourceIndexA = 0;
            if (imageSourceA != 0)
                imageSourceA->stop();
            imageSourceA = imageSources[imageSourceIndexA];
            imageSourceA->play();
            printf("[%s] imageSourceIndexA:%d\n", __PRETTY_FUNCTION__, imageSourceIndexA);
        }
        return;
    case OF_KEY_UP:
        if (secondaryImageSources.size() > 0)
        {
            imageSourceIndexB += 1;
            if (imageSourceIndexB >= secondaryImageSources.size())
                imageSourceIndexB = 0;
            if (imageSourceB != 0)
                imageSourceB->stop();
            imageSourceB = secondaryImageSources[imageSourceIndexB];
            imageSourceB->play();
        }
        return;
    case OF_KEY_DOWN:
        if (secondaryImageSources.size() > 0)
        {
            imageSourceIndexB -= 1;
            if (imageSourceIndexB < 0)
                imageSourceIndexB = secondaryImageSources.size() - 1;
            if (imageSourceB != 0)
                imageSourceB->stop();
            imageSourceB = secondaryImageSources[imageSourceIndexB];
            imageSourceB->play();
        }
        return;
    case OF_KEY_BACKSPACE:
        for (int i = 0; i < MIDI_PARAMS_SIZE; i++)
            midiValueOffset[i] = MIDI_UNINITIALIZED_VALUE;
        return;
    case (int)' ':
        {
            forceBeat = true;
            //resetControllableParameters();
            float now = ofGetElapsedTimef();
            beatDetector.beat(now);
            float interval = beatDetector.getInterval();
            if (interval >= numeric_limits<float>::epsilon())
            {
                beatTimer.setDuration(interval);
                beatTimer.start(now);
            }
        }
        return;
    case OF_KEY_RETURN:
        resetControllableParameters();
        return;
        break;


    case (int)'\t':
        {
            if (isAltPressed)
                loadControllableParameters(0);
            else
                saveControllableParameters(0);
        }
        return;
    case OF_KEY_ESC:
        OF_EXIT_APP(0);
        return;
    case (int)'f':
        if (isAltPressed) {
            printf("⌥+f\n");
            
            ofToggleFullscreen();
            return;
        }
    }
    
    updateControllableParametersWithKeyPressed(key);
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
    isShiftPressed = ofKeyShift();
    isControlPressed = ofKeyControl();
    isAltPressed = ofKeyAlt();

    if (key == (int)' ')
        forceBeat = false;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

#if 0
    if(!frameByframe)
    {
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        float speed = (2 * pct - 1) * 5.0f;
        fingerMovie.setSpeed(speed);
    }
#endif
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

#if 0
    if(!frameByframe)
    {
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        fingerMovie.setPosition(pct);
    }
#endif
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
#if 0
    if(!frameByframe)
    {
        fingerMovie.setPaused(true);
    }
#endif
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
#if 0
    if(!frameByframe)
    {
        fingerMovie.setPaused(false);
    }
#endif
}

//--------------------------------------------------------------
void testApp::resized(int w, int h)
{
}

static float lastTime = 0.0f;
//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels)
{
    if (planLeft == 0 || planRight == 0)
        return;
    
    if (nChannels < 2)
        return;

    float pd = peakDecay.get();

    float now = ofGetElapsedTimef();
    lastTime = now;
    
    // samples are "interleaved"
    peakLeft *= pd;
    peakRight *= pd;
    peakLeftRight *= pd;
    for (int i = 0; i < bufferSize; i++)
    {
        left[i] = input[i*nChannels];
        right[i] = input[i*nChannels+1];
        if (left[i] > peakLeft)
            peakLeft = left[i];
        if (left[i] > peakRight)
            peakRight = right[i];

        peakLeftRight = (left[i] + right[i]) * 0.5f;
    }
    fftwf_execute(planLeft);
    fftwf_execute(planRight);

    // power spectrum peak meter process
    for (int i = 0; i < bufferSize; i++)
    {
        fftPeakLeft[i] *= pd;
        fftPeakRight[i] *= pd;
        if (fftPeakLeft[i] < outLeft[i][0])
            fftPeakLeft[i] = outLeft[i][0];
        if (fftPeakRight[i] < outRight[i][0])
            fftPeakRight[i] = outRight[i][0];
    }
}

#ifdef USE_MIDI_IN
void testApp::setMidiValue(ControllableParameter* cp, int id, int value)
{
    if (id >= MIDI_PARAMS_SIZE)
        return;
#if 0
    if (midiValueOffset[id] == MIDI_UNINITIALIZED_VALUE)
        midiValueOffset[id] = cp->getPosition() - value;
    cp->setPosition(value + midiValueOffset[id]);
#else
    cp->setPosition(value);
#endif
}

void testApp::parseMidiData()
{

    list<MIDIData>::iterator itr = midiDatas.begin();

    while (itr != midiDatas.end())
    {
        int id = (*itr).id;
        int value = (*itr).value;
        itr = midiDatas.erase(itr);


        int midiId[54] =
        {
            14, 15, 16, 17, 18, 19, 20, 21, 22,
            57, 58, 59, 60, 61, 62, 63, 65, 66,
            94, 95, 96, 97, 102, 103, 104, 105, 106,

            85, 86, 87, 88, 89, 90, 91, 92, 93,
            2,   3,  4,  5,  6,  8,  9, 12, 13,
            42, 43, 50, 51, 52, 53, 54, 55, 56,
        };

        if (id == midiId[0] || id == midiId[27])
            setMidiValue(&paramPointSize, id, value);
        else if (id == midiId[1] || id == midiId[28])
            setMidiValue(&scaleX, id, value);
        else if (id == midiId[2] || id == midiId[29])
            setMidiValue(&scaleY, id, value);
        else if (id == midiId[3] || id == midiId[30])
            setMidiValue(&rotationX, id, value);
        else if (id == midiId[4] || id == midiId[31])
            setMidiValue(&rotationY, id, value);
        else if (id == midiId[5] || id == midiId[32])
            setMidiValue(&rotationCenterX, id, value);
        else if (id == midiId[6] || id == midiId[33])
            setMidiValue(&rotationCenterY, id, value);
        else if (id == midiId[7] || id == midiId[34])
            setMidiValue(&distance, id, value);
        else if (id == midiId[8] || id == midiId[35])
            setMidiValue(&displacementGain, id, value);

        else if (id == midiId[9] || id == midiId[36])
            setMidiValue(&zClip, id, value);
        else if (id == midiId[10] || id == midiId[37])
            setMidiValue(&zOffset, id, value);
        else if (id == midiId[11] || id == midiId[38])
            setMidiValue(&colorGain, id, value);
        else if (id == midiId[12] || id == midiId[39])
            setMidiValue(&textureBlend, id, value);
        else if (id == midiId[13] || id == midiId[40])
            setMidiValue(&meshNumX, id, value);
        else if (id == midiId[14] || id == midiId[41])
            setMidiValue(&meshNumY, id, value);
        else if (id == midiId[15] || id == midiId[42])
            setMidiValue(&pointSmoothMode, id, value);
        else if (id == midiId[16] || id == midiId[43])
            setMidiValue(&noiseGain, id, value);
        else if (id == midiId[17] || id == midiId[44])
            setMidiValue(&drawMode, id, value);

        else if (id == midiId[18] || id == midiId[45])
            setMidiValue(&soundGain, id, value);
        else if (id == midiId[19] || id == midiId[46])
            setMidiValue(&peakDecay, id, value);
        else if (id == midiId[20] || id == midiId[47])
            setMidiValue(&peakLumiGain, id, value);
        else if (id == midiId[21] || id == midiId[48])
            setMidiValue(&peakPointSizeGain, id, value);
        else if (id == midiId[22] || id == midiId[49])
            setMidiValue(&textureFilter, id, value);
        else if (id == midiId[23] || id == midiId[50])
            setMidiValue(&blendingMode, id, value);
        else if (id == midiId[24] || id == midiId[51])
            setMidiValue(&layerCount, id, value);
        else if (id == midiId[25] || id == midiId[52])
        {
            setMidiValue(&autoRotation, id, value);

            float max = autoRotation.getRangeMax();
            float min = autoRotation.getRangeMin();

            float current = autoRotation.get();

            autoRotationTimer.setDuration(max + 0.066f - current);
            if (fabsf(current - min) < numeric_limits<float>::epsilon())
                autoRotationTimer.pause();
            else
                autoRotationTimer.start(ofGetElapsedTimef());
        }
        else if (id == midiId[26] || id == midiId[53])
            setMidiValue(&focalOffset, id, value);

//        else if (id == midiId[27] || id == midiId[54]) //duplication
//            setMidiValue(&soundPeakGain, id, value);

#if 0

        else if (id == 2)
            setMidiValue(&zClip, id, value);
        else if (id == 3)
            setMidiValue(&colorGain, id, value);
        else if (id == 4)
            setMidiValue(&textureBlend, id, value);

        else if (id == 5)
            setMidiValue(&meshNumX, id, value);
        else if (id == 6)
            setMidiValue(&meshNumY, id, value);
        else if (id == 23)
            setMidiValue(&pointSmoothMode, id, value);

        else if (id == 8)
            setMidiValue(&noiseGain, id, value);
#endif

        else if (id == 47)
        {
            if (value > 0)
            {
                if (imageSources.size() > 0)
                {
                    imageSourceIndexA -= 1;
                    if (imageSourceIndexA < 0)
                        imageSourceIndexA = imageSources.size() - 1;
                    if (imageSourceA != 0)
                        imageSourceA->stop();
                    imageSourceA = imageSources[imageSourceIndexA];
                    imageSourceA->play();
                }

            }

        }
        else if (id == 48)
        {
            if (value > 0)
            {
                if (imageSources.size() > 0)
                {
                    imageSourceIndexA += 1;
                    if (imageSourceIndexA >= imageSources.size())
                        imageSourceIndexA = 0;
                    if (imageSourceA != 0)
                        imageSourceA->stop();
                    imageSourceA = imageSources[imageSourceIndexA];
                    imageSourceA->play();
                }
            }
        }


        else if (id == 49)
        {
            if (value > 0)
            {
                if (imageSources.size() > 0)
                {
                    imageSourceIndexB -= 1;
                    if (imageSourceIndexB < 0)
                        imageSourceIndexB = secondaryImageSources.size() - 1;
                    if (imageSourceB != 0)
                        imageSourceB->stop();
                    imageSourceB = secondaryImageSources[imageSourceIndexB];
                    imageSourceB->play();
                }

            }

        }
        else if (id == 44)
        {
            if (value > 0)
            {
                if (imageSources.size() > 0)
                {
                    imageSourceIndexB += 1;
                    if (imageSourceIndexB >= secondaryImageSources.size())
                        imageSourceIndexB = 0;
                    if (imageSourceB != 0)
                        imageSourceB->stop();
                    imageSourceB = secondaryImageSources[imageSourceIndexB];
                    imageSourceB->play();
                }
            }
        }

        else if (id == 45)
        {
            if (value > 0)
            {
                if (imageSources.size() > 0)
                {
                    if (imageSourceA != 0)
                        imageSourceA->stop();
                    imageSourceIndexA = 0;
                    imageSourceA = imageSources[imageSourceIndexA];
                    imageSourceA->play();
                }
            }
        }
        else if (id == 46)
        {
            if (value > 0)
                resetControllableParameters();
        }

        else if (id >= 23 && id <= 31)
        {
            if (value > 0)
                saveControllableParameters(id - 23 + 1);
        }
        else if (id >= 33 && id <= 41)
        {
            if (value > 0)
                loadControllableParameters(id - 33 + 1);
        }

        else if (id >= 67 && id <= 75)
        {
            if (value > 0)
                saveControllableParameters(id - 67 + 1);
        }
        else if (id >= 76 && id <= 84)
        {
            if (value > 0)
                loadControllableParameters(id - 76 + 1);
        }

        else if (id >= 107 && id <= 115)
        {
            if (value > 0)
                saveControllableParameters(id - 107 + 1);
        }
        else if (id >= 116 && id <= 124)
        {
            if (value > 0)
                loadControllableParameters(id - 116 + 1);
        }
    }
}


//void testApp::newMessage(int port, int id, int value, double timestamp)
void testApp::newMidiMessage(ofxMidiMessage& msg)
{
    printf("%s\n", (msg.toString()).c_str());

    MidiStatus status;
    int channel;		//< 1 - 16

    if (midiDatas.size() > 256)
        return;

    // FIXME check data order.
    MIDIData md = {msg.portNum, msg.pitch, msg.value, msg.deltatime};

    midiDatas.push_back(md);
}

#endif

void testApp::updateWindowParameters()
{
    width = ofGetWidth();
    if (width <= 0)
        width = 1;
    height = ofGetHeight();
    if (height <= 0)
        height = 1;
}

void testApp::fadeParameters(float fade)
{
    if (fade < 0.0f)
        fade = 0.0f;
    else if (fade > 1.0f)
        fade = 1.0f;

    // skip
    int32_t bm = blendingMode.getPosition();
    int32_t dm = drawMode.getPosition();
    int32_t tb = textureBlend.getPosition();

    if (activePresetParamIndexA >= 0 && activePresetParamIndexB >= 0)
    {
        ControllableParameter* cpOrig[] =
        {
            &paramPointSize,
            &scaleX,
            &scaleY,
            &rotationX,
            &rotationY,
            &rotationCenterX,
            &rotationCenterY,
            &distance,
            &displacementGain,
            &zClip,
            &zOffset,
            &colorGain,
            &textureBlend,
            &meshNumX,
            &meshNumY,
            &pointSmoothMode,
            &noiseGain,
            &drawMode,
            &soundGain,
            &peakDecay,
            &peakLumiGain,
            &peakPointSizeGain,
            &textureFilter,
            &blendingMode,
            &layerCount,
            &autoRotation,
            &focalOffset,
            &feedbackGain,
            &soundPeakGain,
        };


        ControllableParameter* cpA = presetParams[activePresetParamIndexA].params;
        ControllableParameter* cpB = presetParams[activePresetParamIndexB].params;
        for (int count = 0; count < PARAMETER_COUNT; count++)
        {
            cpOrig[count]->setPosition((int32_t)(cpA[count].getPosition() * fade + cpB[count].getPosition() * (1.0f - fade) + 0.5f));
        }

    }
    // skip
    blendingMode.setPosition(bm);
    drawMode.setPosition(dm);
    textureBlend.setPosition(tb);

    lastParameterFade = fade;
}

namespace
{
bool CheckOSCArgs(const ofxOscMessage& msg, const string& addr, const string& fmt)
{
    if (addr != msg.getAddress())
        return false;

    if (fmt.size() > msg.getNumArgs())
        return false;


    for (int n = 0; n < fmt.size(); n++)
    {
        ofxOscArgType type = msg.getArgType(n);
        char c = fmt.at(n);
        if (c == 'i')
        {
            if (OFXOSC_TYPE_INT32 != type)
                return false;
            printf("\targ[%d]=%d\n", n, msg.getArgAsInt32(n));
        }
        else if (c == 'f')
        {
            if (OFXOSC_TYPE_FLOAT != type)
                return false;
            printf("\targ[%d]=%f\n", n, msg.getArgAsFloat(n));
        }
        else if (c == 's')
        {
            if (OFXOSC_TYPE_STRING != type)
                return false;
            printf("\targ[%d]=%s\n", n, msg.getArgAsString(n).c_str());
        }
    }
    return true;
}
}

void testApp::updateOscMessage()
{
    // check for waiting messages
    while( receiver.hasWaitingMessages() )
    {
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage( &m );

        printf("OSC: address=%s numArgs=%d\n", m.getAddress().c_str(), m.getNumArgs());

        if (CheckOSCArgs(m, "/pointSize", "f"))
        {
            paramPointSize.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/scaleXY", "ff"))
        {
            scaleX.setPosition((int)m.getArgAsFloat(1));
            scaleY.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/rotateXY", "ff"))
        {
            printf("OSC: address=%s numArgs=%d value=%f value=%f\n", m.getAddress().c_str(), m.getNumArgs(), m.getArgAsFloat(0), m.getArgAsFloat(1));

            rotationX.setPosition((int)(m.getArgAsFloat(0)));// * rotationCenterX.getPositionMax()));
            rotationY.setPosition((int)(m.getArgAsFloat(1)));// * rotationCenterX.getPositionMax()));
        }
        else if (CheckOSCArgs(m, "/centerXY", "ff"))
        {
            printf("OSC: address=%s numArgs=%d value=%f value=%f\n", m.getAddress().c_str(), m.getNumArgs(), m.getArgAsFloat(0), m.getArgAsFloat(1));
            rotationCenterX.setPosition((int)m.getArgAsFloat(1));
            rotationCenterY.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/distance", "f"))
        {
            distance.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/deform", "f"))
        {
            displacementGain.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/Zclip", "f"))
        {
            zClip.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/Zoffset", "f"))
        {
            zOffset.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/colorScale", "f"))
        {
            colorGain.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/ABfade", "f"))
        {
            textureBlend.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/meshXY", "ff"))
        {
            meshNumX.setPosition((int)m.getArgAsFloat(0));
            meshNumY.setPosition((int)m.getArgAsFloat(1));
        }
        else if (CheckOSCArgs(m, "/pointSmooth", "f"))
        {
            pointSmoothMode.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/noisefade", "f"))
        {
            noiseFeedback = m.getArgAsFloat(0) / 127.0f;
        }
        else if (CheckOSCArgs(m, "/drawType", "f"))
        {
            drawMode.setPosition((int)(m.getArgAsFloat(0)));
        }
        else if (CheckOSCArgs(m, "/soundScale", "f"))
        {
            soundGain.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/sound_gensui", "f"))
        {
            peakDecay.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/sound_kido", "f"))
        {
            peakLumiGain.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/sound2dot", "f"))
        {
            peakPointSizeGain.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/textureFilter", "f"))
        {
            textureFilter.setPosition((int)m.getArgAsFloat(0));
        }

//		else if (CheckOSCArgs(m, "/blendingMode", "f"))
        else if (CheckOSCArgs(m, "/brendingMode", "f"))
        {
            blendingMode.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/layer", "f"))
        {
            layerCount.setPosition(m.getArgAsFloat(0) < 64.0f ? 0 : layerCount.getPositionMax());
        }
        else if (CheckOSCArgs(m, "/auto_rotate", "f"))
        {
            autoRotation.setPosition((int)m.getArgAsFloat(0));
            float max = autoRotation.getRangeMax();
            float min = autoRotation.getRangeMin();
            float current = autoRotation.get();
            autoRotationTimer.setDuration(max + 0.066f - current);
            if (fabsf(current - min) < numeric_limits<float>::epsilon())
                autoRotationTimer.pause();
            else
                autoRotationTimer.start(ofGetElapsedTimef());

        }
        else if (CheckOSCArgs(m, "/depthField", "f"))
        {
            focalOffset.setPosition((int)m.getArgAsFloat(0));
        }
        else if (CheckOSCArgs(m, "/noise_volume", "f"))
        {
            noiseGain.setPosition((int)m.getArgAsFloat(0));
        }

        else if (CheckOSCArgs(m, "/timming", "f"))
        {
            if (m.getArgAsFloat(0) > 63.0f)
            {
                printf("forceBeat\n");
                forceBeat = true;
                //resetControllableParameters();
                float now = ofGetElapsedTimef();
                beatDetector.beat(now);
                float interval = beatDetector.getInterval();
                if (interval >= numeric_limits<float>::epsilon())
                {
                    beatTimer.setDuration(interval);
                    beatTimer.start(now);
                }
            }
            else
            {
                forceBeat = false;
            }

        }
        else if (CheckOSCArgs(m, "/base_sorce_next", "f"))
        {
            if (m.getArgAsFloat(0) > 63.0f && imageSources.size() > 0)
            {
                imageSourceIndexA += 1;
                if (imageSourceIndexA >= imageSources.size())
                    imageSourceIndexA = 0;
                if (imageSourceA != 0)
                    imageSourceA->stop();
                imageSourceA = imageSources[imageSourceIndexA];
                imageSourceA->play();
            }
        }
        else if (CheckOSCArgs(m, "/base_sorce_prev", "f"))
        {
            if (m.getArgAsFloat(0) > 63.0f && imageSources.size() > 0)
            {
                imageSourceIndexA -= 1;
                if (imageSourceIndexA < 0)
                    imageSourceIndexA = imageSources.size() - 1;
                if (imageSourceA != 0)
                    imageSourceA->stop();
                imageSourceA = imageSources[imageSourceIndexA];
                imageSourceA->play();
            }
        }
        else if (CheckOSCArgs(m, "/overlay_sorce_next", "f"))
        {
            if (m.getArgAsFloat(0) > 63.0f && secondaryImageSources.size() > 0)
            {
                imageSourceIndexB += 1;
                if (imageSourceIndexB >= secondaryImageSources.size())
                    imageSourceIndexB = 0;
                if (imageSourceB != 0)
                    imageSourceB->stop();
                imageSourceB = secondaryImageSources[imageSourceIndexB];
                imageSourceB->play();
            }
        }
        else if (CheckOSCArgs(m, "/overlay_sorce_prev", "f"))
        {
            if (m.getArgAsFloat(0) > 63.0f && secondaryImageSources.size() > 0)
            {
                imageSourceIndexB -= 1;
                if (imageSourceIndexB < 0)
                    imageSourceIndexB = secondaryImageSources.size() - 1;
                if (imageSourceB != 0)
                    imageSourceB->stop();
                imageSourceB = secondaryImageSources[imageSourceIndexB];
                imageSourceB->play();
            }
        }
        else if (CheckOSCArgs(m, "/base2overlay_fade", "f"))
        {
            overlayImageAlpha = m.getArgAsFloat(0) / 127.0f;
        }
        else if (CheckOSCArgs(m, "/allReset", "f"))
        {
            resetControllableParameters();
        }
        else if (CheckOSCArgs(m, "/prefade", "f"))
        {
            float fade = m.getArgAsFloat(0) / 127.0f;
            fadeParameters(fade);
        }
        else if (CheckOSCArgs(m, "/soundPeakGain", "f"))
        {
            soundPeakGain.setPosition((int)m.getArgAsFloat(0));
        }
        else
        {
            for (int i = 0; i < PRESET_PARAMETER_COUNT; i++)
            {
                string num(ofToString(i + 1));
                string rec("/rec" + num);
                string preset("/preset" + num);
                if (CheckOSCArgs(m, rec, "f"))
                {
                    if (m.getArgAsFloat(0) > 63.0f)
                        saveControllableParameters(i);
//					printf("Save: %d\n", i);
                }
                else if (CheckOSCArgs(m, preset, "f"))
                {
                    if (m.getArgAsFloat(0) > 63.0f)
                        setNextControllableParameters(i);
//					printf("Next: %d\n", i);
                }
            }
        }

//		else if (CheckOSCArgs(m, "/noise_volume", "f"))
//		{
//			noiseGain.setPosition((int)m.getArgAsFloat(0));
//		}




    }

}


void testApp::updateControllableParametersWithKeyPressed(int key)
{
    if (key == (int)'q')
        IncrementPosition(&paramPointSize);
    else if (key == (int)'Q')
        DecrementPosition(&paramPointSize);
    
    if (key == (int)'w')
        IncrementPosition(&scaleX);
    else if (key == (int)'W')
        DecrementPosition(&scaleX);


    if (key == (int)'e')
        IncrementPosition(&scaleY);
    else if (key == (int)'E')
        DecrementPosition(&scaleY);

    if (key == (int)'r')
        IncrementPosition(&rotationX);
    else if (key == (int)'R')
        DecrementPosition(&rotationX);

    if (key == (int)'t')
        IncrementPosition(&rotationY);
    else if (key == (int)'T')
        DecrementPosition(&rotationY);

    if (key == (int)'y')
        IncrementPosition(&rotationCenterX);
    else if (key == (int)'Y')
        DecrementPosition(&rotationCenterX);

    if (key == (int)'u')
        IncrementPosition(&rotationCenterY);
    else if (key == (int)'U')
        DecrementPosition(&rotationCenterY);

    if (key == (int)'i')
        IncrementPosition(&distance);
    else if (key == (int)'I')
        DecrementPosition(&distance);

    if (key == (int)'o')
        IncrementPosition(&displacementGain);
    else if (key == (int)'O')
        DecrementPosition(&displacementGain);

    if (key == (int)'p')
        IncrementPosition(&zClip);
    else if (key == (int)'P')
        DecrementPosition(&zClip);

    if (key == (int)'a')
        IncrementPosition(&zOffset);
    else if (key == (int)'A')
        DecrementPosition(&zOffset);

    if (key == (int)'s')
        IncrementPosition(&colorGain);
    else if (key == (int)'S')
        DecrementPosition(&colorGain);

    if (key == (int)'d')
        IncrementPosition(&textureBlend);
    else if (key == (int)'D')
        DecrementPosition(&textureBlend);

    if (key == (int)'f')
        IncrementPosition(&meshNumX);
    else if (key == (int)'F')
        DecrementPosition(&meshNumX);

    if (key == (int)'g')
        IncrementPosition(&meshNumY);
    else if (key == (int)'G')
        DecrementPosition(&meshNumY);

    if (key == (int)'h')
        IncrementPosition(&pointSmoothMode);
    else if (key == (int)'H')
        DecrementPosition(&pointSmoothMode);

    if (key == (int)'j')
        IncrementPosition(&noiseGain);
    else if (key == (int)'J')
        DecrementPosition(&noiseGain);

    if (key == (int)'k')
        IncrementPosition(&drawMode);
    else if (key == (int)'K')
        DecrementPosition(&drawMode);

    if (key == (int)'l')
        IncrementPosition(&soundGain);
    else if (key == (int)'L')
        DecrementPosition(&soundGain);

    if (key == (int)'z')
        IncrementPosition(&peakDecay);
    else if (key == (int)'Z')
        DecrementPosition(&peakDecay);

    if (key == (int)'x')
        IncrementPosition(&peakLumiGain);
    else if (key == (int)'X')
        DecrementPosition(&peakLumiGain);

    if (key == (int)'c')
        IncrementPosition(&peakPointSizeGain);
    else if (key == (int)'C')
        DecrementPosition(&peakPointSizeGain);

    if (key == (int)'v')
        IncrementPosition(&textureFilter);
    else if (key == (int)'V')
        DecrementPosition(&textureFilter);

    if (key == (int)'b')
        IncrementPosition(&blendingMode);
    else if (key == (int)'B')
        DecrementPosition(&blendingMode);

    if (key == (int)'n')
        IncrementPosition(&layerCount);
    else if (key == (int)'N')
        DecrementPosition(&layerCount);

    if (key == (int)'m' || key == (int)'M')
    {
        if (key == (int)'m')
            IncrementPosition(&autoRotation);
        else if (key == (int)'M')
            DecrementPosition(&autoRotation);
        float max = autoRotation.getRangeMax();
        float min = autoRotation.getRangeMin();
        float current = autoRotation.get();
        autoRotationTimer.setDuration(max + 0.066f - current);
        if (fabsf(current - min) < numeric_limits<float>::epsilon())
            autoRotationTimer.pause();
        else
            autoRotationTimer.start(ofGetElapsedTimef());
    }


    if (key == (int)',')
        IncrementPosition(&focalOffset);
    else if (key == (int)'<')
        DecrementPosition(&focalOffset);

    if (key == (int)'.')
        IncrementPosition(&feedbackGain);
    else if (key == (int)'>')
        DecrementPosition(&feedbackGain);
    
    if (key == (int)'/')
        IncrementPosition(&soundPeakGain);
    else if (key == (int)'?')
        DecrementPosition(&soundPeakGain);
}

void testApp::resetControllableParameters()
{
    GLint pr[2];
    glGetIntegerv(GL_SMOOTH_POINT_SIZE_RANGE, pr);
    paramPointSize.setup(1, pr[1] / 2, 2, 128);
    scaleX.setup(0.1f, 4.0f, 1, 128);
    scaleY.setup(0.1f, 4.0f, 1, 128);
    rotationX.setup(-90, 90, 0, 128);
    rotationY.setup(-90, 90, 0, 128);
    rotationCenterX.setup(-2, 2, 0, 128);
    rotationCenterY.setup(-2, 2, 0, 128);
    distance.setup(0.9f, 5, 1.2f, 128);
    displacementGain.setup(0, 5, 1, 128);
    zClip.setup(-2, 2, 0, 128);
    zOffset.setup(-1, 1, 0, 128);
    colorGain.setup(0, 5, 1, 128);
    textureBlend.setup(0, 1, 0.5f, 128);
    meshNumX.setup(1, 317, 200, 128);
    meshNumY.setup(1, 317, 200, 128);
    pointSmoothMode.setup(0, 2, 2, 128);
    noiseGain.setup(0, 1, 0, 128);
    drawMode.setup(0, 2, 0, 128);
    soundGain.setup(0, 1, 0.0f, 128);
    peakDecay.setup(PEAK_DECAY_MIN, PEAK_DECAY_MAX, PEAK_DECAY_DEFAULT, 128);
    peakLumiGain.setup(0, 2, 0, 128);
    peakPointSizeGain.setup(0, 64, 0, 128);
    textureFilter.setup(0, 1, 1, 128);
    blendingMode.setup(0, 6, 6, 128);
    layerCount.setup(1, 2, 1, 128);
    autoRotation.setup(0, 30, 0, 128);
    focalOffset.setup(-1, 1, 0.9f, 128);
    feedbackGain.setup(0, 1, 0.0f, 128);
    soundPeakGain.setup(0, 4, 0.0f, 128);

    float max = autoRotation.getRangeMax();
    float min = autoRotation.getRangeMin();
    float current = autoRotation.get();
    autoRotationTimer.setDuration(max + 0.066f - current);
    float nn = current - min;
    if (fabsf(nn) < numeric_limits<float>::epsilon())
        autoRotationTimer.stop();
    else
        autoRotationTimer.start(ofGetElapsedTimef());

    autoRotationTimer.stop();

#if 0

    ControllableParameter* cpOrig[] =
    {
        &paramPointSize,
        &scaleX,
        &scaleY,
        &rotationX,
        &rotationY,
        &rotationCenterX,
        &rotationCenterY,
        &distance,
        &displacementGain,
        &zClip,
        &zOffset,
        &colorGain,
        &textureBlend,
        &meshNumX,
        &meshNumY,
        &pointSmoothMode,
        &noiseGain,
        &drawMode,
        &soundGain,
        &peakDecay,
        &peakLumiGain,
        &peakPointSizeGain,
        &textureFilter,
        &blendingMode,
        &layerCount,
        &autoRotation,
        &focalOffset,
        &feedbackGain,
        &soundPeakGain
    };


    autoRotationTimer.stop();
    for (int index = 0; index < PRESET_PARAMETER_COUNT; index++)
    {
        if (!loadControllableParameters(index))
        {
            ControllableParameter* cp = presetParams[index].params;
            for (int count = 0; count < PARAMETER_COUNT; count++)
            {
                cp[count] = *cpOrig[count];
            }
            saveControllableParameters(index);

        }
    }
    activePresetParamIndexA = 0;
    activePresetParamIndexB = 0;
#endif


    fadeParameters(lastParameterFade);
}

bool testApp::loadControllableParameters(int index)
{
    for (int i = 0; i < MIDI_PARAMS_SIZE; i++)
        midiValueOffset[i] = MIDI_UNINITIALIZED_VALUE;
    /*
    	ControllableParameter* cp[] = {
    		&paramPointSize,
    		&scaleX,
    		&scaleY,
    		&rotationX,
    		&rotationY,
    		&rotationCenterX,
    		&rotationCenterY,
    		&distance,
    		&displacementGain,
    		&zClip,
    		&zOffset,
    		&colorGain,
    		&textureBlend,
    		&meshNumX,
    		&meshNumY,
    		&pointSmoothMode,
    		&noiseGain,
    		&drawMode,
    		&soundGain,
    		&peakDecay,
    		&peakLumiGain,
    		&peakPointSizeGain,
    		&textureFilter,
    		&blendingMode,
    		&layerCount,
    		&autoRotation,
    		&focalOffset,
    		&feedbackGain,
            &soundPeakGain
    	};
    */

    if (index < 0 || index >= PRESET_PARAMETER_COUNT)
    {
        return false;
    }

    ControllableParameter* cp = presetParams[index].params;

    string dataDir = GetDataDirectory();
    string presetFilePath = dataDir + "/preset" + ofToString(index);

    FILE* fp = fopen(presetFilePath.c_str(), "r");
    size_t count = 0;
    if (fp != NULL)
    {
        for (count = 0; count < PARAMETER_COUNT; count++)
        {
            if (!cp[count].deserialize(fp))
                break;
        }
        fclose(fp);
        fp = NULL;
    }
    else
    {
        return false;
    }

    if (count < PARAMETER_COUNT)
    {
        // refresh
        saveControllableParameters(index);
    }
    presetParams[index].isAvailable = true;
    return true;
}

void testApp::saveControllableParameters(int index)
{
    ControllableParameter* cp[] =
    {
        &paramPointSize,
        &scaleX,
        &scaleY,
        &rotationX,
        &rotationY,
        &rotationCenterX,
        &rotationCenterY,
        &distance,
        &displacementGain,
        &zClip,
        &zOffset,
        &colorGain,
        &textureBlend,
        &meshNumX,
        &meshNumY,
        &pointSmoothMode,
        &noiseGain,
        &drawMode,
        &soundGain,
        &peakDecay,
        &peakLumiGain,
        &peakPointSizeGain,
        &textureFilter,
        &blendingMode,
        &layerCount,
        &autoRotation,
        &focalOffset,
        &feedbackGain,
        &soundPeakGain,
    };
    if (index < 0 || index >= PRESET_PARAMETER_COUNT)
    {
        return;
    }

//	ControllableParameter* cp = presetParams[index].params;

    string dataDir = GetDataDirectory();
    string presetFilePath = dataDir + "/preset" + ofToString(index);
    size_t count = 0;

    FILE* fp;
    fp = fopen(presetFilePath.c_str(), "w");
    if (fp != NULL)
    {
        for (count = 0; count < PARAMETER_COUNT; count++)
        {
            if (!cp[count]->serialize(fp))
                return;
        }
        fclose(fp);
    }

    presetParams[index].isAvailable = true;
}


void testApp::setNextControllableParameters(int index)
{
    if (!loadControllableParameters(index))
        return;

    activePresetParamIndexA = activePresetParamIndexB;
    activePresetParamIndexB = index;
    fadeParameters(0);
}
