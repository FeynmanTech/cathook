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
	DECLARE_HACK_METHODS();
	Misc();
	//ConVar* v_bNoShootUntilCanHeadshot;
	ConVar* v_bDebugInfo;
	ConCommand* c_Name;
	ConVar* v_bInfoSpam;
	ConVar* v_bFastCrouch;
	ConCommand* c_DumpItemAttributes;
	ConCommand* c_SayLine;
	ConCommand* c_Shutdown;
	ConCommand* c_AddFriend;
	ConCommand* c_AddRage;
	ConCommand* c_DumpVars;
	ConCommand* c_DumpPlayers;
	ConCommand* c_Teamname;
	ConCommand* c_Lockee;
	ConCommand* c_Info;
	ConCommand* c_Reset;
	ConCommand* c_Disconnect;
	ConCommand* c_DisconnectVAC;
};

DECLARE_HACK_SINGLETON(Misc);

#endif /* HACKS_MISC_H_ */
