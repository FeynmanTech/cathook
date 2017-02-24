/*
 * HAimbot.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: nullifiedcat
 */

#include "../common.h"
#include "../trace.h"
#include "../targethelper.h"

#include "../sdk.h"
#include "../sdk/in_buttons.h"
#include "Aimbot.h"
#include "../targeting/targeting.h"

namespace hacks { namespace shared { namespace aimbot {

CatVar aimkey(CV_KEY, "aimbot_aimkey", "0", "Aimkey", NULL,
		"Aimkey. Look at Aimkey Mode too!");
CatVar aimkey_mode(CV_ENUM, "aimbot_aimkey_mode", "1", "Aimkey mode", new CatEnum({ "DISABLED", "AIMKEY", "REVERSE", "TOGGLE" }),
		"DISABLED: aimbot is always active\nAIMKEY: aimbot is active when key is down\nREVERSE: aimbot is disabled when key is down\nTOGGLE: pressing key toggles aimbot", false);
CatVar enabled(CV_SWITCH, "aimbot_enabled", "0", "Enable Aimbot", NULL,
		"Main aimbot switch");
CatVar fov(CV_FLOAT, "aimbot_fov", "0", "Aimbot FOV", NULL,
		"FOV range for aimbot to lock targets", true, 360.0f);
CatVar hitbox(CV_ENUM, "aimbot_hitbox", "0", "Hitbox", new CatEnum({
	"HEAD", "PELVIS", "SPINE 0", "SPINE 1", "SPINE 2", "SPINE 3", "UPPER ARM L", "LOWER ARM L",
	"HAND L", "UPPER ARM R", "LOWER ARM R", "HAND R", "HIP L", "KNEE L", "FOOT L", "HIP R",
	"KNEE R", "FOOT R" }), "Hitbox to aim at. Change hitbox mode to static for it to be effective.");
//CatVar delay;
CatVar lerp(CV_SWITCH, "aimbot_interp", "1", "Latency interpolation", NULL,
		"Enable basic latency interpolation");
CatVar autoshoot(CV_SWITCH, "aimbot_autoshoot", "1", "Autoshoot", NULL,
		"Shoot automatically when the target is locked, isn't compatible with 'Enable when attacking'");
CatVar silent(CV_SWITCH, "aimbot_silent", "1", "Silent", NULL,
		"Your screen doesn't get snapped to the point where aimbot aims at");
CatVar zoomed_only(CV_SWITCH, "aimbot_zoomed", "1", "Zoomed only", NULL,
		"Don't autoshoot with unzoomed rifles");
CatVar huntsman_charge(CV_FLOAT, "aimbot_huntsman_charge", "0.5", "Huntsman autoshoot", NULL,
		"Minimum charge for autoshooting with huntsman.\n"
		"Set it to 0.01 if you want to shoot as soon as you start "
		"pulling the arrow", true, 1.0f, 0.01f);
CatVar range(CV_INT, "aimbot_maxrange", "0", "Max distance", NULL,
		"Max range for aimbot (doesn't include autoshoot)", true, 4096.0f);
CatVar autoshoot_range(CV_INT, "aimbot_maxrange_autoshoot", "0", "Autoshoot distance", NULL,
		"Max range for autoshoot", true, 4096.0f); // TODO IMPLEMENT
CatVar attack_only(CV_SWITCH, "aimbot_enable_attack_only", "0", "Active when attacking", NULL,
		"Basically makes Mouse1 an AimKey\n"
		"Isn't compatible with AutoShoot");
CatVar projectile(CV_SWITCH, "aimbot_projectile", "1", "Projectile aimbot", NULL,
		"If you turn it off, aimbot won't try to aim "
		"with projectile weapons");
CatVar projectile_speed(CV_FLOAT, "aimbot_proj_speed", "0", "Projectile speed", NULL,
		"Force override projectile speed.\n"
		"Can be useful for playing with MvM upgrades or on x10 servers "
		"since there is no \"automatic\" projectile speed detection in "
		"cathook. Yet.");
CatVar projectile_gravity(CV_FLOAT, "aimbot_proj_gravity", "0", "Projectile gravity", NULL,
		"Force override projectile gravity. Useful for debugging.", true); // TODO IMPLEMENT
CatVar canshoot(CV_SWITCH, "aimbot_only_when_can_shoot", "1", "Active when can shoot", NULL,
		"Aimbot only activates when you can instantly shoot, sometimes making the autoshoot invisible for spectators");
CatVar priority(CV_ENUM, "aimbot_prioritymode", "0", "Priority mode",
		new CatEnum({ "SMART", "FOV", "DISTANCE", "HEALTH" }),
		"Priority mode.\n"
		"SMART: Basically Auto-Threat. Will be tweakable eventually.\n"
		"FOV, DISTANCE, HEALTH are self-explainable.\n"
		"HEALTH picks the weakest enemy");
CatVar projectile_visibility(CV_SWITCH, "aimbot_proj_vispred", "0", "Projectile visibility prediction", NULL,
		"If disabled, aimbot won't lock at enemies that are behind walls, but will come out soon");
CatVar projectile_fov(CV_SWITCH, "aimbot_proj_fovpred", "0", "Projectile FOV mode", NULL,
		"If disabled, FOV restrictions apply to current target position");

bool active_this_tick = false;

bool ShouldTarget(CUserCmd* cmd, CachedEntity& entity) {
	if (!targeting::ShouldTarget(entity)) return false;
	if (entity.Distance() > range) return false;
	return true;
}

bool ShouldActivate(CUserCmd* cmd) {
	if (!enabled) return false;
	if (aimkey && aimkey_mode) {
		static bool aimkey_toggle = false;
		bool down = g_IInputSystem->IsButtonDown((ButtonCode_t)(int)aimkey);
		switch (aimkey_mode) {
		case k_EAimkeyMode::PRESS_TO_DISABLE:
			if (down) return false;
			break;
		case k_EAimkeyMode::PRESS_TO_ENABLE:
			if (!down) return false;
			break;
		case k_EAimkeyMode::PRESS_TO_TOGGLE:
			aimkey_toggle = !aimkey_toggle;
			if (!aimkey_toggle) return false;
		}
	}
	if (canshoot) {
		if (g_LocalPlayer.weapon()->clazz != g_pClassID->CTFMinigun) {
			if (!CanShoot()) return false; // TODO player canshoot
		}
	}
	if (attack_only && !(cmd->buttons & IN_ATTACK)) return false;
	if (g_LocalPlayer.weapon()->clazz == g_pClassID->CTFMinigun) {
		if (!HasCondition(g_LocalPlayer.entity, TFCond_Slowed)) return false;
		//if (!(cmd->buttons & IN_ATTACK2)) return false; TODO minigunstate
	}
	if (IsAmbassador(g_LocalPlayer.weapon())) {
		if ((g_pGlobals->curtime - g_LocalPlayer.weapon()->var<float>(netvar.flLastFireTime)) <= 0.825) {
			return false;
		}
	}
	if (g_LocalPlayer.bZoomed) {
		if (g_LocalPlayer.weapon()->clazz == g_pClassID->CTFSniperRifle ||
			g_LocalPlayer.weapon()->clazz == g_pClassID->CTFSniperRifleDecap) {
			if (!CanHeadshot()) return false;
		}
	}
	return true;
}

void ProcessEntity(CUserCmd* cmd, CachedEntity& entity) {
	if (!active_this_tick) return;
	//if (!enabled) return;
	if (!targeting::ShouldTarget(entity)) return;
}

void DoAimbot(CUserCmd* cmd) {
	if (!active_this_tick) return;
}

}}}

/*bool Aimbot::ShouldAim(CUserCmd* cmd) {
	if (g_LocalPlayer->weapon()->clazz == g_pClassID->CTFMinigun) {
		if (!HasCondition(g_LocalPlayer->entity, TFCond_Slowed)) {
			return false;
		}
		if (!(cmd->buttons & IN_ATTACK2)) {
			return false;
		}
		if (m_nMinigunFixTicks > 0) {
			m_nMinigunFixTicks--;
			cmd->buttons |= IN_ATTACK;
		}
	}

	if (IsAmbassador(g_LocalPlayer->weapon())) { // TODO AmbassadorCanHeadshot()
		if ((g_pGlobals->curtime - CE_FLOAT(g_LocalPlayer->weapon(), netvar.flLastFireTime)) <= 1.0) {
			return false;
		}
	}
	if (g_LocalPlayer->bZoomed) {
		// TODO IsSniperRifle()
		if (g_LocalPlayer->weapon()->clazz == g_pClassID->CTFSniperRifle ||
			g_LocalPlayer->weapon()->clazz == g_pClassID->CTFSniperRifleDecap) {
			if (!CanHeadshot()) return false;
		}
	}
	return true;
}
*/
void Aimbot::ProcessUserCmd(CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return;
	if (CE_BAD(g_LocalPlayer->entity) || CE_BAD(g_LocalPlayer->weapon())) return;
	if (g_LocalPlayer->life_state) return;
	//this->m_iLastTarget = -1;

	if (HasCondition(g_LocalPlayer->entity, TFCond_Taunting)) return;

	switch (GetWeaponMode(g_LocalPlayer->entity)) {
	case weapon_medigun:
	case weapon_pda:
	case weapon_consumable:
	case weapon_throwable:
	case weapon_invalid:
		return;
	case weapon_projectile:
		if (!v_bProjectileAimbot->GetBool()) return;
	};

	if (HasCondition(g_LocalPlayer->entity, TFCond_Cloaked)) return; // TODO other kinds of cloak
	// TODO m_bFeignDeathReady no aim

	if(cmd->buttons & IN_USE) return;

	/*if (this->v_bTriggerMode->GetBool() ) {
		cmd->buttons = cmd->buttons &~ IN_ATTACK;
	}*/

	m_bHeadOnly = false;

	m_iPreferredHitbox = this->v_eHitbox->GetInt();
	if (this->v_bAutoHitbox->GetBool()) {
		int ci = g_LocalPlayer->weapon()->clazz;
		if (ci == g_pClassID->CTFSniperRifle ||
			ci == g_pClassID->CTFSniperRifleDecap) {
			m_bHeadOnly = CanHeadshot();
		} else if (ci == g_pClassID->CTFCompoundBow) {
			m_bHeadOnly = true;
		} else if (ci == g_pClassID->CTFRevolver) {
			m_bHeadOnly = IsAmbassador(g_LocalPlayer->weapon());
		} else if (ci == g_pClassID->CTFRocketLauncher ||
				ci == g_pClassID->CTFRocketLauncher_AirStrike ||
				ci == g_pClassID->CTFRocketLauncher_DirectHit ||
				ci == g_pClassID->CTFRocketLauncher_Mortar) {
			m_iPreferredHitbox = hitbox_t::foot_L;
		} else {
			m_iPreferredHitbox = hitbox_t::pelvis;
		}
	}

	if (g_LocalPlayer->weapon()->clazz == g_pClassID->CTFGrapplingHook) return;

	m_bProjectileMode = (GetProjectileData(g_LocalPlayer->weapon(), m_flProjSpeed, m_flProjGravity));
	if (v_fOverrideProjSpeed->GetBool())
		m_flProjSpeed = v_fOverrideProjSpeed->GetFloat();
	if (v_fOverrideProjGravity->GetBool())
		m_flProjGravity = v_fOverrideProjGravity->GetFloat();

	// TODO priority modes (FOV, Smart, Distance, etc)
	CachedEntity* target_highest = 0;
	float target_highest_score = -256;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		int tg = ShouldTarget(ent);
		if (!tg) {
			if (GetWeaponMode(g_LocalPlayer->entity) == k_EWeaponmode::weapon_melee || this->v_ePriorityMode->GetInt() == 2) {
				Vector result;
				if (ent->m_Type == ENTITY_BUILDING) {
					result = GetBuildingPosition(ent);
				} else {
					GetHitbox(ent, BestHitbox(ent, m_iPreferredHitbox), result);
				}
				float scr = 4096.0f - result.DistTo(g_LocalPlayer->v_Eye);
				if (scr > target_highest_score) {
					target_highest_score = scr;
					target_highest = ent;
				}
			} else {
				switch (this->v_ePriorityMode->GetInt()) {
				case 0: {
					int scr = GetScoreForEntity(ent);
					if (scr > target_highest_score) {
						target_highest_score = scr;
						target_highest = ent;
					}
				} break;
				case 1: {
					Vector result;
					if (ent->m_Type == ENTITY_BUILDING) {
						result = GetBuildingPosition(ent);
					} else {
						GetHitbox(ent, BestHitbox(ent, m_iPreferredHitbox), result);
					}
					float scr = 360.0f - GetFov(g_LocalPlayer->v_OrigViewangles, g_LocalPlayer->v_Eye, result);
					if (scr > target_highest_score) {
						target_highest_score = scr;
						target_highest = ent;
					}
				} break;
				case 3: {
					float scr;
					if (ent->m_Type == ENTITY_BUILDING) {
						scr = 450.0f - CE_INT(ent, netvar.iBuildingHealth);
					} else {
						scr = 450.0f - CE_INT(ent, netvar.iHealth);
					}
					if (scr > target_highest_score) {
						target_highest_score = scr;
						target_highest = ent;
					}
				}
				}
			}
		} else {
			//if (tg != 26)
			//logging::Info("Shouldn't target ent %i %i", ent->m_IDX, tg);
		}
	}
	if (CE_GOOD(target_highest)) {
		target_highest->m_ESPColorFG = colors::pink;
		if (ShouldAim(cmd)) {
			this->m_iLastTarget = target_highest->m_IDX;
			if (g_LocalPlayer->weapon()->clazz == g_pClassID->CTFCompoundBow) { // There is no Huntsman in TF2C.
				float begincharge = CE_FLOAT(g_LocalPlayer->weapon(), netvar.flChargeBeginTime);
				float charge = 0;
				if (begincharge != 0) {
					charge = g_pGlobals->curtime - begincharge;
					if (charge > 1.0f) charge = 1.0f;
					m_bSilentHuntsman = true;
				}
				if (charge >= v_fAutoShootHuntsmanCharge->GetFloat()) {
					cmd->buttons &= ~IN_ATTACK;
					g_phAntiAim->AddSafeTicks(3);
				}
				if (!(cmd->buttons & IN_ATTACK) && m_bSilentHuntsman) {
					Aim(target_highest, cmd);
					m_bSilentHuntsman = false;
				}
			} else {
				Aim(target_highest, cmd);
			}
			if (g_LocalPlayer->weapon()->clazz == g_pClassID->CTFMinigun)
				m_nMinigunFixTicks = 40;
		}
	}
	if (g_LocalPlayer->weapon()->clazz == g_pClassID->CTFMinigun &&
			target_highest == 0 &&
			IDX_GOOD(m_iLastTarget) &&
			m_nMinigunFixTicks && ShouldAim(cmd)) {
		Aim(ENTITY(m_iLastTarget), cmd);
	}
	if (this->v_bSilent->GetBool()) g_LocalPlayer->bUseSilentAngles = true;
	return;
}

int Aimbot::BestHitbox(CachedEntity* target, int preferred) {
	if (!v_bAutoHitbox->GetBool()) return preferred;
	if (m_bHeadOnly) return hitbox_t::head;
	int flags = CE_INT(target, netvar.iFlags);
	bool ground = (flags & (1 << 0));
	if (!ground) {
		if (GetWeaponMode(g_LocalPlayer->entity) == k_EWeaponmode::weapon_projectile) {
			if (g_LocalPlayer->weapon()->clazz != g_pClassID->CTFCompoundBow) {
				preferred = hitbox_t::spine_3;
			}
		}
	}
	if (target->m_pHitboxCache->VisibilityCheck(preferred)) return preferred;
	for (int i = m_bProjectileMode ? 1 : 0; i < target->m_pHitboxCache->HitboxCount(); i++) {
		if (target->m_pHitboxCache->VisibilityCheck(i)) return i;
	}
	return -1;
}

int Aimbot::ShouldTarget(CachedEntity* entity) {
	// Just assuming CE is good
	if (entity->m_Type == ENTITY_PLAYER) {
		if (TF) {
			if (g_Settings.bIgnoreTaunting->GetBool() && HasCondition(entity, TFCond_Taunting)) return 1;
			if (IsPlayerInvulnerable(entity)) return 4;
			if (v_bRespectCloak->GetBool() && IsPlayerInvisible(entity)) return 6;
			k_EWeaponmode mode = GetWeaponMode(LOCAL_E);
			if (mode == k_EWeaponmode::weapon_hitscan || LOCAL_W->clazz == g_pClassID->CTFCompoundBow)
				if (HasCondition(entity, TFCond_UberBulletResist)) return 10;
		}

#if NO_DEVIGNORE != true
		if (Developer(entity)) return 2; // TODO developer relation
#endif
		if (entity->m_lSeenTicks < (unsigned)this->v_iSeenDelay->GetInt()) return 3;
		if (!entity->m_bAlivePlayer) return 5;
		if (!entity->m_bEnemy && !v_bAimAtTeammates->GetBool()) return 7;
		if (v_iMaxRange->GetInt() > 0) {
			if (entity->m_flDistance > v_iMaxRange->GetInt()) return 8;
		}
		if (GetWeaponMode(g_LocalPlayer->entity) == k_EWeaponmode::weapon_melee) {
			if (entity->m_flDistance > 95) return 9;
		}
		if (GetRelation(entity) == relation::FRIEND) return 11;
		Vector resultAim;
		int hitbox = BestHitbox(entity, m_iPreferredHitbox);
		if (m_bHeadOnly && hitbox) return 12;
		if (m_bProjectileMode) {
			if (v_bProjPredFOV->GetBool()) {
				if (v_bProjPredVisibility->GetBool()) {
					if (!GetHitbox(entity, hitbox, resultAim)) return 13;
					if (!IsEntityVisible(entity, hitbox)) return 14;
				}
				resultAim = ProjectilePrediction(entity, hitbox, m_flProjSpeed, m_flProjGravity, PlayerGravityMod(entity));
			} else {
				if (!GetHitbox(entity, hitbox, resultAim)) return 15;
			}
			if (!IsVectorVisible(g_LocalPlayer->v_Eye, resultAim)) return 16;
		} else {
			/*if (v_bMachinaPenetration->GetBool()) {
				if (!GetHitbox(entity, hitbox, resultAim)) return false;
				if (!IsEntityVisiblePenetration(entity, v_eHitbox->GetInt())) return false;
			} else*/ {
				if (!GetHitbox(entity, hitbox, resultAim)) return 17;
				if (!IsEntityVisible(entity, hitbox)) return 18;
			}
		}
		if (v_fFOV->GetFloat() > 0.0f && (GetFov(g_LocalPlayer->v_OrigViewangles, g_LocalPlayer->v_Eye, resultAim) > v_fFOV->GetFloat())) return 25;
		return false;
	} else if (entity->m_Type == ENTITY_BUILDING) {
		if (!v_bAimBuildings->GetBool()) return 19;
		int team = CE_INT(entity, netvar.iTeamNum);
		if (team == g_LocalPlayer->team) return 20;
		if (v_iMaxRange->GetInt() > 0) {
			if (entity->m_flDistance > v_iMaxRange->GetInt()) return 21;
		}
		if (GetWeaponMode(g_LocalPlayer->entity) == k_EWeaponmode::weapon_melee) {
			if (entity->m_flDistance > 95) return 22;
		}
		Vector resultAim;
		// TODO fix proj buildings
		if (m_bProjectileMode) {
			if (!IsBuildingVisible(entity)) return 23;
			resultAim = GetBuildingPosition(entity);
			//resultAim = entity->GetAbsOrigin();
			//if (!PredictProjectileAim(g_pLocalPlayer->v_Eye, entity, (hitbox_t)m_iHitbox, m_flProjSpeed, m_bProjArc, m_flProjGravity, resultAim)) return false;
		} else {
			//logging::Info("IsVisible?");
			resultAim = GetBuildingPosition(entity);
			if (!IsBuildingVisible(entity)) return 24;
		}
		//logging::Info("IsFOV?");
		if (v_fFOV->GetFloat() > 0.0f && (GetFov(g_LocalPlayer->v_OrigViewangles, g_LocalPlayer->v_Eye, resultAim) > v_fFOV->GetFloat())) return 25;
		//logging::Info("Tru");
		return 0;
	} else {
		return 26;
	}
	return 27;
}

// TODO Vector objects
bool Aimbot::Aim(CachedEntity* entity, CUserCmd* cmd) {
	//logging::Info("Aiming!");
	Vector hit;
	Vector angles;
	if (CE_BAD(entity)) return false;
	int hitbox = BestHitbox(entity, m_iPreferredHitbox);
	//if (m_bHeadOnly) hitbox = 0;
	if (entity->m_Type == ENTITY_PLAYER) {
		//logging::Info("A");
		GetHitbox(entity, hitbox, hit);
		//logging::Info("B");
		if (this->v_bInterpolation->GetBool()) SimpleLatencyPrediction(entity, hitbox);
		//logging::Info("C");
	} else if (entity->m_Type == ENTITY_BUILDING) {
		hit = GetBuildingPosition(entity);
	}
	if (m_bProjectileMode) {
		if (v_fOverrideProjSpeed->GetBool())
			m_flProjSpeed = v_fOverrideProjSpeed->GetFloat();
		if (v_fOverrideProjGravity->GetBool())
			m_flProjGravity = v_fOverrideProjGravity->GetFloat();
		bool succ = false;
		hit = ProjectilePrediction(entity, hitbox, m_flProjSpeed, m_flProjGravity, PlayerGravityMod(entity));
	}
	//logging::Info("ayyming!");
	Vector tr = (hit - g_LocalPlayer->v_Eye);
	fVectorAngles(tr, angles);
	bool smoothed = false;
	/*if (this->v_bSmooth->GetBool()) {
		Vector da = (angles - g_pLocalPlayer->v_OrigViewangles);
		fClampAngle(da);
		smoothed = true;
		if (da.IsZero(v_fSmoothAutoshootTreshold->GetFloat())) smoothed = false;
		da *= this->v_fSmoothValue->GetFloat() * (((float)rand() / (float)RAND_MAX) * this->v_fSmoothRandomness->GetFloat());
		angles = g_pLocalPlayer->v_OrigViewangles + da;
	}*/
	fClampAngle(angles);
	cmd->viewangles = angles;
	if (this->v_bSilent->GetBool()) {
		g_LocalPlayer->bUseSilentAngles = true;
	}
	if (this->v_bAutoShoot->GetBool()) {
		if (g_LocalPlayer->clazz == tf_class::tf_sniper) {
			if (g_LocalPlayer->weapon()->clazz == g_pClassID->CTFSniperRifle || g_LocalPlayer->weapon()->clazz == g_pClassID->CTFSniperRifleDecap) {
				if (v_bZoomedOnly->GetBool() && !CanHeadshot()) return true;
			}
		}
		if (g_LocalPlayer->weapon()->clazz != g_pClassID->CTFCompoundBow) {
			cmd->buttons |= IN_ATTACK;
		}
	}
	return true;
}

void Aimbot::OnLevelInit() {
	m_iLastTarget = -1;
	m_bProjectileMode = false;
}
