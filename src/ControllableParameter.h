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
 *  ControllableParameter.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/16.
 *
 */

#ifndef _CONTROLLABLEPARAMETER_H
#define _CONTROLLABLEPARAMETER_H

#include <stdint.h>
#include <cstdio>

class ControllableParameter
{
public:
	enum Transition {
		
		CONSTANT,
		
		LINEAR,
		
		EASE_IN_SINE,
		EASE_OUT_SINE,
		EASE_IN_OUT_SINE,
		
		EASE_IN_CUBIC,
		EASE_OUT_CUBIC,
		EASE_IN_OUT_CUBIC,
		
		EASE_IN_QUINT,
		EASE_OUT_QUINT,
		EASE_IN_OUT_QUINT,
		
		EASE_IN_CIRC,
		EASE_OUT_CIRC,
		EASE_IN_OUT_CIRC,
		
		EASE_IN_BACK,
		EASE_OUT_BACK,
		EASE_IN_OUT_BACK,
		
		EASE_IN_QUAD,
		EASE_OUT_QUAD,
		EASE_IN_OUT_QUAD,
		
		EASE_IN_QUART,
		EASE_OUT_QUART,
		EASE_IN_OUT_QUART,
		
		EASE_IN_EXPO,
		EASE_OUT_EXPO,
		EASE_IN_OUT_EXPO,
		
		EASE_IN_ELASTIC,
		EASE_OUT_ELASTIC,
		EASE_IN_OUT_ELASTIC,
		
		EASE_IN_BOUNCE,
		EASE_OUT_BOUNCE,
		EASE_IN_OUT_BOUNCE,
	};
	
public:
	ControllableParameter();
	ControllableParameter(float min, float max, float initPosition, int32_t ticks);
	
	void setup(float min, float max, float initPosition, int32_t ticks);
	
	void up();
	
	void down();
	
	void setPosition(int32_t pos);
	void setPositionWithTween(int32_t pos, Transition t, float now, float duration, float delay);

	int32_t getPosition();
	int32_t getPositionMax();

	void calc();
	
	float get();
		
	void setFraction(float f);
	void setFractionWithTween(float f, Transition t, float now, float duration, float delay);
	
	bool serialize(std::FILE* fp);
	
	bool deserialize(std::FILE* fp);
	
	float getRangeMin();
	float getRangeMax();
	
private:
	float p;
	float min;
	float max;
	int32_t maxPos;
	int32_t pos;
};

#endif
