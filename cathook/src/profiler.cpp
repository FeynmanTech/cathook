/*
 * profiler.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#include "profiler.h"

#include "common.h"
#include "sdk.h"

#if ENABLE_PROFILER

int g_ProfilerDepth = 0;
long g_ProfilerSections[MAX_PROFILER_SECTIONS];

void PROFILER_BeginSection() {
	if (!ENABLE_PROFILER) return;
	if (g_ProfilerDepth == MAX_PROFILER_SECTIONS - 1) {
		logging::Info("Max profiler depth reached!");
		return;
	}
	g_ProfilerDepth++;
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	g_ProfilerSections[g_ProfilerDepth] = ts.tv_nsec;
}

void PROFILER_EndSection(char* name) {
	if (!ENABLE_PROFILER) return;
	if (g_ProfilerDepth == 0) {
		logging::Info("Profiler underflow!");
		return;
	}
	if (g_ProfilerDepth <= PROFILER_OUTPUT_DEPTH && g_Settings.bProfiler->GetBool()) {
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		logging::Info("[PROF] %s took %ldus!", name, (ts.tv_nsec - g_ProfilerSections[g_ProfilerDepth]));
	}
	g_ProfilerDepth--;
}

#endif
