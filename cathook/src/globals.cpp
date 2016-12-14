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
	this->flForceFOV = CreateConVar("u_fov", "0", "Forces FOV");
	this->bProfiler = CreateConVar("u_profiler", "0", "Profiler output enabled");
	this->bNoZoom = CreateConVar("u_nozoom", "0", "No Zoom (Breaks aimbot)");
	this->bNoFlinch = CreateConVar("u_noflinch", "0", "No-Flinch");
	this->bSendPackets = CreateConVar("u_sendpackets", "1", "Send packets");
	this->bShowLogo = CreateConVar("u_logo", "1", "Show logo");
	this->sDisconnectMsg = CreateConVar("u_disconnect_msg", "", "Set custom disconnect message");
}

GlobalSettings g_Settings;
