/*
 * Misc.h
 *
 *  Created on: Nov 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_MISC_H_
#define HACKS_MISC_H_

#include "IHack.h"

class ConVar;
class ConCommand;

class Misc : public IHack {
public:
	void Create();
	bool CreateMove(void*, float, CUserCmd*);
	void Destroy();
	void PaintTraverse(void*, unsigned int, bool, bool);
	ConVar* v_bDbWeaponInfo;
	ConVar* v_bSemiAuto;
	ConCommand* c_SayLine;
	ConCommand* c_Shutdown;
	ConCommand* c_AddFriend;
	ConCommand* c_AddRage;
	ConCommand* c_DumpVars;
	ConCommand* c_DumpPlayers;
};

#endif /* HACKS_MISC_H_ */
