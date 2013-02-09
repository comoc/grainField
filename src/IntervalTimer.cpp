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
 *  IntervalTimer.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/09/02.
 *
 */

#include "IntervalTimer.h"
#include <limits>
#include <cmath>

using namespace std;

IntervalTimer::IntervalTimer()
    : duration(0.0f)
    , startTime(0.0f)
    , fraction(0.0f)
    , isLoop(true)
    , status(STOPPED)
{
}

IntervalTimer::~IntervalTimer()
{
}

void IntervalTimer::setDuration(float sec)
{
    duration = sec;
    if (duration < 0.0f)
        duration = 0.0f;
}

float IntervalTimer::getDuration()
{
    return duration;
}

void IntervalTimer::setLoop(bool isLoop)
{
    this->isLoop = isLoop;
}

void IntervalTimer::start(float sec)
{
    if (status == STOPPED)
    {
        fraction = 0.0f;
        startTime = sec;
    }
    else
    {
        if (status == PAUSED)
            startTime = sec - fraction * duration;
        else
        {
            if (duration >= numeric_limits<float>::epsilon())
            {
#if 0
                float f = (sec - startTime) / duration;
                f = f - floorf(f);
                fraction = f;
                startTime = sec - f * duration;
#else
                startTime = sec - fraction * duration;
#endif
            }
            else
                startTime = sec;
        }
    }

    status = STARTED;
}

void IntervalTimer::stop()
{
    status = STOPPED;
    fraction = 0.0f;
}

void IntervalTimer::pause()
{
    status = PAUSED;
}

void IntervalTimer::update(float sec)
{
    if (status == STOPPED || status == PAUSED)
        return;

    if (duration < numeric_limits<float>::epsilon())
        fraction = 0.0f;
    else
    {
        fraction = (sec - startTime) / duration;
        if (isLoop)
            fraction = fraction - floorf(fraction);
        else
        {
            if (fraction > 1.0f)
                fraction = 1.0f;
        }
    }
}

float IntervalTimer::getFraction()
{
    return fraction;
}

IntervalTimer::Status IntervalTimer::getStatus()
{
    return status;
}


