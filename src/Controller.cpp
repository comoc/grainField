/*
 *  Controller.cpp
 *  GrainField
 *
 *  Created by Akihiro Komori on 10/07/17.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Controller.h"
#include <cstdio>
#include <cstdlib>

using namespace std;

Controller::Controller()
{
}

Controller::~Controller()
{
}

bool Controller::initialize(const char* path)
{
	FILE* fp = fopen(path, "r");
	if (fp == 0)
		return false;
	
	fseek(fp,  0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp,  0L, SEEK_SET);
	char* input = (char*)malloc(size);
	fread(input, size, 1, fp);
	fclose(fp);
	
	bool ret = true;
	struct json_object *jobj;
	jobj = json_tokener_parse(input);
	if (is_error(jobj)) {
		printf("error parsing json: %s\n",
			   json_tokener_errors[-(unsigned long)jobj]);
		ret = false;
	}
	else
	{
		json_object_object_foreach(jobj, key, val)
		{
			
			if (json_object_get_type(val) == json_type_null)
				printf("\t%s: json_type_null: %s\n", key, json_object_to_json_string(val));
			else if (json_object_get_type(val) == json_type_boolean)
				printf("\t%s: json_type_boolean: %s\n", key, json_object_to_json_string(val));
			else if (json_object_get_type(val) == json_type_double)
				printf("\t%s: json_type_double: %s\n", key, json_object_to_json_string(val));
			else if (json_object_get_type(val) == json_type_int)
				printf("\t%s: json_type_int: %s\n", key, json_object_to_json_string(val));
			else if (json_object_get_type(val) == json_type_object)
				printf("\t%s: json_type_object: %s\n", key, json_object_to_json_string(val));
			else if (json_object_get_type(val) == json_type_array)
			{				
				printf("\t%s: json_type_array: %s\n", key, json_object_to_json_string(val));
				for(int i = 0; i < json_object_array_length(val); i++) {
					struct json_object *item = json_object_array_get_idx(val, i);
					printf("\titem[%d]: %s\n", i, json_object_to_json_string(item));
					
					if (json_object_get_type(item) == json_type_null)
						printf("\t\tjson_type_null: %s\n", json_object_to_json_string(item));
					else if (json_object_get_type(item) == json_type_boolean)
						printf("\t\tjson_type_boolean: %s\n", json_object_to_json_string(item));
					else if (json_object_get_type(item) == json_type_double)
						printf("\t\tjson_type_double: %s\n", json_object_to_json_string(item));
					else if (json_object_get_type(item) == json_type_int)
						printf("\t\tjson_type_int: %s\n", json_object_to_json_string(item));
					else if (json_object_get_type(item) == json_type_object)
						printf("\t\tjson_type_object: %s\n", json_object_to_json_string(item));
					else if (json_object_get_type(item) == json_type_array)
					{				
						printf("\t\tjson_type_array: %s\n", json_object_to_json_string(item));
						for(int i = 0; i < json_object_array_length(item); i++) {
							struct json_object *item2 = json_object_array_get_idx(item, i);
							printf("\t\t\titem[%d]: %s\n", i, json_object_to_json_string(item2));
						}
					}
					else if (json_object_get_type(item) == json_type_string)
						printf("\t\tjson_type_string: %s\n", json_object_to_json_string(item));
/*					
					struct json_object* todo_rid = json_object_object_get(item, "todo_rid");
					struct json_object* body = json_object_object_get(item, "body");
					if (!is_error(todo_rid) && !is_error(body)) {
						char* _todo_rid = utf8_to_string_alloc(json_object_get_string(todo_rid));
						char* _body = utf8_to_string_alloc(json_object_get_string(body));
						printf("TODO-%03d [%s] %s\n", i+1, _todo_rid, _body);
						free(_todo_rid);
						free(_body);
					}
 */
				}			
			}
			else if (json_object_get_type(val) == json_type_string) {
				printf("\t%s: json_type_string: %s\n", key, json_object_to_json_string(val));
				if (strcmp(key, "IP") == 0)
				{
					ip = json_object_to_json_string(val);
				}
			}
		}		
	}	
	json_object_put(jobj);
	free(input);

	return ret;
}


