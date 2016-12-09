/*
 * globals.h
 *
 *  Created on: Nov 16, 2016
 *      Author: nullifiedcat
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

class ConVar;

class GlobalSettings {
public:
	void Init();
	// TODO
	ConVar* flForceFOV;
	ConVar* bHackEnabled;
	ConVar* bIgnoreTaunting;
	ConVar* bProfiler;
	ConVar* bNoZoom;
	ConVar* bNoFlinch;
	ConVar* bSendPackets;
	ConVar* bShowLogo;
	ConVar* flDrawingOpacity;
};

extern GlobalSettings g_Settings;

#endif /* GLOBALS_H_ */
