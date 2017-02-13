/*
 * profiler.h
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include "beforecheaders.h"
#include <chrono>
#include <string>
#include "aftercheaders.h"

class ProfilerNode;

class ProfilerSection {
public:
	ProfilerSection(std::string name);

	void OnNodeDeath(ProfilerNode& node);

	std::chrono::nanoseconds m_min;
	std::chrono::nanoseconds m_max;
	std::chrono::nanoseconds m_sum;
	unsigned m_calls;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_log;
	std::string m_name;
};

class ProfilerNode {
public:
	ProfilerNode(ProfilerSection& section);
	~ProfilerNode();

	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	ProfilerSection& m_section;
};

#define ENABLE_PROFILER true
#if ENABLE_PROFILER
#define PROF_SECTION(id, name) \
	static ProfilerSection __PROFILER__##id(name); \
	ProfilerNode __PROFILER_NODE__##id(__PROFILER__##id);
#else
#define PROF_SECTION(x)
#endif

#endif /* PROFILER_H_ */
