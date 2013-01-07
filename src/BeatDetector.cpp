/*
 *  BeatDetector.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/10/08.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "BeatDetector.h"

BeatDetector::BeatDetector()
: isInit(true)
, timeOld(0.0f)
, interval(0.0f)
{
}

BeatDetector::~BeatDetector()
{
}

void BeatDetector::beat(float time)
{
	if (!isInit)
		interval = time - timeOld;
	else
		isInit = false;
	timeOld = time;
}

float BeatDetector::getInterval()
{
	return interval;
}

void BeatDetector::reset()
{
	isInit = true;
	timeOld = 0.0f;
	interval = 0.0f;
}
