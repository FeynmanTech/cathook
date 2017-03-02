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
class CachedEntity;

class ESP : public IHack {
public:
	ESP();

	virtual void ProcessUserCmd(CUserCmd*) override;
	virtual void Draw() override;

	void DrawBox(CachedEntity* ent, int clr, float widthFactor, float addHeight, bool healthbar, int health, int healthmax);
	void ProcessEntity(CachedEntity* ent);
	void ProcessEntityPT(CachedEntity* ent);
	CatVar* v_bSeeLocal;
	CatVar* v_bBuildingESP;
	CatVar* v_bEnabled;
	CatVar* v_bEntityESP;
	CatVar* v_bTeammates;
	CatVar* v_bItemESP;
	CatVar* v_bShowDroppedWeapons;
	CatVar* v_bShowAmmoPacks;
	CatVar* v_bShowHealthPacks;
	CatVar* v_bShowPowerups;
	CatVar* v_bShowMoney;
	CatVar* v_bShowRedMoney;
	CatVar* v_bTeammatePowerup;
	CatVar* v_bShowEntityID;
	CatVar* v_bShowTank;
	CatVar* v_bBox;
	CatVar* v_bShowDistance;
	CatVar* v_bShowFriendID;
	CatVar* v_bShowFriends;
	CatVar* v_bShowName;
	CatVar* v_bShowClass;
	CatVar* v_bShowConditions;
	CatVar* v_bVisCheck;
	CatVar* v_bLegit;
	CatVar* v_bShowHealthNumbers;
//	CatVar* v_iLegitSeenTicks;
	CatVar* v_iShowRockets;
	CatVar* v_iShowArrows;
	CatVar* v_iShowPipes;
	CatVar* v_iShowStickies;
	CatVar* v_bOnlyEnemyProjectiles;
	CatVar* v_bProjectileESP;
	CatVar* v_bModelName;
	CatVar* v_bShowWeaponSpawners;
	CatVar* v_bShowAdrenaline;

	//ConVar* v_bModelInfo;
};

DECLARE_HACK_SINGLETON(ESP);

#endif /* HESP_H_ */
