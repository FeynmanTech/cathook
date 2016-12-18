/*
 * globals.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: nullifiedcat
 */

#include "common.h"

void GlobalSettings::Init() {
	this->bHackEnabled = CreateConVar(CON_PREFIX "enabled", "1", "Hack enabled (GLOBAL)");
	this->bIgnoreTaunting = CreateConVar(CON_PREFIX "ignore_taunting", "1", "Ignore taunting players");
	this->flForceFOV = CreateConVar(CON_PREFIX "fov", "0", "Forces FOV");
	this->bProfiler = CreateConVar(CON_PREFIX "profiler", "0", "Profiler output enabled");
	this->bNoZoom = CreateConVar(CON_PREFIX "nozoom", "0", "No Zoom (Breaks aimbot)");
	this->bNoFlinch = CreateConVar(CON_PREFIX "noflinch", "0", "No-Flinch");
	this->bSendPackets = CreateConVar(CON_PREFIX "sendpackets", "1", "Send packets");
	this->bShowLogo = CreateConVar(CON_PREFIX "logo", "1", "Show logo");
	this->sDisconnectMsg = CreateConVar(CON_PREFIX "disconnect_msg", "", "Set custom disconnect message");
}

GlobalSettings g_Settings;
