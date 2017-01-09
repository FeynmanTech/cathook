/*
 * HAimbot.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: nullifiedcat
 */

#include "../common.h"
#include "../trace.h"
#include "../targethelper.h"

#include "../targeting/ITargetSystem.h"
#include "../targeting/TargetSystemSmart.h"
#include "../targeting/TargetSystemFOV.h"
#include "../targeting/TargetSystemDistance.h"

#include "../sdk.h"
#include "../sdk/in_buttons.h"
#include "Aimbot.h"

DEFINE_HACK_SINGLETON(Aimbot);

Vector viewangles_old;
bool fix_silent;

int target_lock;

enum TargetSystem_t {
	SMART = 0,
	FOV = 1,
	DISTANCE = 2
};

ITargetSystem* target_systems[3];

const char* Aimbot::GetName() {
	return "AIMBOT";
}

const char* psza__AimKeyMode[] = { "DISABLED", "AIMKEY", "REVERSE", "TOGGLE" };
const char* psza__Hitbox[] = {
	"HEAD", "PELVIS", "SPINE 0", "SPINE 1", "SPINE 2", "SPINE 3", "UPPER ARM L", "LOWER ARM L",
	"HAND L", "UPPER ARM R", "LOWER ARM R", "HAND R", "HIP L", "KNEE L", "FOOT L", "HIP R",
	"KNEE R", "FOOT R"
};
const char* psza__Priority[] = {
	"SMART", "FOV", "DISTANCE", "HEALTH"
};

Aimbot::Aimbot() {
	target_systems[0] = new TargetSystemSmart();
	target_systems[1] = new TargetSystemFOV();
	target_systems[2] = new TargetSystemDistance();
	m_bAimKeySwitch = false;
	this->v_eAimKeyMode = CREATE_CV(new CatEnum(psza__AimKeyMode, ARRAYSIZE(psza__AimKeyMode)), "aimbot_aimkey_mode", "1", "Aimkey Mode");
	this->v_bEnabled = CREATE_CV(CV_SWITCH, "aimbot_enabled", "0", "Enabled");
	this->v_eHitbox = CREATE_CV(new CatEnum(psza__Hitbox, ARRAYSIZE(psza__Hitbox)), "aimbot_hitbox", "0", "Hitbox");
	this->v_bAutoHitbox = CREATE_CV(CV_SWITCH, "aimbot_autohitbox", "1", "Autohitbox");
	this->v_bPrediction = CREATE_CV(CV_SWITCH, "aimbot_prediction", "1", "Latency pred");
	this->v_bAutoShoot = CREATE_CV(CV_SWITCH, "aimbot_autoshoot", "1", "Autoshoot");
	this->v_bSilent = CREATE_CV(CV_SWITCH, "aimbot_silent", "1", "Silent");
	this->v_bZoomedOnly = CREATE_CV(CV_SWITCH, "aimbot_zoomed", "1", "Zoomed Only");
	this->v_iAutoShootCharge = CREATE_CV(CV_FLOAT, "aimbot_autoshoot_charge", "0.0", "Autoshoot Charge");
	this->v_iMaxRange = CREATE_CV(CV_INT, "aimbot_maxrange", "0", "Max distance");
	this->v_bRespectCloak = CREATE_CV(CV_SWITCH, "aimbot_respect_cloak", "1", "Respect cloak");
	this->v_bCharge = CREATE_CV(CV_SWITCH, "aimbot_charge", "0", "Wait for charge");
	this->v_bEnabledAttacking = CREATE_CV(CV_SWITCH, "aimbot_enable_attack_only", "0", "Active when attacking");
	this->v_bTriggerMode = CREATE_CV(CV_SWITCH, "aimbot_triggerlock", "0", "Trigger lock");
	this->v_bProjectileAimbot = CREATE_CV(CV_SWITCH, "aimbot_projectile", "1", "Projectile aimbot");
	this->v_fOverrideProjSpeed = CREATE_CV(CV_FLOAT, "aimbot_proj_speed", "0", "Projectile speed");
	this->v_bDebug = CREATE_CV(CV_SWITCH, "aimbot_debug", "0", "Debug");
	this->v_fFOV = CREATE_CV(CV_FLOAT, "aimbot_fov", "0", "FOV");
	this->v_bMachinaPenetration = CREATE_CV(CV_SWITCH, "aimbot_machina", "0", "Machina Mode");
	this->v_bSmooth = CREATE_CV(CV_SWITCH, "aimbot_smooth", "0", "Smooth");
	this->v_fAutoShootHuntsmanCharge = CREATE_CV(CV_FLOAT, "aimbot_huntsman_charge", "0.5", "Huntsman charge");
	this->v_fSmoothValue = CREATE_CV(CV_FLOAT, "aimbot_smooth_value", "0.2", "Smooth value");
	this->v_eAimKey = CREATE_CV(CV_INT, "aimbot_aimkey", "0", "Aimkey");
	this->v_ePriorityMode = CREATE_CV(new CatEnum(psza__Priority, ARRAYSIZE(psza__Priority)), "aimbot_prioritymode", "0", "Priority mode");
	v_bAimBuildings = CREATE_CV(CV_SWITCH, "aimbot_buildings", "1", "Aim @ Buildings");
	v_bActiveOnlyWhenCanShoot = CREATE_CV(CV_SWITCH, "aimbot_only_when_can_shoot", "1", "Active when can shoot");
	v_fSmoothAutoshootTreshold = CREATE_CV(CV_FLOAT, "aimbot_smooth_autoshoot_treshold", "0.01", "Smooth autoshoot");
	this->v_fSmoothRandomness = CREATE_CV(CV_FLOAT, "aimbot_smooth_randomness", "1.0", "Smooth randomness");
	this->v_iSeenDelay = CREATE_CV(CV_INT, "aimbot_delay", "0", "Aimbot delay");
	fix_silent = false;
}

bool Aimbot::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (CE_BAD(g_pLocalPlayer->entity) || CE_BAD(g_pLocalPlayer->weapon())) return true;
	if (g_pLocalPlayer->life_state) return true;
	//this->m_iLastTarget = -1;
	if (this->v_eAimKey->GetBool() && this->v_eAimKeyMode->GetBool()) {
		bool key_down = interfaces::input->IsButtonDown((ButtonCode_t)this->v_eAimKey->GetInt());
		switch (this->v_eAimKeyMode->GetInt()) {
		case AimKeyMode_t::PRESS_TO_ENABLE:
			if (key_down) break;
			else return true;
		case AimKeyMode_t::PRESS_TO_DISABLE:
			if (key_down) return true;
			else break;
		case AimKeyMode_t::PRESS_TO_TOGGLE:
			m_bAimKeySwitch = !m_bAimKeySwitch;
			if (!m_bAimKeySwitch) return true;
		}
	}

	if (g_pLocalPlayer->cond_0 & cond::taunting) return true;

	switch (GetWeaponMode(g_pLocalPlayer->entity)) {
	case weapon_medigun:
	case weapon_pda:
	case weapon_consumable:
	case weapon_throwable:
	case weapon_invalid:
		return true;
	};

	if (g_pLocalPlayer->cond_0 & cond::cloaked) return true; // TODO other kinds of cloak
	// TODO m_bFeignDeathReady no aim
	if (this->v_bActiveOnlyWhenCanShoot->GetBool()) {
		// Miniguns should shoot and aim continiously. TODO smg
		if (g_pLocalPlayer->weapon()->m_iClassID != ClassID::CTFMinigun) {
			// Melees are weird, they should aim continiously like miniguns too.
			if (GetWeaponMode(g_pLocalPlayer->entity) != weaponmode::weapon_melee) {
				// Finally, CanShoot() check.
				if (!CanShoot()) return true;
			}
		}
	}

	if (this->v_bEnabledAttacking->GetBool() && !(cmd->buttons & IN_ATTACK)) {
		return true;
	}

	if (g_pLocalPlayer->weapon()->m_iClassID == ClassID::CTFMinigun) {
		if (!(g_pLocalPlayer->cond_0 & cond::slowed)) {
			return true;
		}
		if (!(cmd->buttons & IN_ATTACK2)) {
			return true;
		}
		if (m_nMinigunFixTicks > 0) {
			m_nMinigunFixTicks--;
			cmd->buttons |= IN_ATTACK;
		}
	}

	if (IsAmbassador(g_pLocalPlayer->weapon())) { // TODO AmbassadorCanHeadshot()
		if ((interfaces::gvars->curtime - CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flLastFireTime)) <= 1.0) {
			return true;
		}
	}

	if(cmd->buttons & IN_USE) return true;

	if (this->v_bTriggerMode->GetBool() ) {
		cmd->buttons = cmd->buttons &~ IN_ATTACK;
	}

	m_bHeadOnly = false;

	m_iPreferredHitbox = this->v_eHitbox->GetInt();
	if (this->v_bAutoHitbox->GetBool()) {
		switch (g_pLocalPlayer->weapon()->m_iClassID) {
		case ClassID::CTFSniperRifle:
		case ClassID::CTFSniperRifleDecap:
			m_bHeadOnly = CanHeadshot();
		break;
		case ClassID::CTFCompoundBow:
			m_bHeadOnly = true;
		break;
		case ClassID::CTFRevolver:
			m_bHeadOnly = IsAmbassador(g_pLocalPlayer->weapon());
		break;
		case ClassID::CTFRocketLauncher:
		case ClassID::CTFRocketLauncher_AirStrike:
		case ClassID::CTFRocketLauncher_DirectHit:
		case ClassID::CTFRocketLauncher_Mortar:
			m_iPreferredHitbox = hitbox_t::foot_L;
		break;
		default:
			m_iPreferredHitbox = hitbox_t::pelvis;
		}
	}

	if (this->v_bZoomedOnly->GetBool()) {
		// TODO IsSniperRifle()
		if (g_pLocalPlayer->weapon()->m_iClassID == ClassID::CTFSniperRifle ||
			g_pLocalPlayer->weapon()->m_iClassID == ClassID::CTFSniperRifleDecap) {
			if (!CanHeadshot()) return true;
		}
	}

	if (g_pLocalPlayer->weapon()->m_iClassID == ClassID::CTFGrapplingHook) return true;

	m_bProjectileMode = (GetProjectileData(g_pLocalPlayer->weapon(), m_flProjSpeed, m_flProjGravity));
	// TODO priority modes (FOV, Smart, Distance, etc)
	CachedEntity* target_highest = 0;
	float target_highest_score = -256;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		if (ShouldTarget(ent)) {
			if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee || this->v_ePriorityMode->GetInt() == 2) {
				Vector result;
				if (ent->m_Type == ENTITY_BUILDING) {
					result = GetBuildingPosition(ent);
				} else {
					GetHitbox(ent, BestHitbox(ent, m_iPreferredHitbox), result);
				}
				float scr = 4096.0f - result.DistTo(g_pLocalPlayer->v_Eye);
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
					float scr = 360.0f - GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, result);
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
		}
	}
	if (target_highest != 0) {
		this->m_iLastTarget = target_highest->m_IDX;
		Aim(target_highest, cmd);
		if (g_pLocalPlayer->weapon()->m_iClassID == ClassID::CTFMinigun)
			m_nMinigunFixTicks = 40;
	}
	if (g_pLocalPlayer->weapon()->m_iClassID == ClassID::CTFMinigun &&
			target_highest == 0 &&
			IDX_GOOD(m_iLastTarget) &&
			m_nMinigunFixTicks) {
		Aim(ENTITY(m_iLastTarget), cmd);
	}
	return !this->v_bSilent->GetBool();
}

void Aimbot::PaintTraverse(void*, unsigned int, bool, bool) {
	if (!v_bEnabled->GetBool()) return;
	if (IDX_BAD(m_iLastTarget)) return;
	CachedEntity* ent = ENTITY(this->m_iLastTarget);
	if (CE_BAD(ent)) return;
	if (ent->m_Type == ENTITY_PLAYER) {
		int clazz = CE_INT(ent, netvar.iClass);
		if (clazz < 0 || clazz > 9) return;
		AddCenterString(colors::yellow, colors::black, "Prey: %i HP %s (%s)", CE_INT(ent, netvar.iHealth), tfclasses[clazz], ent->m_pPlayerInfo->name);
	} else if (ent->m_Type == ENTITY_BUILDING) {
		AddCenterString(colors::yellow, colors::black, "Prey: %i HP LV %i %s", CE_INT(ent, netvar.iBuildingHealth), CE_INT(ent, netvar.iUpgradeLevel), GetBuildingName(ent));
	}
}

int Aimbot::BestHitbox(CachedEntity* target, int preferred) {
	if (!v_bAutoHitbox->GetBool()) return preferred;
	if (m_bHeadOnly) return 0;
	if (target->m_pHitboxCache->VisibilityCheck(preferred)) return preferred;
	for (int i = 0; i < target->m_pHitboxCache->m_nNumHitboxes; i++) {
		if (target->m_pHitboxCache->VisibilityCheck(i)) return i;
	}
	return -1;
}

bool Aimbot::ShouldTarget(CachedEntity* entity) {
	// Just assuming CE is good
	if (entity->m_Type == ENTITY_PLAYER) {
		if (g_Settings.bIgnoreTaunting->GetBool() && (CE_INT(entity, netvar.iCond) & cond::taunting)) return false;
		if (Developer(entity)) return false; // TODO developer relation
		if (entity->m_lSeenTicks < (unsigned)this->v_iSeenDelay->GetInt()) return false;
		if (IsPlayerInvulnerable(entity)) return false;
		if (!entity->m_bAlivePlayer) return false;
		if (v_bRespectCloak->GetBool() && IsPlayerInvisible(entity)) return false;
		if (!entity->m_bEnemy) return false;
		if (v_iMaxRange->GetInt() > 0) {
			if (entity->m_flDistance > v_iMaxRange->GetInt()) return false;
		}
		if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee) {
			if (entity->m_flDistance > 95) return false;
		}
		int econd = CE_INT(entity, netvar.iCond1);
		if ((econd & cond_ex::vacc_bullet)) return false;
		if (GetRelation(entity) == relation::FRIEND) return false;
		Vector resultAim;
		int hitbox = BestHitbox(entity, m_iPreferredHitbox);
		if (m_bHeadOnly && hitbox) return false;
		if (m_bProjectileMode) {
			if (!IsVectorVisible(g_pLocalPlayer->v_Eye, ProjectilePrediction(entity, hitbox, m_flProjSpeed, m_flProjGravity))) return false;
		} else {
			if (v_bMachinaPenetration->GetBool()) {
				if (!GetHitbox(entity, hitbox, resultAim)) return false;
				if (!IsEntityVisiblePenetration(entity, v_eHitbox->GetInt())) return false;
			} else {
				if (!GetHitbox(entity, hitbox, resultAim)) return false;
				if (!IsEntityVisible(entity, hitbox)) return false;
			}
		}
		if (v_fFOV->GetFloat() > 0.0f && (GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, resultAim) > v_fFOV->GetFloat())) return false;
		return true;
	} else if (entity->m_Type == ENTITY_BUILDING) {
		if (!v_bAimBuildings->GetBool()) return false;
		int team = CE_INT(entity, netvar.iTeamNum);
		if (team == g_pLocalPlayer->team) return false;
		if (v_iMaxRange->GetInt() > 0) {
			if (entity->m_flDistance > v_iMaxRange->GetInt()) return false;
		}
		if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee) {
			if (entity->m_flDistance > 95) return false;
		}
		Vector resultAim;
		// TODO fix proj buildings
		if (m_bProjectileMode) {
			return false;
			//resultAim = entity->GetAbsOrigin();
			//if (!PredictProjectileAim(g_pLocalPlayer->v_Eye, entity, (hitbox_t)m_iHitbox, m_flProjSpeed, m_bProjArc, m_flProjGravity, resultAim)) return false;
		} else {
			//logging::Info("IsVisible?");
			if (!IsBuildingVisible(entity)) return false;
		}
		//logging::Info("IsFOV?");
		if (v_fFOV->GetFloat() > 0.0f && (GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, resultAim) > v_fFOV->GetFloat())) return false;
		//logging::Info("Tru");
		return true;
	} else {
		return false;
	}
	return false;
}

// TODO Vector objects
bool Aimbot::Aim(CachedEntity* entity, CUserCmd* cmd) {
	//logging::Info("Aiming!");
	Vector hit;
	Vector angles;
	if (CE_BAD(entity)) return false;
	int hitbox = BestHitbox(entity, m_iPreferredHitbox);
	if (entity->m_Type == ENTITY_PLAYER) {
		//logging::Info("A");
		GetHitbox(entity, hitbox, hit);
		//logging::Info("B");
		if (this->v_bPrediction->GetBool()) SimpleLatencyPrediction(entity, hitbox);
		//logging::Info("C");
	} else if (entity->m_Type == ENTITY_BUILDING) {
		hit = GetBuildingPosition(entity);
	}
	if (v_bProjectileAimbot->GetBool()) {
		if (m_bProjectileMode) {
			if (v_fOverrideProjSpeed->GetBool())
				m_flProjSpeed = v_fOverrideProjSpeed->GetFloat();
			hit = ProjectilePrediction(entity, hitbox, m_flProjSpeed, m_flProjGravity);
		}
	}
	//logging::Info("ayyming!");
	Vector tr = (hit - g_pLocalPlayer->v_Eye);
	fVectorAngles(tr, angles);
	bool smoothed = false;
	if (this->v_bSmooth->GetBool()) {
		Vector da = (angles - g_pLocalPlayer->v_OrigViewangles);
		fClampAngle(da);
		smoothed = true;
		if (da.IsZero(v_fSmoothAutoshootTreshold->GetFloat())) smoothed = false;
		da *= this->v_fSmoothValue->GetFloat() * (((float)rand() / (float)RAND_MAX) * this->v_fSmoothRandomness->GetFloat());
		angles = g_pLocalPlayer->v_OrigViewangles + da;
	}
	fClampAngle(angles);
	cmd->viewangles = angles;
	if (this->v_bSilent->GetBool()) {
		g_pLocalPlayer->bUseSilentAngles = true;
	}
	if (!smoothed && this->v_bAutoShoot->GetBool()) {
		if (g_pLocalPlayer->clazz == tf_class::tf_sniper) {
			if (g_pLocalPlayer->bZoomed) {
				if (this->v_iAutoShootCharge->GetBool()) {
					float bdmg = CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flChargedDamage);
					if (bdmg < this->v_iAutoShootCharge->GetFloat()) return true;
				} else {
					if (!CanHeadshot()) return true;
				}
			}
		}
		if (g_pLocalPlayer->weapon()->m_iClassID == ClassID::CTFCompoundBow) {
			float begincharge = CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flChargeBeginTime);
			float charge = 0;
			if (begincharge != 0) {
				charge = interfaces::gvars->curtime - begincharge;
				if (charge > 1.0f) charge = 1.0f;
			}
			if (charge >= v_fAutoShootHuntsmanCharge->GetFloat()) {
				cmd->buttons &= ~IN_ATTACK;
			}
		} else {
			cmd->buttons |= IN_ATTACK;
		}
	}
	return true;
}
