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
 *  IntervalTimer.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/09/02.
 *
 */

#ifndef _INTERVALTIMER_H
#define _INTERVALTIMER_H

class IntervalTimer
{
public:
    IntervalTimer();
    ~IntervalTimer();
    void setDuration(float sec);
    float getDuration();
    void setLoop(bool isLoop);
    void start(float sec);
    void stop();
    void pause();
    void update(float sec);
    float getFraction();
    enum Status
    {
        STARTED,
        STOPPED,
        PAUSED,
    };
    Status getStatus();
private:
    float duration;
    float startTime;
    float fraction;
    bool isLoop;
    Status status;
};

#endif