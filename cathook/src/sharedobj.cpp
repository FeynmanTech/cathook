/*
 * sharedobj.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "sharedobj.h"
#include "logging.h"
#include "helpers.h"

#include <unistd.h>
#include <link.h>
#include <dlfcn.h>

const char* path_from_proc_maps(const char* name) {
	FILE* proc_maps = fopen(strfmt("/proc/%i/maps", getpid()), "r");
	if (!proc_maps) return (const char*)0;
	char* buffer = new char[512];
	while (fgets(buffer, 512, proc_maps)) {
		size_t length = strlen(buffer);
		size_t path_begin = 0;
		size_t filename_begin = 0;
		for (size_t i = 0; i < length; i++) {
			if (*(char*)((size_t)buffer + i) == '/' && !path_begin) path_begin = i;
			if (*(char*)((size_t)buffer + i) == '/') filename_begin = i + 1;
		}
		if (!path_begin || !filename_begin) continue;
		char* filename = buffer + filename_begin;
		filename[strlen(filename) - 1] = '\0';
		if (!strcmp(name, filename)) {
			return buffer + path_begin;
		}
	}
	delete [] buffer;
	return (const char*)0;
}

namespace logging {
void Info(const char* fmt, ...);
}

sharedobj::SharedObject* sharedobj::steamclient = 0;
sharedobj::SharedObject* sharedobj::client = 0;
sharedobj::SharedObject* sharedobj::engine = 0;
sharedobj::SharedObject* sharedobj::vguimatsurface = 0;
sharedobj::SharedObject* sharedobj::vgui2 = 0;
sharedobj::SharedObject* sharedobj::vstdlib = 0;
sharedobj::SharedObject* sharedobj::tier0 = 0;
sharedobj::SharedObject* sharedobj::inputsystem = 0;

sharedobj::SharedObject::SharedObject(const char* name, bool factory) {
	logging::Info("Loading SharedObject: %s", name);
	while (!(this->path = path_from_proc_maps(name))) {
		sleep(1);
	}
	while (!(lmap = (link_map*)dlopen(path, RTLD_NOLOAD))) {
		sleep(1);
		if (0 != dlerror()) {
			logging::Info("DLERROR: %s", dlerror());
		}
	}
	logging::Info("Shared object %s loaded at 0x%08x", basename(lmap->l_name), lmap->l_addr);
	if (factory) {
		fptr = reinterpret_cast<fn_CreateInterface_t>(dlsym(lmap, "CreateInterface"));
		if (!this->fptr) {
			logging::Info("Failed to create interface factory for %s", basename(lmap->l_name));
		}
	}
}

int* sharedobj::SharedObject::Pointer(int offset) {
	if (this->lmap) {
		return (int*)((int) this->lmap->l_addr + offset);
	} else {
		return 0;
	}
}

void* sharedobj::SharedObject::CreateInterface(const char* name) {
	int result;
	void* interface = (void*)(fptr(name, &result));
	if (result) {
		logging::Info("Interface creation failed: %s from %s", name, basename(lmap->l_name));
	}
	return interface;
}

void sharedobj::LoadAllSharedObjects() {
	sharedobj::client = new SharedObject("client.so", true);
	sharedobj::engine = new SharedObject("engine.so", true);
	sharedobj::steamclient = new SharedObject("steamclient.so", true);
	sharedobj::tier0 = new SharedObject("libtier0.so", false);
	sharedobj::vgui2 = new SharedObject("vgui2.so", true);
	sharedobj::vguimatsurface = new SharedObject("vguimatsurface.so", true);
	sharedobj::vstdlib = new SharedObject("libvstdlib.so", true);
	sharedobj::inputsystem = new SharedObject("inputsystem.so", true);
}

