/*
 * sharedobj.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "sharedobj.h"
#include "logging.h"
#include "helpers.h"
#include "copypasted/CSignature.h"

#include <unistd.h>
#include <link.h>
#include <dlfcn.h>

// TODO rewrite!
const char* path_from_proc_maps(const std::string& name) {
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
		if (!strcmp(name.c_str(), filename)) {
			return buffer + path_begin;
		}
	}
	delete [] buffer;
	return (const char*)0;
}

const char* tf_path_from_maps() {
	FILE* proc_maps = fopen(strfmt("/proc/%i/maps", getpid()), "r");
	if (!proc_maps) return (const char*)0;
	char* buffer = new char[512];
	char* path = new char[256];
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
		if (!strcmp("client.so", filename)) {
			strcpy(path, buffer + path_begin);
			path[filename_begin - path_begin - 4] = '\0';
			return path;
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
sharedobj::SharedObject* sharedobj::studiorender = 0;

sharedobj::SharedObject::SharedObject(const std::string& name, bool factory) {
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

void* sharedobj::SharedObject::CreateInterface(const std::string& name) {
	return (void*)(fptr(name.c_str(), nullptr));;
}

void sharedobj::LoadAllSharedObjects() {
	sharedobj::client = new SharedObject("client.so", true);
	/*uintptr_t itemschemapatch = (gSignatures.GetClientSignature("8B 85 78 FF FF FF 89 44 24 0C 8B 85 68 FF FF FF 89 44 24 08 8B 85 38 FF FF FF 89 44 24 04 8B 85 28 FF FF FF 89 04 24 E8 F4 D0 FF FF 84 C0 0F 84 FC 00 00 00") + 44);
	logging::Info("Patching @ 0x%08x", itemschemapatch);
	if (itemschemapatch) {
		unsigned char patch1[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		Patch((void*)itemschemapatch, (void*)patch1, 8);
	}

	uintptr_t itemschemapatch2 = (gSignatures.GetClientSignature("8B 45 10 89 44 24 08 8B 45 0C 89 44 24 04 8B 45 08 89 04 24 E8 CA 35 A8 00 A2 B4 A9 F8 01 C9 C3") + 22);
	logging::Info("Patching @ 0x%08x", itemschemapatch2);
	if (itemschemapatch2) {
		unsigned char patch1[] = { 0xb0, 0x01, 0x90, 0x90, 0x90 };
		Patch((void*)itemschemapatch2, (void*)patch1, 5);
	}*/
	sharedobj::engine = new SharedObject("engine.so", true);
	sharedobj::steamclient = new SharedObject("steamclient.so", true);
	sharedobj::tier0 = new SharedObject("libtier0.so", false);
	sharedobj::vgui2 = new SharedObject("vgui2.so", true);
	sharedobj::vguimatsurface = new SharedObject("vguimatsurface.so", true);
	sharedobj::vstdlib = new SharedObject("libvstdlib.so", true);
	sharedobj::inputsystem = new SharedObject("inputsystem.so", true);
	sharedobj::studiorender = new SharedObject("studiorender.so", true);
}

