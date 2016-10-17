/*
 * SharedObject.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "SharedObject.h"
#include "Log.h"
#include <dlfcn.h>
#include <unistd.h>

SharedObject::SharedObject(const char* path) {
	this->path = path;
}

SharedObject* SharedObject::Load() {
	if (lmap) return this;
	while (!lmap) {
		lmap = (link_map*)dlopen(path, RTLD_NOLOAD);
		sleep(1);
	}
	hack::Log("Loaded SharedObject %s at 0x%08x", lmap->l_name, lmap->l_addr);
	return this;
}

int* SharedObject::RelativePtr(int offset) {
	return (int*)((int) lmap->l_addr + offset);
}

SharedObject* SharedObject::SetupFactory() {
	fptr = reinterpret_cast<CreateInterface_t>(dlsym(lmap, "CreateInterface"));
	return this;
}

template<typename T>
T SharedObject::CreateInterface(const char* name) {
	int retval;
	T iface = static_cast<T>(fptr(name, &retval));
	if (retval) {
		hack::Log("Failed to create interface %s: error %i (%s)", name, retval, lmap->l_name);
	} else {
		hack::Log("Interface %s created (%s)", name, lmap->l_name);
	}
	return iface;
}
