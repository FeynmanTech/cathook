/*
 * globals.h
 *
 *  Created on: Nov 16, 2016
 *      Author: nullifiedcat
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

class ConVar;

class CatVar;

class GlobalSettings {
public:
	void Init();
	CatVar* flForceFOV;
	CatVar* bHackEnabled;
	CatVar* bIgnoreTaunting;
	CatVar* bProfiler;
	CatVar* bNoZoom;
	CatVar* bNoFlinch;
	CatVar* bSendPackets;
	CatVar* bShowLogo;
	CatVar* flDrawingOpacity;
	ConVar* sDisconnectMsg;
	CatVar* bShowAntiAim;
	CatVar* bThirdperson;
	CatVar* bNoVisuals;
};

extern GlobalSettings g_Settings;

#endif /* GLOBALS_H_ */
