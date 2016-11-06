/*
 * helpers.cpp
 *
 *  Created on: Oct 8, 2016
 *      Author: nullifiedcat
 */

#include "helpers.h"
#include "interfaces.h"
#include "entity.h"
#include "logging.h"
#include "usercmd.h"
#include "trace.h"
#include "localplayer.h"

#include "fixsdk.h"
#include <tier1/convar.h>
#include <engine/ivmodelinfo.h>
#include <icliententity.h>
#include <cmodel.h>
#include <studio.h>
#include <gametrace.h>
#include <icliententitylist.h>
#include <cdll_int.h>
#include <engine/IEngineTrace.h>
#include <client_class.h>
#include <inetchannelinfo.h>

bool IsPlayerInvulnerable(IClientEntity* player) {
	int cond1 = GetEntityValue<int>(player, eoffsets.iCond);
	int cond2 = GetEntityValue<int>(player, eoffsets.iCond1);
	int uber_mask_1 = (cond::uber | cond::uber_expiration | cond::bonk);
	int uber_mask_2 = (cond_ex::hidden_uber | cond_ex::canteen_uber | cond_ex::misc_uber | cond_ex::phlog_uber);
	if ((cond1 & uber_mask_1) || (cond2 & uber_mask_2)) return true;
	return false;
}

bool IsPlayerCritBoosted(IClientEntity* player) {
	int cond1 = GetEntityValue<int>(player, eoffsets.iCond);
	int cond2 = GetEntityValue<int>(player, eoffsets.iCond1);
	int cond4 = GetEntityValue<int>(player, eoffsets.iCond3);
	int crit_mask_1 = (cond::kritzkrieg);
	int crit_mask_2 = (cond_ex::halloween_crit | cond_ex::canteen_crit | cond_ex::first_blood_crit | cond_ex::winning_crit |
			cond_ex::intelligence_crit | cond_ex::on_kill_crit | cond_ex::phlog_crit | cond_ex::misc_crit);
	int crit_mask_4 = (cond_ex3::powerup_crit);
	if ((cond1 & crit_mask_1) || (cond2 & crit_mask_2) || (cond4 & crit_mask_4)) return true;
	return false;
}

ConVar* CreateConVar(const char* name, const char* value, const char* help) {
	ConVar* ret = new ConVar(name, value, 0, help);
	interfaces::cvar->RegisterConCommand(ret);
	return ret;
}

ConCommand* CreateConCommand(const char* name, FnCommandCallback_t callback, const char* help) {
	ConCommand* ret = new ConCommand(name, callback, help);
	interfaces::cvar->RegisterConCommand(ret);
	return ret;
}

const char* GetModelPath(IClientEntity* entity) {
	if (!entity) return "NULL";
	const model_t* model = entity->GetModel();
	return interfaces::model->GetModelName(model);
}

/* Takes CBaseAnimating entity as input */
item_type GetItemType(IClientEntity* entity) {
	if (entity == 0) return item_type::item_null;
	const char* path = GetModelPath(entity); /* SDK function */
	size_t length = strlen(path);
	/* Default/Festive medkits */
	if (length >= 29 && path[16] == 'k') {
		if (path[20] == 's') return item_type::item_medkit_small;
		if (path[20] == 'm') return item_type::item_medkit_medium;
		if (path[20] == 'l') return item_type::item_medkit_large;
	}
	/* Sandwich/Steak */
	if (length >= 22 && path[13] == 'p' && path[14] == 'l') {
		return item_type::item_medkit_medium;
	}
	/* Medieval meat */
	if (length == 39 && path[31] == 'm' && path[29] == 'l') {
		return item_type::item_medkit_medium;
	}
	/* Halloween medkits */
	if (length >= 49 && path[33] == 'm' && path[36] == 'k') {
		if (path[20] == 's') return item_type::item_medkit_small;
		if (path[40] == 'm') return item_type::item_medkit_medium;
		if (path[40] == 'l') return item_type::item_medkit_large;
	}
	/* Ammo packs */
	if (length >= 31 && path[14] == 'm' && path[15] == 'm') {
		if (path[22] == 's') return item_type::item_ammo_small;
		if (path[22] == 'm') return item_type::item_ammo_medium;
		if (path[22] == 'l') return item_type::item_ammo_large;
	}
	/* Powerups */
	if (length >= 38 && path[20] == 'p' && path[24] == 'w') {
		if (path[30] == 'h') return item_type::item_mp_haste;
		if (path[30] == 'v') return item_type::item_mp_vampire;
		if (path[30] == 'u') return item_type::item_mp_uber;
		if (path[32] == 'e') return item_type::item_mp_precision;
		if (path[30] == 'w') return item_type::item_mp_warlock;
		if (path[32] == 'r') return item_type::item_mp_strength;
		if (path[32] == 'g') return item_type::item_mp_regeneration;
		if (path[37] == 'v') return item_type::item_mp_supernova;
		/* looks like resistance.mdl is unused and replaced with defense.mdl? idk */
		if (path[37] == 'n') return item_type::item_mp_resistance;
		if (path[34] == 'k') return item_type::item_mp_knockout;
		/* actually this one is 'defense' but it's used for resistance powerup */
		if (path[35] == 's') return item_type::item_mp_resistance;
		if (path[30] == 'c') return item_type::item_mp_crit;
		if (path[30] == 'a') return item_type::item_mp_agility;
		if (path[31] == 'i') return item_type::item_mp_king;
		if (path[33] == 'g') return item_type::item_mp_plague;
		if (path[36] == 't') return item_type::item_mp_reflect;
		if (path[30] == 't') return item_type::item_mp_thorns;
	}
	return item_type::item_null;
}

pack_type GetHealthPackType(IClientEntity* ent) {
	if (!ent) return pack_type::not_pack;
	const char* name = GetModelPath(ent);
	// models/items/medkit_SIZE.mdl
	if (strlen(name) >= 20 && name[13] == 'm' && name[16] == 'k') {
		if (name[20] == 's') return pack_type::small;
		if (name[20] == 'm') return pack_type::medium;
		if (name[20] == 'l') return pack_type::large;
	}
	// models/props_halloween/halloween_medkit_SIZE.mdl
	if (strlen(name) >= 42 && name[33] == 'm' && name[36] == 'k') {
		if (name[40] == 's') return pack_type::small;
		if (name[40] == 'm') return pack_type::medium;
		if (name[40] == 'l') return pack_type::large;
	}
	// models/items/plate
	if (strlen(name) >= 18 && name[13] == 'p' && name[17] == 'e') {
		return pack_type::medium;
	}
	return pack_type::not_pack;
	// TODO add halloween packs n' stuff
}

pack_type GetAmmoPackType(IClientEntity* ent) {
	if (!ent) return pack_type::not_pack;
	const char* name = GetModelPath(ent);
	if (strlen(name) < 30) return pack_type::not_pack;
	return pack_type::not_pack;
}

/* Strength [30]=='s' */

powerup_type GetPowerupType(IClientEntity* ent) {
	if (!ent) return powerup_type::not_powerup;
	const char* name = GetModelPath(ent);
	if (strlen(name) < 35) return powerup_type::not_powerup;
	if (name[27] != 'u' || name[22] != 'p') return powerup_type::not_powerup;

	if (name[30] == 's' && name[31] == 't') return powerup_type::strength;
	if (name[30] == 'd' && name[32] == 'f') return powerup_type::resistance;
	if (name[30] == 'v') return powerup_type::vampire;
	if (name[30] == 'r' && name[32] == 'f') return powerup_type::reflect;
	if (name[30] == 'h') return powerup_type::haste;
	if (name[30] == 'r' && name[32] == 'g') return powerup_type::regeneration;
	if (name[30] == 'p' && name[31] == 'r') return powerup_type::precision;
	if (name[30] == 'a') return powerup_type::agility;
	if (name[30] == 'k' && name[31] == 'n') return powerup_type::knockout;
	if (name[30] == 'k' && name[31] == 'i') return powerup_type::king;
	if (name[30] == 'p' && name[31] == 'l') return powerup_type::plague;
	if (name[30] == 's' && name[31] == 'u') return powerup_type::supernova;

	return powerup_type::not_powerup;
}

powerup_type GetPowerupOnPlayer(IClientEntity* player) {
	if (!player) return powerup_type::not_powerup;
	uint32_t cond2 = GetEntityValue<uint32_t>(player, eoffsets.iCond2);
	uint32_t cond3 = GetEntityValue<uint32_t>(player, eoffsets.iCond3);
	//if (!(cond2 & cond_ex2::powerup_generic)) return powerup_type::not_powerup;
	if (cond2 & cond_ex2::powerup_strength) return powerup_type::strength;
	if (cond2 & cond_ex2::powerup_haste) return powerup_type::haste;
	if (cond2 & cond_ex2::powerup_regen) return powerup_type::regeneration;
	if (cond2 & cond_ex2::powerup_resistance) return powerup_type::resistance;
	if (cond2 & cond_ex2::powerup_vampire) return powerup_type::vampire;
	if (cond2 & cond_ex2::powerup_reflect) return powerup_type::reflect;
	if (cond3 & cond_ex3::powerup_precision) return powerup_type::precision;
	if (cond3 & cond_ex3::powerup_agility) return powerup_type::agility;
	if (cond3 & cond_ex3::powerup_knockout) return powerup_type::knockout;
	if (cond3 & cond_ex3::powerup_king) return powerup_type::king;
	if (cond3 & cond_ex3::powerup_plague) return powerup_type::plague;
	if (cond3 & cond_ex3::powerup_supernova) return powerup_type::supernova;

	return powerup_type::not_powerup;
}

void VectorTransform (const float *in1, const matrix3x4_t& in2, float *out)
{
	out[0] = (in1[0] * in2[0][0] + in1[1] * in2[0][1] + in1[2] * in2[0][2]) + in2[0][3];
	out[1] = (in1[0] * in2[1][0] + in1[1] * in2[1][1] + in1[2] * in2[1][2]) + in2[1][3];
	out[2] = (in1[0] * in2[2][0] + in1[1] * in2[2][1] + in1[2] * in2[2][2]) + in2[2][3];
}

int GetHitboxPosition(IClientEntity* entity, int hb, Vector& out) {
	if (!entity) return 1;
	if (entity->IsDormant()) return 1;
	const model_t* model = entity->GetModel();
	studiohdr_t* shdr = interfaces::model->GetStudiomodel(model);
	if (!shdr) return 2;
	// TODO rewrite

	mstudiohitboxset_t* set = shdr->pHitboxSet(GetEntityValue<int>(entity, eoffsets.iHitboxSet));
	if (!set) return 4;
	mstudiobbox_t* box = set->pHitbox(hb);
	if (!box) return 5;
	matrix3x4_t bones[128];
	if (!entity->SetupBones(bones, 128, 0x100, 0)) return 3;
	Vector min, max;
	if (box->bone < 0 || box->bone >= 128) return 6;
	VectorTransform(box->bbmin, bones[box->bone], min);
	VectorTransform(box->bbmax, bones[box->bone], max);
	out = (min + max) / 2;

	return 0;
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

void FixMovement(CUserCmd& cmd, Vector& viewangles) {
	Vector movement(cmd.forwardmove, cmd.sidemove, cmd.upmove);
	float speed = sqrt(movement.x * movement.x + movement.y * movement.y);
	Vector ang;
	VectorAngles(movement, ang);
	float yaw = DEG2RAD(ang.y - viewangles.y + cmd.viewangles.y);
	cmd.forwardmove = cos(yaw) * speed;
	cmd.sidemove = sin(yaw) * speed;
	/*Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
	float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
	Vector ang;
	VectorAngles(vsilent, ang);
	float yaw = deg2rad(ang.y - viewangles_old.y + cmd->viewangles.y);
	cmd->forwardmove = cos(yaw) * speed;
	cmd->sidemove = sin(yaw) * speed;*/
}

float deg2rad(float deg) {
	return deg * (PI / 180);
}

trace::FilterDefault* trace_filter;
bool IsEntityVisible(IClientEntity* entity, int hb) {
	if (!trace_filter) {
		trace_filter = new trace::FilterDefault();
	}
	trace_t trace_visible;
	Ray_t ray;
	IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
	trace_filter->SetSelf(local);
	Vector hit;
	int ret = GetHitboxPosition(entity, hb, hit);
	if (ret) {
		logging::Info("Couldn't get hitbox position: %i", hb);
		return false;
	}
	ray.Init(local->GetAbsOrigin() + GetEntityValue<Vector>(local, eoffsets.vViewOffset), hit);
	interfaces::trace->TraceRay(ray, 0x4200400B, trace_filter, &trace_visible);
	if (trace_visible.m_pEnt) {
		return ((IClientEntity*)trace_visible.m_pEnt) == entity;
	}
	return false;
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

float DistToSqr(IClientEntity* entity) {
	if (entity == 0) return 0.0f;
	return g_pLocalPlayer->v_Origin.DistToSqr(entity->GetAbsOrigin());
}

bool GetProjectileData(IClientEntity* weapon, float& speed, bool& arc) {
	if (!weapon) return false;
	switch (weapon->GetClientClass()->m_ClassID) {
	case weapons::WP_DIRECT_HIT:
		speed = 1980;
		arc = false;
	break;
	case weapons::WP_ROCKET_LAUNCHER:
		speed = 1100;
		arc = false;
	break;
	case weapons::WP_GRENADE_LAUNCHER:
		speed = 1217.5f;
		arc = true;
	break;
	case weapons::WP_HUNTSMAN:
		speed = 2600.0f;
		arc = true;
	break;
	case weapons::WP_SANDMAN:
		speed = 3000.0f;
		arc = true;
	break;
	case weapons::WP_FLAREGUN:
		speed = 2000.0f;
		arc = true;
	break;
	default:
		return false;
	}
	return true;
}

bool PredictProjectileAim(Vector origin, IClientEntity* target, hitbox hb, float speed, bool arc, Vector& result) {
	if (!target) return false;
	Vector res1 = result;
	if (GetHitboxPosition(target, hb, result)) {
		res1 = target->GetAbsOrigin();
	}
	int flags = GetEntityValue<int>(target, eoffsets.iFlags);
	bool ground = (flags & (1 << 0));
	float distance = origin.DistTo(res1);
	float latency = interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) +
		interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	if (speed == 0) return false;
	float time = distance / speed + latency;
	if (!ground) {
		res1.z -= (400 * time * time);
	}
	res1 += GetEntityValue<Vector>(target, eoffsets.vVelocity) * time;
	if (arc)
		res1.z += (800 * 0.5 * 0.1 * time * time);
	result = res1;
	//if (!IsVisible())
	return true;
}

const char* powerups[] = {
	"STRENGTH",
	"RESISTANCE",
	"VAMPIRE",
	"REFLECT",
	"HASTE",
	"REGENERATION",
	"PRECISION",
	"AGILITY",
	"KNOCKOUT",
	"KING",
	"PLAGUE",
	"SUPERNOVA",
	"CRITS"
};

const char* packs[] = {
	"+",
	"++",
	"+++"
};
