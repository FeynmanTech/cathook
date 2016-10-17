/*
 * Log.h
 *
 *  Created on: Aug 11, 2016
 *      Author: nullifiedcat
 */

#ifndef LOG_H_
#define LOG_H_

namespace hack {

void LogInit();
void LogClose();
void Log(const char* fmt, ...);

}

#endif /* LOG_H_ */
