/*
 * HAimbot.h
 *
 *  Created on: Oct 8, 2016
 *      Author: nullifiedcat
 */

#ifndef HAIMBOT_H_
#define HAIMBOT_H_

#include "../enums.h"

class ConVar;
class IClientEntity;
class Vector;

namespace hacks { namespace shared { namespace aimbot {

enum class k_EAimkeyMode {
	DISABLED,
	PRESS_TO_ENABLE,
	PRESS_TO_DISABLE,
	PRESS_TO_TOGGLE
};

enum class k_EHitboxMode {
	STATIC,
	HEADONLY,
	AUTOHEAD,
	AUTOBODY,
	CLOSEST
};

void ProcessUserCmd(CUserCmd* cmd);
void ProcessEntity(CUserCmd* cmd, CachedEntity& entity);
void DoAiming(CUserCmd* cmd);

/*
 * TODO smooth aimbot, autoshoot charge, autoshoot range
 */

extern CatVar aimkey;
extern CatVar aimkey_mode;
extern CatVar enabled;
extern CatVar fov;
extern CatVar hitbox;
extern CatVar auto_hitbox;
//extern CatVar delay;
extern CatVar lerp;
extern CatVar autoshoot;
extern CatVar silent;
extern CatVar zoomed_only;
extern CatVar huntsman_charge;
extern CatVar range;
extern CatVar autoshoot_range; // TODO IMPLEMENT
extern CatVar attack_only;
extern CatVar projectile;
extern CatVar projectile_speed;
extern CatVar projectile_gravity; // TODO IMPLEMENT
extern CatVar canshoot;
extern CatVar priority;
extern CatVar projectile_visibility;
extern CatVar projectile_fov;

}}}

/*class Aimbot : public IHack {
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
	bool m_bSilentHuntsman;
	//Vector m_vPredictedProjTarget;

	int m_iLastTarget;
	int m_iPreferredHitbox;
	int m_iFinalHitbox;
	bool m_bAimKeySwitch;
	int m_nMinigunFixTicks;
	bool m_bHeadOnly;
};*/

#endif /* HAIMBOT_H_ */
