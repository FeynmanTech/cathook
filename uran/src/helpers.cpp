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

#include "fixsdk.h"
#include <tier1/convar.h>
#include <engine/ivmodelinfo.h>
#include <icliententity.h>
#include <cmodel.h>
#include <studio.h>

bool IsPlayerInvulnerable(IClientEntity* player) {
	int cond1 = GetEntityValue<int>(player, entityvars.iCond);
	int cond2 = GetEntityValue<int>(player, entityvars.iCond1);
	int uber_mask_1 = (cond::uber | cond::uber_expiration | cond::bonk);
	int uber_mask_2 = (cond_ex::hidden_uber | cond_ex::canteen_uber | cond_ex::misc_uber | cond_ex::phlog_uber);
	if ((cond1 & uber_mask_1) || (cond2 & uber_mask_2)) return true;
	return false;
}

bool IsPlayerCritBoosted(IClientEntity* player) {
	int cond1 = GetEntityValue<int>(player, entityvars.iCond);
	int cond2 = GetEntityValue<int>(player, entityvars.iCond1);
	int cond4 = GetEntityValue<int>(player, entityvars.iCond3);
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
	uint32_t cond2 = GetEntityValue<uint32_t>(player, entityvars.iCond2);
	uint32_t cond3 = GetEntityValue<uint32_t>(player, entityvars.iCond3);
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
	if (entity->IsDormant()) return 1;
	const model_t* model = entity->GetModel();
	studiohdr_t* shdr = interfaces::model->GetStudiomodel(model);
	if (!shdr) return 2;
	// TODO rewrite

	mstudiohitboxset_t* set = shdr->pHitboxSet(GetEntityValue<int>(entity, entityvars.iHitboxSet));
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
	"SUPERNOVA"
};

const char* packs[] = {
	"+",
	"++",
	"+++"
};
