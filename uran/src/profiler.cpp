/*
 * profiler.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#include "profiler.h"

#include "logging.h"
#include "globals.h"

#include "fixsdk.h"
#include <convar.h>

int g_ProfilerDepth = 0;
time_t g_ProfilerSections[MAX_PROFILER_SECTIONS];

void PROFILER_BeginSection() {
	if (!ENABLE_PROFILER) return;
	if (g_ProfilerDepth == MAX_PROFILER_SECTIONS - 1) {
		logging::Info("Max profiler depth reached!");
		return;
	}
	g_ProfilerDepth++;
	g_ProfilerSections[g_ProfilerDepth] = time(0);
}

void PROFILER_EndSection(char* name) {
	if (!ENABLE_PROFILER) return;
	if (g_ProfilerDepth == 0) {
		logging::Info("Profiler underflow!");
		return;
	}
	if (g_ProfilerDepth <= PROFILER_OUTPUT_DEPTH && g_Settings.bProfiler->GetBool())
		logging::Info("[PROF] %s took %ims!", name, (time(0) - g_ProfilerSections[g_ProfilerDepth]));
	g_ProfilerDepth--;
}
