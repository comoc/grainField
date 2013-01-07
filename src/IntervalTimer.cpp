/*
 
 Copyright (c) 2013, Akihiro Komori
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the developer nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY DAMIAN STEWART ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL DAMIAN STEWART BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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


