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

enum class aimkey_mode {
	DISABLED,
	PRESS_TO_ENABLE,
	PRESS_TO_DISABLE,
	PRESS_TO_TOGGLE
};

enum class hitbox_mode {
	STATIC,
	HEADONLY,
	AUTOHEAD,
	AUTOBODY,
	CLOSEST
};

/*
 * TODO smooth aimbot, autoshoot charge, autoshoot range
 */

CatVar aimkey;
CatVar aimkey_mode;
CatVar enabled;
CatVar fov;
CatVar hitbox;
CatVar auto_hitbox;
CatVar delay;
CatVar lerp;
CatVar autoshoot;
CatVar silent;
CatVar zoomed_only;
CatVar teammates;
CatVar huntsman_charge;
CatVar range;
CatVar autoshoot_range; // TODO IMPLEMENT
CatVar respect_cloak;
CatVar attack_only;
CatVar projectile;
CatVar projectile_speed;
CatVar projectile_gravity; // TODO IMPLEMENT
CatVar buildings;
CatVar canshoot;
CatVar priority;
CatVar projectile_visibility;
CatVar projectile_fov;

struct data_s {
	data_s();
	bool projectile;
	float proj_speed;
	float proj_gravity;
	int last_target_idx;
	bool aimkey_switch;
	int minigun_spinup;
};

}}}

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
	bool m_bSilentHuntsman;
	//Vector m_vPredictedProjTarget;

	int m_iLastTarget;
	int m_iPreferredHitbox;
	int m_iFinalHitbox;
	bool m_bAimKeySwitch;
	int m_nMinigunFixTicks;
	bool m_bHeadOnly;
};

#endif /* HAIMBOT_H_ */
