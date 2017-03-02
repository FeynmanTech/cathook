/*
 * globals.h
 *
 *  Created on: Nov 16, 2016
 *      Author: nullifiedcat
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

class Vector;
class ConVar;
class CatVar;

typedef void(EstimateAbsVelocity_t)(IClientEntity*, Vector&);
extern EstimateAbsVelocity_t* EstimateAbsVelocity;

extern int g_AppID;

class GlobalSettings {
public:
	void Init();
// TODO
//	CatVar* bMaxPerformance;
	CatVar* flForceFOV;
	CatVar* flForceFOVZoomed;
	CatVar* bZoomedFOV;
	CatVar* bHackEnabled;
	CatVar* bIgnoreTaunting;
//	CatVar* bProfiler;
	CatVar* bNoZoom;
//	CatVar* bNoFlinch;
	CatVar* bSendPackets;
	CatVar* bShowLogo;
	CatVar* flDrawingOpacity;
	CatVar* sDisconnectMsg;
	CatVar* bShowAntiAim;
	CatVar* bThirdperson;
	CatVar* bNoVisuals;
	CatVar* bCleanScreenshots;
	CatVar* bDebugLog;
	Vector last_angles;
	CatVar* bFastOutline;
	CatVar* kRollSpeedhack;
	CatVar* bFastVischeck;
	bool bInvalid;
};

extern const char* g_pszTFPath;

extern GlobalSettings g_Settings;

#endif /* GLOBALS_H_ */
