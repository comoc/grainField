/*
 *  MultiTouch.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 11/07/04.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "MultiTouch.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
	
Finger gFingers[MAX_FINGERS];
int gFingerCount = 0;
	
static pthread_mutex_t _gMutex = PTHREAD_MUTEX_INITIALIZER;
	
typedef void *MTDeviceRef;
typedef int (*MTContactCallbackFunction)(int,Finger*,int,double,int);

MTDeviceRef MTDeviceCreateDefault();
void MTRegisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTDeviceStart(MTDeviceRef, int); // thanks comex

int callback(int device, Finger *data, int nFingers, double timestamp, int frame) {
	pthread_mutex_lock(&_gMutex);
	
	for (int i=0; i<nFingers; i++) {
		
		if (i < MAX_FINGERS) {
			gFingerCount = i + 1;
			memcpy(&gFingers[i], &data[i], sizeof(Finger));
		}
		
//		Finger *f = &data[i];
//		printf("Frame %7d: Angle %6.2f, ellipse %6.3f x%6.3f; "
//			   "position (%6.3f,%6.3f) vel (%6.3f,%6.3f) "
//			   "ID %d, state %d [%d %d?] size %6.3f, %6.3f?\n",
//			   f->frame,
//			   f->angle * 90 / atan2(1,0),
//			   f->majorAxis,
//			   f->minorAxis,
//			   f->normalized.pos.x,
//			   f->normalized.pos.y,
//			   f->normalized.vel.x,
//			   f->normalized.vel.y,
//			   f->identifier, f->state, f->foo3, f->foo4,
//			   f->size, f->unk2);
	}
//	printf("\n");
	pthread_mutex_unlock(&_gMutex);
	return 0;
}

void InitMultiTouch(void) {
	MTDeviceRef dev = MTDeviceCreateDefault();
	MTRegisterContactFrameCallback(dev, callback);
	MTDeviceStart(dev, 0);
}

void UpdateMultiTouch(void) {
}

	
#ifdef __cplusplus
}
#endif