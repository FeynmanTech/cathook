/*
 * globals.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: nullifiedcat
 */

#include "globals.h"

#include "helpers.h"

void GlobalSettings::Init() {
	this->bHackEnabled = CreateConVar("u_enabled", "1", "Hack enabled (GLOBAL)");
	this->bIgnoreTaunting = CreateConVar("u_ignore_taunting", "1", "Ignore taunting players");
	this->flForceFOV = CreateConVar("u_fov", "90", "Forces FOV");
	this->bProfiler = CreateConVar("u_profiler", "0", "Profiler output enabled");
}

GlobalSettings g_Settings;
