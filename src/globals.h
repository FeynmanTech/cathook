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
class CatVar_DEPRECATED;

typedef void(EstimateAbsVelocity_t)(IClientEntity*, Vector&);
extern EstimateAbsVelocity_t* EstimateAbsVelocity;

extern int g_AppID;

class GlobalSettings {
public:
	void Init();
// TODO
//	CatVar* bMaxPerformance;
	CatVar_DEPRECATED* flForceFOV;
	CatVar_DEPRECATED* flForceFOVZoomed;
	CatVar_DEPRECATED* bZoomedFOV;
	CatVar_DEPRECATED* bHackEnabled;
	CatVar_DEPRECATED* bIgnoreTaunting;
//	CatVar* bProfiler;
	CatVar_DEPRECATED* bNoZoom;
//	CatVar* bNoFlinch;
	CatVar_DEPRECATED* bSendPackets;
	CatVar_DEPRECATED* bShowLogo;
	CatVar_DEPRECATED* flDrawingOpacity;
	CatVar_DEPRECATED* sDisconnectMsg;
	CatVar_DEPRECATED* bShowAntiAim;
	CatVar_DEPRECATED* bThirdperson;
	CatVar_DEPRECATED* bNoVisuals;
	CatVar_DEPRECATED* bCleanScreenshots;
	CatVar_DEPRECATED* bDebugLog;
	Vector last_angles;
	CatVar_DEPRECATED* bFastOutline;
	CatVar_DEPRECATED* kRollSpeedhack;
	CatVar_DEPRECATED* bFastVischeck;
	bool bInvalid;
};

extern const char* g_pszTFPath;

extern GlobalSettings g_Settings;

#endif /* GLOBALS_H_ */
