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
