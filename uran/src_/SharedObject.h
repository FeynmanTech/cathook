/*
 * SharedObject.h
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#ifndef SHAREDOBJECT_H_
#define SHAREDOBJECT_H_

#include <link.h>

typedef void *(*CreateInterface_t)(const char*, int*);

class SharedObject {
public:
	SharedObject(const char* path);
	SharedObject* Load();
	SharedObject* SetupFactory();
	int* RelativePtr(int offset);
	template<typename T>
	T CreateInterface(const char* name);

	link_map* lmap;
	const char* path;
	CreateInterface_t fptr;
};

#endif /* SHAREDOBJECT_H_ */
