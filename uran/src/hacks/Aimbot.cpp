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
Aimbot::Aimbot() {
	target_systems[0] = new TargetSystemSmart();
	target_systems[1] = new TargetSystemFOV();
	target_systems[2] = new TargetSystemDistance();
	m_bAimKeySwitch = false;
	this->v_iAimKeyMode = CreateConVar("u_aimbot_aimkey_mode", "1", "AimKey mode [DISABLED/PTE/PTD/TOGGLE]");
	this->v_bEnabled = CreateConVar("u_aimbot_enabled", "0", "Enables aimbot. EXPERIMENTAL AND TOTALLY NOT LEGIT");
	this->v_iHitbox = CreateConVar("u_aimbot_hitbox", "0", "Hitbox");
	this->v_bAutoHitbox = CreateConVar("u_aimbot_autohitbox", "1", "Autohitbox");
	this->v_bPrediction = CreateConVar("u_aimbot_prediction", "1", "Latency prediction");
	this->v_bAutoShoot = CreateConVar("u_aimbot_autoshoot", "1", "Autoshoot");
	this->v_bSilent = CreateConVar("u_aimbot_silent", "1", "Silent mode");
	this->v_bZoomedOnly = CreateConVar("u_aimbot_zoomed", "1", "Only acitve with zoomed rifle");
	this->v_iAutoShootCharge = CreateConVar("u_aimbot_autoshoot_charge", "0.0", "Minimal charge for autoshoot");
	this->v_iMinRange = CreateConVar("u_aimbot_minrange", "0", "Minimum range to aim");
	this->v_bRespectCloak = CreateConVar("u_aimbot_respect_cloak", "1", "Will not shoot cloaked spies.");
	this->v_bCharge = CreateConVar("u_aimbot_charge", "0", "Autoshoot only with charge ready");
	this->v_bEnabledAttacking = CreateConVar("u_aimbot_enable_attack_only", "0", "Aimbot only active with attack key held");
	this->v_bStrictAttack = CreateConVar("u_aimbot_strict_attack", "0", "Not attacking unless target is locked");
	this->v_bProjectileAimbot = CreateConVar("u_aimbot_projectile", "1", "Projectile aimbot (EXPERIMENTAL)");
	this->v_iOverrideProjSpeed = CreateConVar("u_aimbot_proj_speed", "0", "Override proj speed");
	this->v_bDebug = CreateConVar("u_aimbot_debug", "0", "Aimbot debug");
	this->v_fFOV = CreateConVar("u_aimbot_fov", "0", "Aimbot fov");
	this->v_bMachinaPenetration = CreateConVar("u_aimbot_machina", "0", "Machina penetration aimbot (just for fun)");
	this->v_bSmooth = CreateConVar("u_aimbot_smooth", "0", "Smooth aimbot");
	this->v_flAutoShootHuntsmanCharge = CreateConVar("u_aimbot_huntsman_charge", "0.5", "Huntsman autoshoot charge");
	this->v_fSmoothValue = CreateConVar("u_aimbot_smooth_value", "0.2", "Smooth value");
	this->v_iAimKey = CreateConVar("u_aimbot_aimkey", "0", "Aim Key");
	this->v_iPriorityMode = CreateConVar("u_aimbot_prioritymode", "0", "Priority mode [SMART/FOV/DISTANCE/HEALTH]");
	v_bAimBuildings = CreateConVar("u_aimbot_buildings", "1", "Aim at buildings");
	v_bActiveOnlyWhenCanShoot = CreateConVar("u_aimbot_only_when_can_shoot", "1", "Aimbot active only when can shoot");
	v_fSmoothAutoshootTreshold = CreateConVar("u_aimbot_smooth_autoshoot_treshold", "0.01", "Smooth aim autoshoot treshold");
	this->v_fSmoothRandomness = CreateConVar("u_aimbot_smooth_randomness", "1.0", "Smooth randomness");
	fix_silent = false;
}

bool Aimbot::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	this->m_iLastTarget = -1;
	if (this->v_iAimKey->GetBool() && this->v_iAimKeyMode->GetBool()) {
		bool key_down = interfaces::input->IsButtonDown((ButtonCode_t)this->v_iAimKey->GetInt());
		switch (this->v_iAimKeyMode->GetInt()) {
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

	switch (GetWeaponMode(g_pLocalPlayer->entity)) {
	case weapon_medigun:
	case weapon_pda:
	case weapon_consumable:
		return true;
	};

	if (g_pLocalPlayer->cond_0 & cond::cloaked) return true; // TODO other kinds of cloak
	// TODO m_bFeignDeathReady no aim

	if (this->v_bActiveOnlyWhenCanShoot->GetBool() && !BulletTime()) return true;

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
	}

	if (IsAmbassador(g_pLocalPlayer->weapon)) {
		if ((interfaces::gvars->curtime - GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flLastFireTime)) <= 1.0) {
			return true;
		}
	}

	if(cmd->buttons & IN_USE) return true;

	if (this->v_bStrictAttack->GetBool() ) {
		cmd->buttons = cmd->buttons &~ IN_ATTACK;
	}
	IClientEntity* player = g_pLocalPlayer->entity;
	if (!player) return true;
	if (player->IsDormant()) return true;
	m_iHitbox = this->v_iHitbox->GetInt();
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
			switch (this->v_iPriorityMode->GetInt()) {
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
			case 2: {
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
			} break;
			case 3: {
				float scr;
				if (IsBuilding(ent)) {
					scr = 450.0f - GetEntityValue<int>(ent, eoffsets.iBuildingHealth);
				} else {
					scr = 450.0f - GetEntityValue<int>(ent, eoffsets.iHealth);
				}
				if (scr > target_highest_score) {
					target_highest_score = scr;
					target_highest = ent;
				}
			}
			}

		}
	}
	if (target_highest != 0) {
		this->m_iLastTarget = target_highest->entindex();
		Aim(target_highest, cmd);
	}
	return !this->v_bSilent->GetBool();
}

void Aimbot::PaintTraverse(void*, unsigned int, bool, bool) {
	if (!v_bEnabled->GetBool()) return;
	if (this->m_iLastTarget == -1) return;
	IClientEntity* ent = interfaces::entityList->GetClientEntity(this->m_iLastTarget);
	if (!ent) return;
	if (IsPlayer(ent)) {
		int clazz = GetEntityValue<int>(ent, eoffsets.iClass);
		if (clazz < 0 || clazz > 9) return;
		player_info_t info;
		if (!interfaces::engineClient->GetPlayerInfo(this->m_iLastTarget, &info)) return;
		AddCenterString(colors::yellow, colors::black, "Prey: %i HP %s (%s)", GetEntityValue<int>(ent, eoffsets.iHealth), tfclasses[clazz], info.name);
	} else if (IsBuilding(ent)) {
		AddCenterString(colors::yellow, colors::black, "Prey: %i HP LV %i %s", GetEntityValue<int>(ent, eoffsets.iBuildingHealth), GetEntityValue<int>(ent, eoffsets.iUpgradeLevel), GetBuildingType(ent));
	}
}

bool Aimbot::ShouldTarget(IClientEntity* entity) {
	//logging::Info("Should target?");
	if (!entity) return false;
	if (entity->IsDormant()) return false;
	if (IsPlayer(entity)) {
		if (IsPlayerInvulnerable(entity)) return false;
		int team = GetEntityValue<int>(entity, eoffsets.iTeamNum);
		int local = interfaces::engineClient->GetLocalPlayer();
		IClientEntity* player = interfaces::entityList->GetClientEntity(local);
		char life_state = GetEntityValue<char>(entity, eoffsets.iLifeState);
		if (life_state) return false;
		if (!player) return false;
		if (v_bRespectCloak->GetBool() && (GetEntityValue<int>(entity, eoffsets.iCond) & cond::cloaked)) return false;
		int health = GetEntityValue<int>(entity, eoffsets.iHealth);
		/*if (this->v_bCharge->GetBool() && (GetEntityValue<int>(player, eoffsets.iClass) == 2)) {
			int rifleHandle = GetEntityValue<int>(player, eoffsets.hActiveWeapon);
			IClientEntity* rifle = interfaces::entityList->GetClientEntity(rifleHandle & 0xFFF);
			if (!rifle) return false;
			float bdmg = GetEntityValue<float>(rifle, eoffsets.flChargedDamage);
			if (health > 150 && (health > (150 + bdmg) || bdmg < 15.0f)) return false;
		}*/
		int team_my = GetEntityValue<int>(player, eoffsets.iTeamNum);
		if (team == team_my) return false;
		Vector enemy_pos = entity->GetAbsOrigin();
		Vector my_pos = player->GetAbsOrigin();
		if (v_iMinRange->GetInt() > 0) {
			if ((enemy_pos - my_pos).Length() > v_iMinRange->GetInt()) return false;
		}
		int econd = GetEntityValue<int>(entity, eoffsets.iCond1);
		if ((econd & cond_ex::vacc_bullet)) return false;
		if (GetRelation(entity) == relation::FRIEND) return false;
		Vector resultAim;
		if (m_bProjectileMode) {
			if (!IsVectorVisible(g_pLocalPlayer->v_Eye, ProjectilePrediction(entity, m_iHitbox, m_flProjSpeed, m_flProjGravity))) return false;
		} else {
			if (v_bMachinaPenetration->GetBool()) {
				if (GetHitboxPosition(entity, m_iHitbox, resultAim)) return false;
				if (!IsEntityVisiblePenetration(entity, v_iHitbox->GetInt())) return false;
			} else {
				if (GetHitboxPosition(entity, m_iHitbox, resultAim)) return false;
				if (!IsEntityVisible(entity, m_iHitbox)) return false;
			}
		}
		if (v_fFOV->GetFloat() > 0.0f && (GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, resultAim) > v_fFOV->GetFloat())) return false;
		return true;
	} else if (IsBuilding(entity)) {
		if (!v_bAimBuildings->GetBool()) return false;
		int team = GetEntityValue<int>(entity, eoffsets.iTeamNum);
		if (team == g_pLocalPlayer->team) return false;
		Vector enemy_pos = entity->GetAbsOrigin();
		if (v_iMinRange->GetInt() > 0) {
			if ((enemy_pos - g_pLocalPlayer->v_Origin).Length() > v_iMinRange->GetInt()) return false;
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
		SimpleLatencyPrediction(entity, m_iHitbox);
		//logging::Info("C");
	} else if (IsBuilding(entity)) {
		hit = GetBuildingPosition(entity);
	}
	if (v_bProjectileAimbot->GetBool()) {
		if (m_bProjectileMode) {
			if (v_iOverrideProjSpeed->GetBool())
				m_flProjSpeed = v_iOverrideProjSpeed->GetFloat();
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
			if (g_pLocalPlayer->cond_0 & cond::zoomed) {
				if (this->v_iAutoShootCharge->GetBool()) {
					int rifleHandle = GetEntityValue<int>(local, eoffsets.hActiveWeapon);
					IClientEntity* rifle = interfaces::entityList->GetClientEntity(rifleHandle & 0xFFF);
					float bdmg = GetEntityValue<float>(rifle, eoffsets.flChargedDamage);
					if (bdmg < this->v_iAutoShootCharge->GetFloat()) return true;
				} else {
					if (!CanHeadshot(g_pLocalPlayer->entity)) return true;
				}
			}
		}
		if (g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == ClassID::CTFCompoundBow) {
			float begincharge = GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flChargeBeginTime);
			float charge = 0;
			if (begincharge != 0) {
				charge = interfaces::gvars->curtime - begincharge;
				if (charge > 1.0f) charge = 1.0f;
			}
			if (charge >= v_flAutoShootHuntsmanCharge->GetFloat()) {
				cmd->buttons &= ~IN_ATTACK;
			}
		} else {
			cmd->buttons |= IN_ATTACK;
		}
	}
	return true;
}
