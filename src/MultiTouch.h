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
