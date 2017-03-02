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
	CatVar_DEPRECATED* v_bSeeLocal;
	CatVar_DEPRECATED* v_bBuildingESP;
	CatVar_DEPRECATED* v_bEnabled;
	CatVar_DEPRECATED* v_bEntityESP;
	CatVar_DEPRECATED* v_bTeammates;
	CatVar_DEPRECATED* v_bItemESP;
	CatVar_DEPRECATED* v_bShowDroppedWeapons;
	CatVar_DEPRECATED* v_bShowAmmoPacks;
	CatVar_DEPRECATED* v_bShowHealthPacks;
	CatVar_DEPRECATED* v_bShowPowerups;
	CatVar_DEPRECATED* v_bShowMoney;
	CatVar_DEPRECATED* v_bShowRedMoney;
	CatVar_DEPRECATED* v_bTeammatePowerup;
	CatVar_DEPRECATED* v_bShowEntityID;
	CatVar_DEPRECATED* v_bShowTank;
	CatVar_DEPRECATED* v_bBox;
	CatVar_DEPRECATED* v_bShowDistance;
	CatVar_DEPRECATED* v_bShowFriendID;
	CatVar_DEPRECATED* v_bShowFriends;
	CatVar_DEPRECATED* v_bShowName;
	CatVar_DEPRECATED* v_bShowClass;
	CatVar_DEPRECATED* v_bShowConditions;
	CatVar_DEPRECATED* v_bVisCheck;
	CatVar_DEPRECATED* v_bLegit;
	CatVar_DEPRECATED* v_bShowHealthNumbers;
//	CatVar* v_iLegitSeenTicks;
	CatVar_DEPRECATED* v_iShowRockets;
	CatVar_DEPRECATED* v_iShowArrows;
	CatVar_DEPRECATED* v_iShowPipes;
	CatVar_DEPRECATED* v_iShowStickies;
	CatVar_DEPRECATED* v_bOnlyEnemyProjectiles;
	CatVar_DEPRECATED* v_bProjectileESP;
	CatVar_DEPRECATED* v_bModelName;
	CatVar_DEPRECATED* v_bShowWeaponSpawners;
	CatVar_DEPRECATED* v_bShowAdrenaline;

	//ConVar* v_bModelInfo;
};

DECLARE_HACK_SINGLETON(ESP);

#endif /* HESP_H_ */
