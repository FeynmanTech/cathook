/*
 * HAimbot.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: nullifiedcat
 */

#include "HAimbot.h"

#include "../usercmd.h"
#include "../helpers.h"
#include "../entity.h"
#include "../interfaces.h"
#include "../trace.h"
#include "../targethelper.h"
#include "../localplayer.h"

//typedef int CBaseEntity;

#include "../fixsdk.h"
#include <client_class.h>
#include <inetchannelinfo.h>
#include <icliententity.h>
#include <icliententitylist.h>
#include <cdll_int.h>
#include <gametrace.h>
#include <engine/IEngineTrace.h>
#include "../sdk/in_buttons.h"

Vector viewangles_old;
bool fix_silent;

int target_lock;

/* null-safe */
void HAimbot::Create() {
	this->v_bEnabled = CreateConVar("u_aimbot_enabled", "0", "Enables aimbot. EXPERIMENTAL AND TOTALLY NOT LEGIT");
	this->v_iHitbox = CreateConVar("u_aimbot_hitbox", "0", "Hitbox");
	this->v_bPrediction = CreateConVar("u_aimbot_prediction", "1", "Latency prediction");
	this->v_bAutoShoot = CreateConVar("u_aimbot_autoshoot", "1", "Autoshoot");
	this->v_bSilent = CreateConVar("u_aimbot_silent", "1", "'Silent' mode");
	this->v_bZoomedOnly = CreateConVar("u_aimbot_zoomed", "1", "Only acitve with zoomed rifle");
	this->v_iAutoShootCharge = CreateConVar("u_aimbot_autoshoot_charge", "15.0", "Minimal charge for autoshoot");
	this->v_iMinRange = CreateConVar("u_aimbot_minrange", "0", "Minimum range to aim");
	this->v_bPriority = CreateConVar("u_aimbot_priority", "1", "Use priority system");
	this->v_bRespectCloak = CreateConVar("u_aimbot_respect_cloak", "1", "Will not shoot cloaked spies.");
	this->v_bCharge = CreateConVar("u_aimbot_charge", "0", "Autoshoot only with charge ready");
	this->v_bEnabledAttacking = CreateConVar("u_aimbot_enable_attack_only", "0", "Aimbot only active with attack key held");
	this->v_bStrictAttack = CreateConVar("u_aimbot_strict_attack", "0", "Not attacking unless target is locked");
	this->v_bProjectileAimbot = CreateConVar("u_aimbot_projectile", "1", "Projectile aimbot (EXPERIMENTAL)");
	this->v_iOverrideProjSpeed = CreateConVar("u_aimbot_proj_speed", "0", "Override proj speed");
	this->v_bDebug = CreateConVar("u_aimbot_debug", "0", "Aimbot debug");
	this->v_iFOV = CreateConVar("u_aimbot_fov", "0", "FOV aimbot (experimental)");
	fix_silent = false;
}

bool HAimbot::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (this->v_bEnabledAttacking->GetBool() && !(cmd->buttons & IN_ATTACK)) {
		return true;
	}

	if (g_pLocalPlayer->bIsReloading) {
		return true;
	}

	if (this->v_bStrictAttack->GetBool() ) {
		cmd->buttons = cmd->buttons &~ IN_ATTACK;
	}
	IClientEntity* player = g_pLocalPlayer->entity;
	if (!player) return true;
	if (player->IsDormant()) return true;
	if (g_pLocalPlayer->clazz == 2 && this->v_bZoomedOnly->GetBool() &&
		!(g_pLocalPlayer->cond_0 & cond::zoomed)) {
		return true;
	}
	if (g_pLocalPlayer->weapon) {
		if (g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == 210) return true;
	} /* Grappling hook */

	//logging::Info("Creating move.. aimbot");
	m_bProjectileMode = (GetProjectileData(g_pLocalPlayer->weapon, m_flProjSpeed, m_bProjArc));
	if (!this->v_bPriority->GetBool()) {
		IClientEntity* target_locked = interfaces::entityList->GetClientEntity(target_lock);
		if (target_locked != 0) {
			if (ShouldTarget(target_locked)) {
				Aim(target_locked, cmd);
				return true;
			} else {
				target_lock = 0;
			}
		}
	}
	IClientEntity* target_highest = 0;
	int target_highest_score = 0;
	for (int i = 0; i < interfaces::entityList->GetHighestEntityIndex() && i < 64; i++) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
		if (ent == 0) continue;
		if (ent->GetClientClass()->m_ClassID != 241) continue; // TODO magic number: player
		if (ShouldTarget(ent)) {
			//if (v_bDebug->GetBool()) {

			//}
			if (!this->v_bPriority->GetBool()) {
				target_lock = i;
				if (Aim(ent, cmd)) {
					continue;
				}
			} else {
				int scr = GetScoreForEntity(ent);
				if (scr > target_highest_score) {
					target_highest_score = scr;
					target_highest = ent;
				}
			}
		}
	}
	if (this->v_bPriority->GetBool()) {
		if (target_highest != 0) {
			Aim(target_highest, cmd);
		}
	}
	return !this->v_bSilent->GetBool();
}

void HAimbot::Destroy() {}
void HAimbot::PaintTraverse(void*, unsigned int, bool, bool) {}

bool HAimbot::ShouldTarget(IClientEntity* entity) {
	if (!entity) return false;
	if (entity->IsDormant()) return false;
	if (IsPlayerInvulnerable(entity)) return false;
	int team = GetEntityValue<int>(entity, eoffsets.iTeamNum);
	int local = interfaces::engineClient->GetLocalPlayer();
	IClientEntity* player = interfaces::entityList->GetClientEntity(local);
	char life_state = GetEntityValue<char>(entity, eoffsets.iLifeState);
	if (life_state) return false; // TODO magic number: life state
	if (!player) return false;
	if (v_bRespectCloak->GetBool() && (GetEntityValue<int>(entity, eoffsets.iCond) & cond::cloaked)) return false;
	int health = GetEntityValue<int>(entity, eoffsets.iHealth);
	if (this->v_bCharge->GetBool() && (GetEntityValue<int>(player, eoffsets.iClass) == 2)) {
		int rifleHandle = GetEntityValue<int>(player, eoffsets.hActiveWeapon);
		IClientEntity* rifle = interfaces::entityList->GetClientEntity(rifleHandle & 0xFFF);
		if (!rifle) return false;
		float bdmg = GetEntityValue<float>(rifle, eoffsets.flChargedDamage);
		if (health > 150 && (health > (150 + bdmg) || bdmg < 15.0f)) return false;
	}
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
	if (!m_bProjectileMode) {
		Vector hbv;
		if (GetHitboxPosition(entity, v_iHitbox->GetInt(), hbv)) return false;
		if (v_iFOV->GetBool() && (GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, hbv) > v_iFOV->GetInt())) return false;
		return IsEntityVisible(entity, v_iHitbox->GetInt());
	} else {
		Vector res = entity->GetAbsOrigin();
		bool succ = PredictProjectileAim(g_pLocalPlayer->v_Eye, entity, (hitbox)v_iHitbox->GetInt(), m_flProjSpeed, m_bProjArc, res);
		if (v_iFOV->GetBool() && (GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, res) > v_iFOV->GetInt())) return false;
		return succ;
	}
}

void PredictPosition(Vector vec, IClientEntity* ent) {
	if (!ent) return;
	Vector vel = GetEntityValue<Vector>(ent, eoffsets.vVelocity);
	float latency = interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) +
			interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	vec += vel * latency;
}

bool HAimbot::Aim(IClientEntity* entity, CUserCmd* cmd) {
	Vector hit;
	Vector angles;
	if (!entity) return false;
	GetHitboxPosition(entity, v_iHitbox->GetInt(), hit);
	//logging::Info("Predicting.. entity: 0x%08f", entity);
	PredictPosition(hit, entity);
	if (v_bProjectileAimbot->GetBool()) {
		float speed = 0.0f;
		bool arc = false;
		if (GetProjectileData(g_pLocalPlayer->weapon, speed, arc)) {
			if (v_iOverrideProjSpeed->GetBool())
				speed = v_iOverrideProjSpeed->GetFloat();
			PredictProjectileAim(g_pLocalPlayer->v_Eye, entity, (hitbox)v_iHitbox->GetInt(), speed, arc, hit);
		}
	}
	//logging::Info("Hit: %f %f %f", hit.x, hit.y, hit.z);
	IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
	Vector tr = (hit - g_pLocalPlayer->v_Eye);
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
	if (this->v_bSilent->GetBool()) {
		//FixMovement(*cmd, viewangles_old);
		Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
		Vector ang;
		VectorAngles(vsilent, ang);
		float yaw = deg2rad(ang.y - g_pLocalPlayer->v_OrigViewangles.y + cmd->viewangles.y);
		cmd->forwardmove = cos(yaw) * speed;
		cmd->sidemove = sin(yaw) * speed;
	}
	//logging::Info("Angles: %f %f %f", angles.x, angles.y, angles.z);
	if (this->v_bAutoShoot->GetBool()) {
		if (this->v_iAutoShootCharge->GetBool() && (GetEntityValue<int>(local, eoffsets.iClass) == 2)) {
			int rifleHandle = GetEntityValue<int>(local, eoffsets.hActiveWeapon);
			IClientEntity* rifle = interfaces::entityList->GetClientEntity(rifleHandle & 0xFFF);
			float bdmg = GetEntityValue<float>(rifle, eoffsets.flChargedDamage);
			if (bdmg < this->v_iAutoShootCharge->GetFloat()) return true;
		}
		cmd->buttons |= IN_ATTACK;
	}
	return true;
}
