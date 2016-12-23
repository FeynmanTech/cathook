/*
 * logging.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "logging.h"
#include "helpers.h"

#include <stdarg.h>
#include <string.h>
#include <pwd.h>

#include "interfaces.h"
#include "sdk.h"
#include "sstring.h"

FILE* logging::handle = 0;

void logging::Initialize() {
	passwd* pwd = getpwuid(getuid());
	char* user = pwd->pw_name;
	logging::handle = fopen((const char*)strfmt("/tmp/cathook-%s.log", user), "w");
}

void logging::Info(const char* fmt, ...) {
	if (logging::handle == 0) return;
	char* buffer = new char[1024];
	va_list list;
	va_start(list, fmt);
	vsprintf(buffer, fmt, list);
	va_end(list);
	size_t length = strlen(buffer);
	char* result = new char[length + 9];
	sprintf(result, "[CAT] %s\n", buffer);
	fprintf(logging::handle, "%s", result);
	fflush(logging::handle);
	if (interfaces::cvar) {
		interfaces::cvar->ConsolePrintf("%s", result);
	}
	delete [] buffer;
	delete [] result;
}

void logging::Shutdown() {
	fclose(logging::handle);
	logging::handle = 0;
}
