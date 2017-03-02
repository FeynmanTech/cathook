/*
 * globals.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"
#include "copypasted/CSignature.h"

EstimateAbsVelocity_t* EstimateAbsVelocity = 0;

int g_AppID = 0;

void ThirdpersonCallback(IConVar* var, const char* pOldValue, float flOldValue) {
	if (g_Settings.bThirdperson && !g_Settings.bThirdperson->GetBool()) {
		if (g_pLocalPlayer && g_pLocalPlayer->entity)
			CE_INT(g_pLocalPlayer->entity, netvar.nForceTauntCam) = 0;
	}
}

void GlobalSettings::Init() {
	EstimateAbsVelocity = (EstimateAbsVelocity_t*)gSignatures.GetClientSignature("55 89 E5 56 53 83 EC 30 8B 5D 08 8B 75 0C E8 4D 2E 01 00 39 D8 74 69 0F B6 05 24 3F 00 02 81 C3 B8 02 00 00 C6 05 24 3F 00 02 01 88 45 F0 A1 20 3F 00 02 89 45 F4 A1 28 3F 00 02 89 45 EC 8D 45 EC A3 28 3F 00 02 A1 14 C8 F6 01 8B 40 0C 89 74 24 04 89 1C 24 89 44 24 08 E8 A2 41 00 00 0F B6 45 F0 A2 24 3F 00 02 8B 45 F4 A3 20 3F 00 02 8B 45 EC A3 28 3F 00 02 83 C4 30 5B 5E 5D C3");

	this->sDisconnectMsg = new CatVar(CV_STRING, "disconnect_msg", "", "Disconnect reason", NULL, "A custom disconnect message");
	this->bFastVischeck = new CatVar(CV_SWITCH, "fast_vischeck", "1", "Fast VisCheck", NULL, "VisChecks only certain hitboxes");
//	this->bMaxPerformance = CREATE_CV(CV_SWITCH, "optimize_performance", "0", "deprecated");
	this->bHackEnabled = new CatVar(CV_SWITCH, "enabled", "1", "CatHook enabled", NULL, "Master Hack Switch");
	this->bIgnoreTaunting = new CatVar(CV_SWITCH, "ignore_taunting", "1", "Ignore taunting", NULL, "Aimbot/Triggerbot won't attack taunting enemies");
	this->flForceFOV = new CatVar(CV_FLOAT, "fov", "0", "FOV", NULL, "FOV override (0 - don't change)", true, 360.0f, 0.0f);
//	this->bProfiler = CREATE_CV(CV_SWITCH, "profiler", "0", "Profiler");
	this->bNoZoom = new CatVar(CV_SWITCH, "nozoom", "0", "No Zoom", NULL, "Disable black scope overlay");
	this->flForceFOVZoomed = new CatVar(CV_FLOAT, "fov_zoomed", "0", "FOV when zoomed", NULL, "FOV override (when zoomed)", true, 360.0f, 0.0f);
	this->bZoomedFOV = new CatVar(CV_SWITCH, "zoom_keep_fov", "1", "When zoomed, use " CON_PREFIX "fov_zoomed");
//	this->bNoFlinch = CREATE_CV(CV_SWITCH, "noflinch", "0", "No Flinch (broken)");
	this->bSendPackets = new CatVar(CV_SWITCH, "sendpackets", "1", "Send packets", NULL, "bSendPackets");
	this->bShowLogo = new CatVar(CV_SWITCH, "logo", "1", "Show cathook", NULL, "Show cathook build/version in top left corner");
	this->bShowAntiAim = new CatVar(CV_SWITCH, "thirdperson_angles", "1", "Real TP angles", NULL, "You can see your own AA/Aimbot angles in thirdperson");
	this->bThirdperson = new CatVar(CV_SWITCH, "thirdperson", "0", "Thirdperson", NULL, "Enable thirdperson view");
	this->bNoVisuals = new CatVar(CV_SWITCH, "novisuals", "0", "Disable visuals", NULL, "Disable all visuals");
	this->bCleanScreenshots = new CatVar(CV_SWITCH, "clean_screenshot", "1", "Clean screenshots", NULL, "Clean screenshots");
	this->bDebugLog = new CatVar(CV_SWITCH, "log", "1", "Debug Log", NULL, "Disable this if you don't need cathook messages in your console");
	this->bThirdperson->OnRegister([](CatVar* var) {
		var->convar->InstallChangeCallback(ThirdpersonCallback);
	});
	this->bFastOutline = new CatVar(CV_SWITCH, "fastoutline", "0", "Low quality outline", NULL, "Might increase performance when there is a lot of ESP text to draw");
	this->kRollSpeedhack = new CatVar(CV_KEY, "rollspeedhack", "0", "Roll Speedhack", NULL, "Roll speedhack key");
	bInvalid = true;
}

const char* g_pszTFPath = 0;

GlobalSettings g_Settings;
