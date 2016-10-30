/*
 * HAimbot.h
 *
 *  Created on: Oct 8, 2016
 *      Author: nullifiedcat
 */

#ifndef HAIMBOT_H_
#define HAIMBOT_H_

#include "IHack.h"

class ConVar;
class IClientEntity;

class HAimbot : public IHack {
public:
	void Create();
	void Destroy();
	bool CreateMove(void*, float, CUserCmd*);
	void PaintTraverse(void*, unsigned int, bool, bool);
	bool ShouldTarget(IClientEntity* entity);
	bool Aim(IClientEntity* entity, CUserCmd* cmd);
	ConVar* v_bEnabled;
	ConVar* v_iHitbox;
	ConVar* v_bPrediction;
	ConVar* v_bAutoShoot;
	ConVar* v_bSilent;
	ConVar* v_bZoomedOnly;
	ConVar* v_iAutoShootCharge;
	ConVar* v_iMinRange;
	ConVar* v_bPriority;
	ConVar* v_bRespectCloak;
	ConVar* v_bCharge;
	ConVar* v_bEnabledAttacking;
	ConVar* v_bStrictAttack;
};


#endif /* HAIMBOT_H_ */
