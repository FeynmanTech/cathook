/*
 * hooks.cpp
 *
 *  Created on: Oct 4, 2016
 *      Author: nullifiedcat
 */

#include "hooks.h"

#include "logging.h"

#include <stdlib.h>

unsigned int hooks::offCreateMove = 22;
unsigned int hooks::offPaintTraverse = 42;

unsigned int hooks::CountMethods(void** vmt) {
	unsigned int i = -1;
	do ++i; while (vmt[i]);
	return i;
}

void**& hooks::GetVMT(void* inst, unsigned int offset) {
	// I'm not quite sure what happens here...
	// inst is the pointer to class
	// (char*) inst + offset is the pointer to vmt
	// so it casts vmt pointer to void*** and returns its value..?
	return *reinterpret_cast<void***>((char*)inst + offset);
}

void hooks::VMTHook::Init(void* inst, unsigned int offset) {
	logging::Info("Initializing VMTHook at 0x%08x", inst);
	vmt = &GetVMT(inst, offset);
	oldvmt = *vmt;
	unsigned int cnt = CountMethods(oldvmt);
	logging::Info("found %i methods...", cnt);
	void **arr = array = (void**)malloc((cnt + 4) * sizeof(void*));
	arr[0] = this;
	arr[1] = (void* )GUARD;
	(arr + 3)[cnt] = 0;
	unsigned int i = -1;
	do arr[i + 3] = oldvmt[i]; while (++i < cnt);
}

void hooks::VMTHook::Kill() {
	*vmt = oldvmt;
	vmt = 0;
	free(array);
	array = 0;
}

void hooks::VMTHook::HookMethod(void* func, unsigned int idx) {
	array[idx + 3] = func;
}

void* hooks::VMTHook::GetMethod(unsigned int idx) const {
	return oldvmt[idx];
}

void hooks::VMTHook::Apply() {
	*vmt = array + 3;
}

hooks::VMTHook* hooks::hkCreateMove = 0;
hooks::VMTHook* hooks::hkPaintTraverse = 0;

