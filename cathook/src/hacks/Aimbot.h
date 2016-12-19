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
class Vector;

struct AimKeyMode_t {
	enum {
		DISABLED,
		PRESS_TO_ENABLE,
		PRESS_TO_DISABLE,
		PRESS_TO_TOGGLE
	};
};

class Aimbot : public IHack {
public:
	DECLARE_HACK_METHODS();
	Aimbot();
	bool ShouldTarget(IClientEntity* entity);
	bool Aim(IClientEntity* entity, CUserCmd* cmd);

	bool m_bProjectileMode;
	float m_flProjSpeed;
	float m_flProjGravity;
	bool m_bProjArc;
	//Vector m_vPredictedProjTarget;

	int m_iLastTarget;
	int m_iHitbox;
	bool m_bAimKeySwitch;
	int m_nMinigunFixTicks;
	ConVar* v_iAimKey;
	ConVar* v_iAimKeyMode;
	ConVar* v_bMinigunFix;
	ConVar* v_bSmooth;
	ConVar* v_fSmoothValue;
	ConVar* v_bDebug;
	ConVar* v_bEnabled;
	ConVar* v_fFOV;
	ConVar* v_iHitbox;
	ConVar* v_bAutoHitbox;
	ConVar* v_iSeenDelay;
	ConVar* v_bPrediction;
	ConVar* v_bAutoShoot;
	ConVar* v_bSilent;
	ConVar* v_bZoomedOnly;
	ConVar* v_iAutoShootCharge;
	ConVar* v_flAutoShootHuntsmanCharge;
	ConVar* v_iMaxRange;
	ConVar* v_bRespectCloak;
	ConVar* v_bCharge;
	ConVar* v_bEnabledAttacking;
	ConVar* v_bStrictAttack;
	ConVar* v_bProjectileAimbot;
	ConVar* v_iOverrideProjSpeed;
	ConVar* v_bMachinaPenetration;
	ConVar* v_bAimBuildings;
	ConVar* v_bActiveOnlyWhenCanShoot;
	ConVar* v_fSmoothAutoshootTreshold;
	ConVar* v_fSmoothRandomness;
	ConVar* v_iPriorityMode;
};

DECLARE_HACK_SINGLETON(Aimbot);

#endif /* HAIMBOT_H_ */
