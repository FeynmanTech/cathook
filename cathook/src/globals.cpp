/*
 * globals.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"

void ThirdpersonCallback(IConVar* var, const char* pOldValue, float flOldValue) {
	if (g_Settings.bThirdperson && !g_Settings.bThirdperson->GetBool()) {
		if (g_pLocalPlayer && g_pLocalPlayer->entity)
			SetEntityValue<int>(g_pLocalPlayer->entity, netvar.nForceTauntCam, 0);
	}
}

void GlobalSettings::Init() {
	this->bHackEnabled = CreateConVar(CON_PREFIX "enabled", "1", "Hack enabled (GLOBAL)");
	this->bIgnoreTaunting = CreateConVar(CON_PREFIX "ignore_taunting", "1", "Ignore taunting players");
	this->flForceFOV = CreateConVar(CON_PREFIX "fov", "0", "Forces FOV");
	this->bProfiler = CreateConVar(CON_PREFIX "profiler", "0", "Profiler output enabled");
	this->bNoZoom = CreateConVar(CON_PREFIX "nozoom", "0", "No Zoom");
	this->bNoFlinch = CreateConVar(CON_PREFIX "noflinch", "0", "No-Flinch (broken)");
	this->bSendPackets = CreateConVar(CON_PREFIX "sendpackets", "1", "Send packets");
	this->bShowLogo = CreateConVar(CON_PREFIX "logo", "1", "Show logo");
	this->sDisconnectMsg = CreateConVar(CON_PREFIX "disconnect_msg", "", "Set custom disconnect message");
	this->bShowAntiAim = CreateConVar(CON_PREFIX "thirdperson_angles", "1", "Real angles in thirdperson");
	this->bThirdperson = CreateConVar(CON_PREFIX "thirdperson", "0", "Thirdperson");
	this->bThirdperson->InstallChangeCallback(ThirdpersonCallback);
}

GlobalSettings g_Settings;
