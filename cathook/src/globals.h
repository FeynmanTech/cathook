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

class GlobalSettings {
public:
	void Init();
// TODO
//	CatVar* bMaxPerformance;
	CatVar* flForceFOV;
	CatVar* bHackEnabled;
	CatVar* bIgnoreTaunting;
//	CatVar* bProfiler;
//	CatVar* bNoZoom;
//	CatVar* bNoFlinch;
	CatVar* bSendPackets;
	CatVar* bShowLogo;
	CatVar* flDrawingOpacity;
	ConVar* sDisconnectMsg;
	CatVar* bShowAntiAim;
	CatVar* bThirdperson;
	CatVar* bNoVisuals;
	Vector last_angles;
	bool bInvalid;
};

extern GlobalSettings g_Settings;

#endif /* GLOBALS_H_ */
