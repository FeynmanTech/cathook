/*
 * profiler.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#include "profiler.h"

#include "logging.h"

int g_ProfilerDepth = 0;
char* g_ProfilerSections[MAX_PROFILER_SECTIONS];
time_t g_ProfilerSectionsTime[MAX_PROFILER_SECTIONS];

void BeginSection(char* name) {
	if (!ENABLE_PROFILER) return;
	if (g_ProfilerDepth == MAX_PROFILER_SECTIONS - 1) {
		logging::Info("Max profiler depth reached!");
		return;
	}
	g_ProfilerSections[g_ProfilerDepth] = name;
	g_ProfilerSectionsTime[g_ProfilerDepth] = time(0);
	g_ProfilerDepth++;
}

void EndSection(char* name) {
	if (!ENABLE_PROFILER) return;
	if (g_ProfilerDepth == 0) {
		logging::Info("Profiler underflow!");
		return;
	}
	if (g_ProfilerDepth <= PROFILER_OUTPUT_DEPTH)
		logging::Info("[PROF] %s took %lums!", g_ProfilerSections[g_ProfilerDepth], time(0) - g_ProfilerSectionsTime[g_ProfilerDepth]);
	g_ProfilerDepth--;
}
