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



