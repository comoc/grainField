/*
 *  MultiTouch.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 11/07/04.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MultiTouch_h_
#define _MultiTouch_h_

#include <math.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	typedef struct { float x,y; } mtPoint;
	typedef struct { mtPoint pos,vel; } mtReadout;
	
	typedef struct {
		int frame;
		double timestamp;
		int identifier, state, foo3, foo4;
		mtReadout normalized;
		float size;
		int zero1;
		float angle, majorAxis, minorAxis; // ellipsoid
		mtReadout mm;
		int zero2[2];
		float unk2;
	} Finger;

	enum {
		MAX_FINGERS =  10,
	};
	
	extern Finger gFingers[MAX_FINGERS];
	extern int gFingerCount;
	
	void InitMultiTouch(void);
	void UpdateMultiTouch(void);

#ifdef __cplusplus
}
#endif

#endif
