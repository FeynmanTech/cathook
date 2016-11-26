/*
 * profiler.h
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include <time.h>

#define ENABLE_PROFILER false
#define PROFILER_OUTPUT_DEPTH 1
#define MAX_PROFILER_SECTIONS 16

#if ENABLE_PROFILER == true
#define PROF_BEGIN() PROFILER_BeginSection()
#define PROF_END(X) PROFILER_EndSection(X)
#else
#define PROF_BEGIN(X)
#define PROF_END(X)
#endif

void PROFILER_BeginSection();
void PROFILER_EndSection(char* name);

#endif /* PROFILER_H_ */
