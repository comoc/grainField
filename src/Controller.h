/*
 *  Controller.h
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/17.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "ControllableParameter.h"
#include "json/json.h"
#include <map>
#include <string>

class Controller
{
public:
	Controller();
	~Controller();
	bool initialize(const char* path);

private:
	std::map<std::string, ControllableParameter*> cps;
	std::string ip;
};

#endif
