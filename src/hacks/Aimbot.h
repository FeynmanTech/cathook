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

	CatVar_DEPRECATED* v_kAimKey;
	CatVar_DEPRECATED* v_eAimKeyMode;
	CatVar_DEPRECATED* v_eHitboxMode;
	//CatVar* v_bSmooth;
	//CatVar* v_fSmoothValue;
	//CatVar* v_bDebug;
	CatVar_DEPRECATED* v_bEnabled;
	CatVar_DEPRECATED* v_fFOV;
	CatVar_DEPRECATED* v_eHitbox;
//	CatVar* v_bAutoHitbox;
//	CatVar* v_iSeenDelay;
	CatVar_DEPRECATED* v_bInterpolation;
	CatVar_DEPRECATED* v_bAutoShoot;
	CatVar_DEPRECATED* v_bSilent;
	CatVar_DEPRECATED* v_bZoomedOnly;
	CatVar_DEPRECATED* v_bAimAtTeammates;
	//CatVar* v_iAutoShootCharge;
	CatVar_DEPRECATED* v_fAutoShootHuntsmanCharge;
	CatVar_DEPRECATED* v_iMaxRange;
	CatVar_DEPRECATED* v_iMaxAutoshootRange; // TODO IMPLEMENT
	CatVar_DEPRECATED* v_bRespectCloak;
	//CatVar* v_bCharge;
	CatVar_DEPRECATED* v_bEnabledAttacking;
	//CatVar* v_bTriggerMode;
	CatVar_DEPRECATED* v_bProjectileAimbot;
	CatVar_DEPRECATED* v_fOverrideProjSpeed;
	CatVar_DEPRECATED* v_fOverrideProjGravity; // TODO IMPLEMENT
	//CatVar* v_bMachinaPenetration;
	CatVar_DEPRECATED* v_bAimBuildings;
	CatVar_DEPRECATED* v_bActiveOnlyWhenCanShoot;
	//CatVar* v_fSmoothAutoshootTreshold;
	//CatVar* v_fSmoothRandomness;
	CatVar_DEPRECATED* v_ePriorityMode;
	CatVar_DEPRECATED* v_bProjPredVisibility;
	CatVar_DEPRECATED* v_bProjPredFOV;
	CatVar_DEPRECATED* v_bAdvancedSapperAim;
};

DECLARE_HACK_SINGLETON(Aimbot);

#endif /* HAIMBOT_H_ */
