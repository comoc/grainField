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
 *  MultiTouch.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 11/07/04.
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
