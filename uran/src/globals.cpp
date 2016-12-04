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
	this->bNoZoom = CreateConVar("u_nozoom", "1", "No Zoom");
	this->bNoFlinch = CreateConVar("u_noflinch", "0", "No-Flinch");
	this->bSendPackets = CreateConVar("u_sendpackets", "1", "Send packets");
}

GlobalSettings g_Settings;
