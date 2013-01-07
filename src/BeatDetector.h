/*
 *  BeatDetector.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/10/08.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BEATDETECTOR_H
#define _BEATDETECTOR_H

class BeatDetector
{
public:
	BeatDetector();
	~BeatDetector();
	void beat(float time);
	float getInterval();
	void reset();
private:
	bool isInit;
	float timeOld;
	float interval;
};

#endif
