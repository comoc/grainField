/*
 *  IntervalTimer.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/09/02.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
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
	enum Status {
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