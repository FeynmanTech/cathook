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

#define PI 3.14159265358979323846f

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

trace::FilterDefault* aim_filter;

void HAimbot::Create() {
	this->v_bEnabled = CreateConVar("u_aimbot_enabled", "0", "Enables aimbot. EXPERIMENTAL AND TOTALLY NOT LEGIT");
	this->v_iHitbox = CreateConVar("u_aimbot_hitbox", "0", "Hitbox");
	this->v_bPrediction = CreateConVar("u_aimbot_prediction", "1", "Latency prediction");
	this->v_bAutoShoot = CreateConVar("u_aimbot_autoshoot", "1", "Autoshoot");
	this->v_bSilent = CreateConVar("u_aimbot_silent", "1", "'Silent' mode");
	this->v_bZoomedOnly = CreateConVar("u_aimbot_zoomed", "1", "Only acitve with zoomed rifle");
	this->v_bAutoShootCharge = CreateConVar("u_aimbot_autoshoot_charge", "1", "Charge is needed for autoshoot");
	this->v_iMinRange = CreateConVar("u_aimbot_minrange", "0", "Minimum range to aim");
	this->v_bPriority = CreateConVar("u_aimbot_priority", "1", "Use priority system");
	this->v_bRespectCloak = CreateConVar("u_aimbot_respect_cloak", "1", "Will not shoot cloaked spies.");
	aim_filter = new trace::FilterDefault();
	fix_silent = false;
}

bool HAimbot::CreateMove(void*, float, CUserCmd* cmd) {
	int local = interfaces::engineClient->GetLocalPlayer();
	IClientEntity* player = interfaces::entityList->GetClientEntity(local);
	if (!player) return true;
	if (player->IsDormant()) return true;
	if ((GetEntityValue<int>(player, entityvars.iClass) == 2) && this->v_bZoomedOnly->GetBool() &&
		!(GetEntityValue<int>(player, entityvars.iCond) & cond::zoomed)) {
		return true;
	}
	if (g_pLocalPlayer->weapon) {
		if (g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == 210) return true;
	} /* Grappling hook */
	viewangles_old = cmd->viewangles;
	if (!this->v_bEnabled->GetBool()) return true;
	//logging::Info("Creating move.. aimbot");
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

bool HAimbot::IsVisible(IClientEntity* entity) {
	//logging::Info("visible??");
	trace_t aim_trace;
	Ray_t ray;
	IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
	aim_filter->SetSelf(local);
	Vector hit;
	int ret = GetHitboxPosition(entity, v_iHitbox->GetInt(), hit);
	if (ret) {
		//logging::Info("ERROR %i", ret);
		return false;
	}
	ray.Init(local->GetAbsOrigin() + GetEntityValue<Vector>(local, entityvars.vViewOffset), hit);
	interfaces::trace->TraceRay(ray, 0x4200400B, aim_filter, &aim_trace);
	if (aim_trace.m_pEnt) {
		//return (aim_trace.m_pEnt == (CBaseEntity*)(entity));
		//return (aim_trace.m_pEnt == entity);
		return ((IClientEntity*)aim_trace.m_pEnt) == entity;
	}
	//logging::Info("nO");
	return false;
}

bool HAimbot::ShouldTarget(IClientEntity* entity) {
	if (!entity) return false;
	if (entity->IsDormant()) return false;
	if (IsPlayerInvulnerable(entity)) return false;
	int team = GetEntityValue<int>(entity, entityvars.iTeamNum);
	int local = interfaces::engineClient->GetLocalPlayer();
	IClientEntity* player = interfaces::entityList->GetClientEntity(local);
	char life_state = GetEntityValue<char>(entity, entityvars.iLifeState);
	if (life_state) return false; // TODO magic number: life state
	if (!player) return false;
	int team_my = GetEntityValue<int>(player, entityvars.iTeamNum);
	if (team == team_my) return false;
	Vector enemy_pos = entity->GetAbsOrigin();
	Vector my_pos = player->GetAbsOrigin();
	if (v_iMinRange->GetInt() > 0) {
		if ((enemy_pos - my_pos).Length() > v_iMinRange->GetInt()) return false;
	}
	int econd = GetEntityValue<int>(entity, entityvars.iCond1);
	if ((econd & cond_ex::vacc_bullet)) return false;
	return this->IsVisible(entity);
}

void fVectorAngles(Vector &forward, Vector &angles) {
	float tmp, yaw, pitch;

	if(forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if(forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / PI);
		if(yaw < 0)
			yaw += 360;

		tmp = sqrt((forward[0] * forward[0] + forward[1] * forward[1]));
		pitch = (atan2(-forward[2], tmp) * 180 / PI);
		if(pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void fClampAngle(Vector& qaAng) {
	while(qaAng[0] > 89)
		qaAng[0] -= 180;

	while(qaAng[0] < -89)
		qaAng[0] += 180;

	while(qaAng[1] > 180)
		qaAng[1] -= 360;

	while(qaAng[1] < -180)
		qaAng[1] += 360;

	qaAng.z = 0;
}

void PredictPosition(Vector vec, IClientEntity* ent) {
	Vector vel = GetEntityValue<Vector>(ent, entityvars.vVelocity);
	float latency = interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) +
			interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	vec += vel * latency;
}

void VectorAngles(Vector &forward, Vector &angles) {
	float tmp, yaw, pitch;

	if(forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if(forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / PI);
		if(yaw < 0)
			yaw += 360;

		tmp = sqrt((forward[0] * forward[0] + forward[1] * forward[1]));
		pitch = (atan2(-forward[2], tmp) * 180 / PI);
		if(pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

float deg2rad(float deg) {
	return deg * (PI / 180);
}

bool HAimbot::Aim(IClientEntity* entity, CUserCmd* cmd) {
	Vector hit;
	Vector angles;
	GetHitboxPosition(entity, v_iHitbox->GetInt(), hit);
	if (v_bPrediction->GetBool()) {
		PredictPosition(hit, entity);
	}
	IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
	Vector tr = (hit - (local->GetAbsOrigin() + GetEntityValue<Vector>(local, entityvars.vViewOffset)));
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
	if (this->v_bAutoShoot->GetBool()) {
		if (this->v_bAutoShootCharge->GetBool() && (GetEntityValue<int>(local, entityvars.iClass) == 2)) {
			int rifleHandle = GetEntityValue<int>(local, entityvars.hActiveWeapon);
			IClientEntity* rifle = interfaces::entityList->GetClientEntity(rifleHandle & 0xFFF);
			float bdmg = GetEntityValue<float>(rifle, entityvars.flChargedDamage);
			if (bdmg < 15.0f) return true;
		}
		cmd->buttons |= IN_ATTACK;
	}
	if (this->v_bSilent->GetBool()) {
		Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
		Vector ang;
		VectorAngles(vsilent, ang);
		float yaw = deg2rad(ang.y - viewangles_old.y + cmd->viewangles.y);
		cmd->forwardmove = cos(yaw) * speed;
		cmd->sidemove = sin(yaw) * speed;
	}
	if (!this->v_bSilent->GetBool()) {
		QAngle a;
		a.x = angles.x;
		a.y = angles.y;
		a.z = angles.z;
		interfaces::engineClient->SetViewAngles(a);
	}
	return true;
}
