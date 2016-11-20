/*
 * sharedobj.h
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#ifndef SHAREDOBJ_H_
#define SHAREDOBJ_H_

struct link_map;
typedef void *(*fn_CreateInterface_t)(const char*, int*);

namespace sharedobj {

class SharedObject {
public:
	SharedObject(const char* path, bool factory);
	int* Pointer(int offset);
	void* CreateInterface(const char* name);
	fn_CreateInterface_t fptr;
	const char* path;
	link_map* lmap;
};

extern SharedObject* steamclient;
extern SharedObject* client;
extern SharedObject* engine;
extern SharedObject* vguimatsurface;
extern SharedObject* vgui2;
extern SharedObject* vstdlib;
extern SharedObject* tier0;
extern SharedObject* inputsystem;

void LoadAllSharedObjects();

}

#endif /* SHAREDOBJ_H_ */
