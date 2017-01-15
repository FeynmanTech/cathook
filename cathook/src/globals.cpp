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
			CE_INT(g_pLocalPlayer->entity, netvar.nForceTauntCam) = 0;
	}
}

void GlobalSettings::Init() {
	this->sDisconnectMsg = CreateConVar(CON_PREFIX "disconnect_msg", "", "Set custom disconnect message");

//	this->bMaxPerformance = CREATE_CV(CV_SWITCH, "optimize_performance", "0", "deprecated");
	this->bHackEnabled = CREATE_CV(CV_SWITCH, "enabled", "1", "CatHook enabled");
	this->bIgnoreTaunting = CREATE_CV(CV_SWITCH, "ignore_taunting", "1", "Ignore taunting players");
	this->flForceFOV = CREATE_CV(CV_FLOAT, "fov", "0", "FOV");
//	this->bProfiler = CREATE_CV(CV_SWITCH, "profiler", "0", "Profiler");
	this->bNoZoom = CREATE_CV(CV_SWITCH, "nozoom", "0", "No Zoom");
	this->flForceFOVZoomed = CREATE_CV(CV_FLOAT, "fov_zoomed", "0", "FOV when zoomed");
	this->bZoomedFOV = CREATE_CV(CV_SWITCH, "zoom_keep_fov", "1", "When zoomed, use " CON_PREFIX "fov_zoomed");
//	this->bNoFlinch = CREATE_CV(CV_SWITCH, "noflinch", "0", "No Flinch (broken)");
	this->bSendPackets = CREATE_CV(CV_SWITCH, "sendpackets", "1", "Send packets");
	this->bShowLogo = CREATE_CV(CV_SWITCH, "logo", "1", "Show logo");
	this->bShowAntiAim = CREATE_CV(CV_SWITCH, "thirdperson_angles", "1", "Real angles in thirdperson");
	this->bThirdperson = CREATE_CV(CV_SWITCH, "thirdperson", "0", "Thirdperson");
	this->bNoVisuals = CREATE_CV(CV_SWITCH, "novisuals", "0", "Disable visuals");
	this->bThirdperson->m_pConVar->InstallChangeCallback(ThirdpersonCallback);
	bInvalid = true;
}

const char* g_pszTFPath = 0;

GlobalSettings g_Settings;
