/*
 * Log.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "Log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

typedef void(MsgFn)(const char* msg, va_list);

namespace uran {
extern MsgFn p_MsgFn;
extern FILE* file_log;
}


namespace hack {

void LogInit() {
	//openlog("uranhack", 0, LOG_USER);
	uran::file_log = fopen("/tmp/uran.log", "w");
}

void Log(const char* fmt, ...) {
	char buffer[989];
	va_list list;
	va_start(list, fmt);
	vsprintf(buffer, fmt, list);
	va_end(list);
	size_t len = strlen(buffer);
	char* newstr = (char*) malloc(len + 9);
	sprintf(newstr, "[URAN] %s\n", buffer);
	fprintf(uran::file_log, "%s", newstr);
	fflush(uran::file_log);
	//syslog(LOG_INFO, "%s", newstr);
	if (uran::p_MsgFn)
		uran::p_MsgFn(newstr, list);
	free(newstr);
}

void LogClose() {
	fclose(uran::file_log);
}

}
