/*
 * HAimbot.h
 *
 *  Created on: Oct 8, 2016
 *      Author: nullifiedcat
 */

#ifndef HAIMBOT_H_
#define HAIMBOT_H_

#include "IHack.h"
#include "../enums.h"

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
	Aimbot();

	virtual void ProcessUserCmd(CUserCmd*) override;
	virtual void OnLevelInit() override;

	int ShouldTarget(CachedEntity* entity);
	bool Aim(CachedEntity* entity, CUserCmd* cmd);
	bool ShouldAim(CUserCmd* cmd);
	int BestHitbox(CachedEntity* target, int preferred = hitbox_t::spine_0);

	bool m_bProjectileMode;
	float m_flProjSpeed;
	float m_flProjGravity;
	bool m_bProjArc;
	bool m_bSilentHuntsman;
	//Vector m_vPredictedProjTarget;

	int m_iLastTarget;
	int m_iPreferredHitbox;
	int m_iFinalHitbox;
	bool m_bAimKeySwitch;
	int m_nMinigunFixTicks;
	bool m_bHeadOnly;

	CatVar* v_kAimKey;
	CatVar* v_eAimKeyMode;
	CatVar* v_eHitboxMode;
	//CatVar* v_bSmooth;
	//CatVar* v_fSmoothValue;
	//CatVar* v_bDebug;
	CatVar* v_bEnabled;
	CatVar* v_fFOV;
	CatVar* v_eHitbox;
//	CatVar* v_bAutoHitbox;
//	CatVar* v_iSeenDelay;
	CatVar* v_bInterpolation;
	CatVar* v_bAutoShoot;
	CatVar* v_bSilent;
	CatVar* v_bZoomedOnly;
	CatVar* v_bAimAtTeammates;
	//CatVar* v_iAutoShootCharge;
	CatVar* v_fAutoShootHuntsmanCharge;
	CatVar* v_iMaxRange;
	CatVar* v_iMaxAutoshootRange; // TODO IMPLEMENT
	CatVar* v_bRespectCloak;
	//CatVar* v_bCharge;
	CatVar* v_bEnabledAttacking;
	//CatVar* v_bTriggerMode;
	CatVar* v_bProjectileAimbot;
	CatVar* v_fOverrideProjSpeed;
	CatVar* v_fOverrideProjGravity; // TODO IMPLEMENT
	//CatVar* v_bMachinaPenetration;
	CatVar* v_bAimBuildings;
	CatVar* v_bActiveOnlyWhenCanShoot;
	//CatVar* v_fSmoothAutoshootTreshold;
	//CatVar* v_fSmoothRandomness;
	CatVar* v_ePriorityMode;
	CatVar* v_bProjPredVisibility;
	CatVar* v_bProjPredFOV;
	CatVar* v_bAdvancedSapperAim;
};

DECLARE_HACK_SINGLETON(Aimbot);

#endif /* HAIMBOT_H_ */
