/*
 * HEsp.h
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#ifndef HESP_H_
#define HESP_H_

#include "IHack.h"

class ConVar;
class Color;
class CachedEntity;

class ESP : public IHack {
public:
	DECLARE_HACK_METHODS();
	ESP();
	void DrawBox(CachedEntity* ent, Color clr, float widthFactor, float addHeight, bool healthbar, int health, int healthmax);
	void ProcessEntity(CachedEntity* ent);
	void ProcessEntityPT(CachedEntity* ent);
	ConVar* v_bEnabled;
	ConVar* v_bBoxESP;
	ConVar* v_bEntityESP;
	ConVar* v_bTeammates;
	ConVar* v_bItemESP;
	ConVar* v_bShowDroppedWeapons;
	ConVar* v_bShowAmmoPacks;
	ConVar* v_bShowHealthPacks;
	ConVar* v_bShowPowerups;
	ConVar* v_bShowMoney;
	ConVar* v_bTeammatePowerup;
	ConVar* v_bShowHead;
	ConVar* v_bShowEntityID;
	ConVar* v_bBox;
	ConVar* v_bShowDistance;
	ConVar* v_bShowFriendID;
	ConVar* v_bShowFriends;
	ConVar* v_bVisCheck;
	ConVar* v_bLegit;
	ConVar* v_bShowHealthNumbers;
	ConVar* v_iLegitSeenTicks;
	//ConVar* v_bModelInfo;
};

DECLARE_HACK_SINGLETON(ESP);

#endif /* HESP_H_ */
