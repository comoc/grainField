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
 *  ControllableParameter.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/17.
 *
 */

#include "ControllableParameter.h"

#include <cmath>
#include <cfloat>
#include <stdint.h>

using namespace std;

ControllableParameter::ControllableParameter()
{
	setup(0, 1, 0, 1);
}

ControllableParameter::ControllableParameter(float min, float max, float initPosition, int32_t ticks)
{
	setup(min, max, initPosition, ticks);
}

void ControllableParameter::setup(float min, float max, float initPosition, int ticks)
{
	if (fabsf(max - min) < FLT_EPSILON)
		max = min + FLT_EPSILON;
	else if (max < min)
		max = min + FLT_EPSILON;
	
	if (ticks < 1)
		ticks = 1;
	
	this->min = min;
	this->max = max;
	this->maxPos = ticks - 1;
	
	if (initPosition > max)
	{
		initPosition = max;
		this->pos = this->maxPos;
	}
	else if (initPosition < min)
	{
		initPosition = min;
		this->pos = 0;
	}
	else
		this->pos = (int32_t)((initPosition - min) * maxPos / (max - min) + 0.5f);
	
	this->p = initPosition;
}

void ControllableParameter::up()
{
	setPosition(pos + 1);
}

void ControllableParameter::down()
{
	setPosition(pos - 1);
}

void ControllableParameter::setPosition(int32_t pos)
{
	if (pos > maxPos)
		pos = maxPos;
	else if (pos < 0)
		pos = 0;
	
	this->pos = pos;
	
	calc();
}

int32_t ControllableParameter::getPosition()
{
	return pos;
}

int32_t ControllableParameter::getPositionMax()
{
	return maxPos;
}

void ControllableParameter::calc()
{
	if (maxPos > 0)
		p = min + (max - min) * ((float)pos / (float)maxPos);
	else
		p = min;
}

float ControllableParameter::get()
{
	return p;
}

void ControllableParameter::setFraction(float f)
{
	if (f < 0.0f)
		f = 0.0f;
	else if (f > 1.0f)
		f = 1.0f;
	setPosition((int32_t)(f * maxPos + 0.5f));
}

bool ControllableParameter::serialize(FILE* fp)
{
/*
 float p;
 float min;
 float max;
 int maxPos;
 int pos;
*/

	if (fwrite(&p, sizeof(float), 1, fp) < 1)
		return false;
	if (fwrite(&min, sizeof(float), 1, fp) < 1)
		return false;
	if (fwrite(&max, sizeof(float), 1, fp) < 1)
		return false;
	if (fwrite(&maxPos, sizeof(int32_t), 1, fp) < 1)
		return false;
	if (fwrite(&pos, sizeof(int32_t), 1, fp) < 1)
		return false;
	return true;
}


bool ControllableParameter::deserialize(FILE* fp)
{
	if (fread(&p, sizeof(float), 1, fp) < 1)
		return false;
	if (fread(&min, sizeof(float), 1, fp) < 1)
		return false;
	if (fread(&max, sizeof(float), 1, fp) < 1)
		return false;
	if (fread(&maxPos, sizeof(int32_t), 1, fp) < 1)
		return false;
	if (fread(&pos, sizeof(int32_t), 1, fp) < 1)
		return false;
	return true;
}

float ControllableParameter::getRangeMin()
{
	return min;
}

float ControllableParameter::getRangeMax()
{
	return max;
}



