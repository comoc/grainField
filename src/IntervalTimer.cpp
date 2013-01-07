/*
 *  IntervalTimer.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/09/02.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
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


