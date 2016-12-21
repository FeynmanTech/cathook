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

/* null-safe */

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
	if (g_pLocalPlayer->entity && g_pLocalPlayer->life_state) return true;
	this->m_iLastTarget = -1;
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
	if (g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID != ClassID::CTFMinigun)
		if (this->v_bActiveOnlyWhenCanShoot->GetBool() && !BulletTime() && !(GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee)) return true;

	if (this->v_bEnabledAttacking->GetBool() && !(cmd->buttons & IN_ATTACK)) {
		return true;
	}

	if (g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == ClassID::CTFMinigun) {
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

	if (IsAmbassador(g_pLocalPlayer->weapon)) {
		if ((interfaces::gvars->curtime - GetEntityValue<float>(g_pLocalPlayer->weapon, netvar.flLastFireTime)) <= 1.0) {
			return true;
		}
	}

	if(cmd->buttons & IN_USE) return true;

	if (this->v_bTriggerMode->GetBool() ) {
		cmd->buttons = cmd->buttons &~ IN_ATTACK;
	}
	IClientEntity* player = g_pLocalPlayer->entity;
	if (!player) return true;
	if (player->IsDormant()) return true;
	m_iHitbox = this->v_eHitbox->GetInt();
	if (this->v_bAutoHitbox->GetBool()) m_iHitbox = 7;
	if (g_pLocalPlayer->weapon && this->v_bAutoHitbox->GetBool()) {
		switch (g_pLocalPlayer->weapon->GetClientClass()->m_ClassID) {
		case ClassID::CTFSniperRifle:
		case ClassID::CTFSniperRifleDecap:
			if (!CanHeadshot(g_pLocalPlayer->entity)) {
				if (this->v_bZoomedOnly->GetBool()) return true;
			} else {
				m_iHitbox = 0;
			}
		break;
		case ClassID::CTFCompoundBow:
			m_iHitbox = 0;
		break;
		case ClassID::CTFRevolver:
			if (IsAmbassador(g_pLocalPlayer->weapon)) {
				m_iHitbox = 0;
			}
		break;
		case ClassID::CTFRocketLauncher:
		case ClassID::CTFRocketLauncher_AirStrike:
		case ClassID::CTFRocketLauncher_DirectHit:
		case ClassID::CTFRocketLauncher_Mortar:
			m_iHitbox = 14;
		}
	}

	if (this->v_bZoomedOnly->GetBool()) {
		// TODO IsSniperRifle()
		if (g_pLocalPlayer->weapon) {
			if (g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == ClassID::CTFSniperRifle ||
				g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == ClassID::CTFSniperRifleDecap) {
				if (!CanHeadshot(g_pLocalPlayer->entity)) return true;
			}
		}
	}
	if (g_pLocalPlayer->weapon) {
		if (g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == 210) return true;
	}

	m_bProjectileMode = (GetProjectileData(g_pLocalPlayer->weapon, m_flProjSpeed, m_flProjGravity));
	// TODO priority modes (FOV, Smart, Distance, etc)
	IClientEntity* target_highest = 0;
	float target_highest_score = -256;
	for (int i = 0; i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
		if (ent == 0) continue;
		if (!(IsPlayer(ent) || IsBuilding(ent))) continue;
		if (ShouldTarget(ent)) {
			if (GetWeaponMode(player) == weaponmode::weapon_melee || this->v_ePriorityMode->GetInt() == 2) {
				Vector result;
				if (IsBuilding(ent)) {
					result = GetBuildingPosition(ent);
				} else {
					GetHitboxPosition(ent, m_iHitbox, result);
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
					if (IsBuilding(ent)) {
						result = GetBuildingPosition(ent);
					} else {
						GetHitboxPosition(ent, m_iHitbox, result);
					}
					float scr = 360.0f - GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, result);
					if (scr > target_highest_score) {
						target_highest_score = scr;
						target_highest = ent;
					}
				} break;
				case 3: {
					float scr;
					if (IsBuilding(ent)) {
						scr = 450.0f - GetEntityValue<int>(ent, netvar.iBuildingHealth);
					} else {
						scr = 450.0f - GetEntityValue<int>(ent, netvar.iHealth);
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
		this->m_iLastTarget = target_highest->entindex();
		Aim(target_highest, cmd);
		if (g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == ClassID::CTFMinigun)
			m_nMinigunFixTicks = 40;
	}
	if (g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == ClassID::CTFMinigun &&
			target_highest == 0 &&
			interfaces::entityList->GetClientEntity(m_iLastTarget) &&
			m_nMinigunFixTicks) {
		Aim(interfaces::entityList->GetClientEntity(m_iLastTarget), cmd);
	}
	return !this->v_bSilent->GetBool();
}

void Aimbot::PaintTraverse(void*, unsigned int, bool, bool) {
	if (!v_bEnabled->GetBool()) return;
	if (this->m_iLastTarget == -1) return;
	IClientEntity* ent = interfaces::entityList->GetClientEntity(this->m_iLastTarget);
	if (!ent) return;
	if (IsPlayer(ent)) {
		int clazz = GetEntityValue<int>(ent, netvar.iClass);
		if (clazz < 0 || clazz > 9) return;
		player_info_t info;
		if (!interfaces::engineClient->GetPlayerInfo(this->m_iLastTarget, &info)) return;
		AddCenterString(colors::yellow, colors::black, "Prey: %i HP %s (%s)", GetEntityValue<int>(ent, netvar.iHealth), tfclasses[clazz], info.name);
	} else if (IsBuilding(ent)) {
		AddCenterString(colors::yellow, colors::black, "Prey: %i HP LV %i %s", GetEntityValue<int>(ent, netvar.iBuildingHealth), GetEntityValue<int>(ent, netvar.iUpgradeLevel), GetBuildingType(ent));
	}
}

bool Aimbot::ShouldTarget(IClientEntity* entity) {
	//logging::Info("Should target?");
	if (!entity) return false;
	if (entity->IsDormant()) return false;
	if (IsPlayer(entity)) {
		if (g_Settings.bIgnoreTaunting->GetBool() && (GetEntityValue<int>(entity, netvar.iCond) & cond::taunting)) return false;
		if (Developer(entity)) return false;
		if (gEntityCache.GetEntity(entity->entindex())->m_lSeenTicks < this->v_iSeenDelay->GetInt()) return false;
		if (IsPlayerInvulnerable(entity)) return false;
		int team = GetEntityValue<int>(entity, netvar.iTeamNum);
		int local = interfaces::engineClient->GetLocalPlayer();
		IClientEntity* player = interfaces::entityList->GetClientEntity(local);
		char life_state = GetEntityValue<char>(entity, netvar.iLifeState);
		if (life_state) return false;
		if (!player) return false;
		if (v_bRespectCloak->GetBool() && IsPlayerInvisible(entity)) return false;
		int health = GetEntityValue<int>(entity, netvar.iHealth);
		/*if (this->v_bCharge->GetBool() && (GetEntityValue<int>(player, eoffsets.iClass) == 2)) {
			int rifleHandle = GetEntityValue<int>(player, eoffsets.hActiveWeapon);
			IClientEntity* rifle = interfaces::entityList->GetClientEntity(rifleHandle & 0xFFF);
			if (!rifle) return false;
			float bdmg = GetEntityValue<float>(rifle, eoffsets.flChargedDamage);
			if (health > 150 && (health > (150 + bdmg) || bdmg < 15.0f)) return false;
		}*/
		int team_my = GetEntityValue<int>(player, netvar.iTeamNum);
		if (team == team_my) return false;
		Vector enemy_pos = entity->GetAbsOrigin();
		Vector my_pos = player->GetAbsOrigin();
		if (v_iMaxRange->GetInt() > 0) {
			if ((enemy_pos - my_pos).Length() > v_iMaxRange->GetInt()) return false;
		}
		if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee) {
			if ((enemy_pos - my_pos).Length() > 95) return false;
		}
		int econd = GetEntityValue<int>(entity, netvar.iCond1);
		if ((econd & cond_ex::vacc_bullet)) return false;
		if (GetRelation(entity) == relation::FRIEND) return false;
		Vector resultAim;
		if (m_bProjectileMode) {
			if (!IsVectorVisible(g_pLocalPlayer->v_Eye, ProjectilePrediction(entity, m_iHitbox, m_flProjSpeed, m_flProjGravity))) return false;
		} else {
			if (v_bMachinaPenetration->GetBool()) {
				if (GetHitboxPosition(entity, m_iHitbox, resultAim)) return false;
				if (!IsEntityVisiblePenetration(entity, v_eHitbox->GetInt())) return false;
			} else {
				if (GetHitboxPosition(entity, m_iHitbox, resultAim)) return false;
				if (!IsEntityVisible(entity, m_iHitbox)) return false;
			}
		}
		if (v_fFOV->GetFloat() > 0.0f && (GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, resultAim) > v_fFOV->GetFloat())) return false;
		return true;
	} else if (IsBuilding(entity)) {
		if (!v_bAimBuildings->GetBool()) return false;
		int team = GetEntityValue<int>(entity, netvar.iTeamNum);
		if (team == g_pLocalPlayer->team) return false;
		Vector enemy_pos = entity->GetAbsOrigin();
		if (v_iMaxRange->GetInt() > 0) {
			if ((enemy_pos - g_pLocalPlayer->v_Origin).Length() > v_iMaxRange->GetInt()) return false;
		}
		if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee) {
			if ((enemy_pos - g_pLocalPlayer->v_Origin).Length() > 95) return false;
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

bool Aimbot::Aim(IClientEntity* entity, CUserCmd* cmd) {
	//logging::Info("Aiming!");
	Vector hit;
	Vector angles;
	if (!entity) return false;
	if (IsPlayer(entity)) {
		//logging::Info("A");
		GetHitboxPosition(entity, m_iHitbox, hit);
		//logging::Info("B");
		if (this->v_bPrediction->GetBool()) SimpleLatencyPrediction(entity, m_iHitbox);
		//logging::Info("C");
	} else if (IsBuilding(entity)) {
		hit = GetBuildingPosition(entity);
	}
	if (v_bProjectileAimbot->GetBool()) {
		if (m_bProjectileMode) {
			if (v_fOverrideProjSpeed->GetBool())
				m_flProjSpeed = v_fOverrideProjSpeed->GetFloat();
			hit = ProjectilePrediction(entity, m_iHitbox, m_flProjSpeed, m_flProjGravity);
		}
	}
	//logging::Info("ayyming!");
	IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
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
					int rifleHandle = GetEntityValue<int>(local, netvar.hActiveWeapon);
					IClientEntity* rifle = interfaces::entityList->GetClientEntity(rifleHandle & 0xFFF);
					float bdmg = GetEntityValue<float>(rifle, netvar.flChargedDamage);
					if (bdmg < this->v_iAutoShootCharge->GetFloat()) return true;
				} else {
					if (!CanHeadshot(g_pLocalPlayer->entity)) return true;
				}
			}
		}
		if (g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == ClassID::CTFCompoundBow) {
			float begincharge = GetEntityValue<float>(g_pLocalPlayer->weapon, netvar.flChargeBeginTime);
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
