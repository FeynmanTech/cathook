/*
 * helpers.cpp
 *
 *  Created on: Oct 8, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "hooks.h"
#include "sdk.h"

#include <pwd.h>

FILE* hConVarsFile = 0;
void BeginConVars() {
	passwd* pwd = getpwuid(getuid());
	char* user = pwd->pw_name;
	hConVarsFile = fopen(strfmt("/home/%s/.local/share/Steam/steamapps/common/Team Fortress 2/tf/cfg/uran.cfg", user), "w");
	SetCVarInterface(interfaces::cvar);
}

void EndConVars() {
	fclose(hConVarsFile);
	ConVar_Register();
}


bool IsPlayerInvulnerable(IClientEntity* player) {
	int cond1 = GetEntityValue<int>(player, eoffsets.iCond);
	int cond2 = GetEntityValue<int>(player, eoffsets.iCond1);
	int uber_mask_1 = (cond::uber | cond::bonk);
	int uber_mask_2 = (cond_ex::hidden_uber | cond_ex::canteen_uber | cond_ex::misc_uber | cond_ex::phlog_uber);
	if ((cond1 & uber_mask_1) || (cond2 & uber_mask_2)) {
		//logging::Info("COND1: %i MASK1: %i", cond1, uber_mask_1);
		//logging::Info("COND2: %i MASK2: %i", cond2, uber_mask_2);
		return true;
	}
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
	fprintf(hConVarsFile, "%s %s\n", name, value);
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

const char* GetBuildingType(IClientEntity* ent) {
	if (!ent) return "[NULL]";
	switch (ent->GetClientClass()->m_ClassID) {
	case ClassID::CObjectSentrygun:
		// TODO mini
		return "Sentry";
	case ClassID::CObjectDispenser:
		return "Dispenser";
	case ClassID::CObjectTeleporter:
		// TODO exit/entr
		return "Teleporter";
	}
	return "[NULL]";
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

bool IsPlayerInvisible(IClientEntity* player) {
	return false; // TODO stumpy.flv
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
		//logging::Info("Couldn't get hitbox position: %i", hb);
		return false;
	}
	ray.Init(local->GetAbsOrigin() + GetEntityValue<Vector>(local, eoffsets.vViewOffset), hit);
	interfaces::trace->TraceRay(ray, 0x4200400B, trace_filter, &trace_visible);
	if (trace_visible.m_pEnt) {
		return ((IClientEntity*)trace_visible.m_pEnt) == entity;
	}
	return false;
}

Vector GetBuildingPosition(IClientEntity* ent) {
	Vector res = ent->GetAbsOrigin();
	switch (ent->GetClientClass()->m_ClassID) {
	case ClassID::CObjectDispenser:
		res.z += 30;
		break;
	case ClassID::CObjectTeleporter:
		res.z += 8;
		break;
	case ClassID::CObjectSentrygun:
		switch (GetEntityValue<int>(ent, eoffsets.iUpgradeLevel)) {
		case 1:
			res.z += 30;
			break;
		case 2:
			res.z += 50;
			break;
		case 3:
			res.z += 60;
			break;
		}
		break;
	}
	return res;
}

bool IsBuildingVisible(IClientEntity* ent) {
	if (!trace_filter) {
		trace_filter = new trace::FilterDefault();
	}
	trace_t trace_visible;
	Ray_t ray;
	trace_filter->SetSelf(g_pLocalPlayer->entity);
	ray.Init(g_pLocalPlayer->v_Eye, GetBuildingPosition(ent));
	interfaces::trace->TraceRay(ray, 0x4200400B, trace_filter, &trace_visible);
	return (IClientEntity*)trace_visible.m_pEnt == ent;
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

weaponmode GetWeaponMode(IClientEntity* player) {
	if (!player) return weapon_invalid;
	int weapon_handle = GetEntityValue<int>(player, eoffsets.hActiveWeapon);
	if (weapon_handle == GetEntityValue<int>(player, eoffsets.hMyWeapons + sizeof(int) * 2)) return weaponmode::weapon_melee;
	IClientEntity* weapon = interfaces::entityList->GetClientEntity(weapon_handle & 0xFFF);
	if (!weapon) return weaponmode::weapon_invalid;
	switch (weapon->GetClientClass()->m_ClassID) {
	case ClassID::CTFLunchBox:
	case ClassID::CTFLunchBox_Drink:
	case ClassID::CTFBuffItem:
		return weaponmode::weapon_consumable;
	case ClassID::CTFRocketLauncher_DirectHit:
	case ClassID::CTFRocketLauncher:
	case ClassID::CTFGrenadeLauncher:
	case ClassID::CTFCompoundBow:
	case ClassID::CTFBat_Wood:
	case ClassID::CTFBat_Giftwrap:
	case ClassID::CTFFlareGun:
	case ClassID::CTFFlareGun_Revenge:
	case ClassID::CTFSyringeGun:
		return weaponmode::weapon_projectile;
	};
	if (weapon_handle == GetEntityValue<int>(player, eoffsets.hMyWeapons + sizeof(int) * 3)) return weaponmode::weapon_pda;
	if (GetEntityValue<int>(player, eoffsets.iClass) == tf_class::tf_medic) {
		if (weapon_handle == GetEntityValue<int>(player, eoffsets.hMyWeapons + sizeof(int) * 1)) return weaponmode::weapon_medigun;
	}
	return weaponmode::weapon_hitscan;
}

bool GetProjectileData(IClientEntity* weapon, float& speed, bool& arc, float& gravity) {
	if (!weapon) return false;
	float rspeed;
	bool rarc;
	float rgrav = 0.5f;
	switch (weapon->GetClientClass()->m_ClassID) {
	case ClassID::CTFRocketLauncher_DirectHit:
		rspeed = 1980.0f;
		rarc = false;
		//rgrav = 0.0f;
	break;
	case ClassID::CTFRocketLauncher:
		rspeed = 1100.0f;
		rarc = false;
		//rgrav = 0.0f;
	break;
	case ClassID::CTFGrenadeLauncher:
		rspeed = 1200.0f; // TODO Loch-N-Load: 1500u
		rarc = true;
		//rgrav = 0.5f;
	break;
	case ClassID::CTFCompoundBow: {
		// TODO VMT Stuff!
		float servertime = (float)GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.nTickBase) * interfaces::gvars->interval_per_tick;
		float curtime_old = interfaces::gvars->curtime;
		interfaces::gvars->curtime = servertime;
		typedef float(GetProjectileData)(IClientEntity*);
		rspeed = (reinterpret_cast<GetProjectileData*>(*(*(const void ***) weapon + 527)))(weapon);
		rgrav = ((GetProjectileData*) *(*(const void ***) weapon + 528))(weapon);
		//logging::Info("%f", ((GetProjectileData*)(hooks::GetVMT(weapon, 0)[457]))(weapon));
		interfaces::gvars->curtime = curtime_old;
		// TODO curtime
		/*float begincharge = GetEntityValue<float>(weapon, eoffsets.flChargeBeginTime);
		float charge = 0;
		if (begincharge != 0) {
			charge = servertime - begincharge;
			if (charge > 1.0f) charge = 1.0f;
		}*/
		//rgrav = ( ( ( 1.3 - charge ) / 3 ) * 1000 ) / 800; // F1Public's method
		/*if (charge <= 0.5f) {
			rgrav = 0.5 - 0.4 * charge;
		} else {
			rgrav = 0.4 - 0.3 * charge;
		}*/
		//rgrav = 0.5 - 0.4 * (charge);

		//rspeed = 1800 + 800 * charge;
		rarc = true;
	} break;
	case ClassID::CTFBat_Wood:
		//rgrav = 1.0f;
		rspeed = 3000.0f;
		rarc = true;
	break;
	case ClassID::CTFFlareGun:
		//rgrav = 1.0f;
		rspeed = 2000.0f;
		rarc = true;
	break;
	case ClassID::CTFSyringeGun:
		rgrav = 0.2f;
		rspeed = 990.0f;
		rarc = true;
	break;
	default:
		return false;
	}
	speed = rspeed;
	arc = rarc;
	gravity = rgrav;
	if (!arc) gravity = 0;
	return true;
}

const char* MakeInfoString(IClientEntity* player) {
	char* buf = new char[256]();
	player_info_t info;
	if (!interfaces::engineClient->GetPlayerInfo(player->entindex(), &info)) return (const char*)0;
	logging::Info("a");
	int hWeapon = GetEntityValue<int>(player, eoffsets.hActiveWeapon);
	if (GetEntityValue<char>(player, eoffsets.iLifeState)) {
		sprintf(buf, "%s is dead %s", info.name, tfclasses[GetEntityValue<int>(player, eoffsets.iClass)]);
		return buf;
	}
	if (hWeapon) {
		IClientEntity* weapon = interfaces::entityList->GetClientEntity(hWeapon & 0xFFF);
		sprintf(buf, "%s is %s with %i health using %s", info.name, tfclasses[GetEntityValue<int>(player, eoffsets.iClass)], GetEntityValue<int>(player, eoffsets.iHealth), weapon->GetClientClass()->GetName());
	} else {
		sprintf(buf, "%s is %s with %i health", info.name, tfclasses[GetEntityValue<int>(player, eoffsets.iClass)], GetEntityValue<int>(player, eoffsets.iHealth));
	}
	logging::Info("Result: %s", buf);
	return buf;
}

trace::FilterNoPlayer* vec_filter;
bool IsVectorVisible(Vector origin, Vector target) {
	//logging::Info("ISVV");
	if (!vec_filter) {
		vec_filter = new trace::FilterNoPlayer();
	}
	trace_t trace_visible;
	Ray_t ray;
	vec_filter->SetSelf(g_pLocalPlayer->entity);
	ray.Init(origin, target);
	interfaces::trace->TraceRay(ray, 0x4200400B, vec_filter, &trace_visible);
	float dist2 = origin.DistToSqr(trace_visible.endpos);
	float dist1 = origin.DistToSqr(target);
	//logging::Info("Target: %.1f, %.1f, %.1f; ENDPOS: %.1f, %.1f, %.1f", target.x, target.y, target.z, trace_visible.endpos.x, trace_visible.endpos.y, trace_visible.endpos.z);
	//logging::Info("Dist1: %f, Dist2: %f");
	return (dist1 <= dist2);
}

/*bool PredictProjectileAim(Vector origin, IClientEntity* target, hitbox_t hb, float speed, bool arc, float gravity, Vector& result) {
	if (!target) return false;
	//logging::Info("PRED PROJ AIM");
	//logging::Info("ProjSpeed: %f", speed);
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
		res1.z += (800 * 0.5 * gravity * time * time);
	result = res1;
	//if (!IsVisible();
	return IsVectorVisible(origin, res1);
}*/

relation GetRelation(IClientEntity* ent) {
	if (!ent) return relation::NEUTRAL;
	if (ent->IsDormant()) return relation::NEUTRAL;
	player_info_t info;
	if (!interfaces::engineClient->GetPlayerInfo(ent->entindex(), &info)) return relation::NEUTRAL;
	for (int i = 0; i < n_friends; i++) {
		if (friends[i] == info.friendsID) return relation::FRIEND;
	}
	for (int i = 0; i < n_rage; i++) {
		if (rage[i] == info.friendsID) return relation::RAGE;
	}
	return relation::NEUTRAL;
}

bool IsSentryBuster(IClientEntity* entity) {
	// TODO
	return (entity && entity->GetClientClass()->m_ClassID == ClassID::CTFPlayer && GetEntityValue<int>(entity, eoffsets.iClass) == tf_class::tf_demoman && g_pPlayerResource->GetMaxHealth(entity) == 2500);
}

bool IsAmbassador(IClientEntity* entity) {
	if (!entity) return false;
	if (entity->GetClientClass()->m_ClassID != ClassID::CTFRevolver) return false;
	int defidx = GetEntityValue<int>(entity, eoffsets.iItemDefinitionIndex);
	switch (defidx) {
	case 61:
	case 1006:
		return true;
	}
	return false;
}

bool CheckCE(CachedEntity* entity) {
	return (entity && entity->m_pEntity && !entity->m_pEntity->IsDormant());
}

// F1 c&p
Vector CalcAngle(Vector src, Vector dst) {
	Vector AimAngles;
	Vector delta = src - dst;
	float hyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y)); //SUPER SECRET IMPROVEMENT CODE NAME DONUT STEEL
	AimAngles.x = atanf(delta.z / hyp) * RADPI;
	AimAngles.y = atanf(delta.y / delta.x) * RADPI;
	AimAngles.z = 0.0f;
	if(delta.x >= 0.0)
		AimAngles.y += 180.0f;
	return AimAngles;
}

bool IsBuilding(IClientEntity* ent) {
	if (!ent) return false;
	switch (ent->GetClientClass()->m_ClassID) {
	case ClassID::CObjectSentrygun:
	case ClassID::CObjectDispenser:
	case ClassID::CObjectTeleporter:
		return true;
	}
	return false;
}

bool IsPlayer(IClientEntity* ent) {
	return (ent && ent->GetClientClass()->m_ClassID == ClassID::CTFPlayer);
}

void MakeVector(Vector angle, Vector& vector)
{
	float pitch = float(angle[0] * PI / 180);
	float yaw = float(angle[1] * PI / 180);
	float tmp = float(cos(pitch));
	vector[0] = float(-tmp * -cos(yaw));
	vector[1] = float(sin(yaw)*tmp);
	vector[2] = float(-sin(pitch));
}

float GetFov(Vector angle, Vector src, Vector dst)
{
	Vector ang, aim;
	ang = CalcAngle(src, dst);

	MakeVector(angle, aim);
	MakeVector(ang, ang);

	float mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
	float u_dot_v = aim.Dot(ang);

	return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
}

bool CanHeadshot(IClientEntity* player) {
	int weapon_handle = GetEntityValue<int>(player, eoffsets.hActiveWeapon);
	IClientEntity* weapon = interfaces::entityList->GetClientEntity(weapon_handle & 0xFFF);
	if (!weapon) return false;
	float charged_damage = GetEntityValue<float>(weapon, eoffsets.flChargedDamage);
	switch(weapon->GetClientClass()->m_ClassID) {
	case ClassID::CTFSniperRifle:
		return charged_damage >= 15.0f;
	case ClassID::CTFSniperRifleDecap: {
		int decaps = GetEntityValue<int>(player, eoffsets.iDecapitations);
		switch (decaps) { // TODO VALUES ARE NOT PRECISE
		// noobish
		case 0:
			return charged_damage >= 7.5f;
		case 1:
			return charged_damage >= 13.0f;
		case 2:
			return charged_damage >= 15.0f;
		case 3:
			return charged_damage >= 18.5f;
		case 4:
			return charged_damage >= 22.5f;
		case 5:
			return charged_damage >= 24.0f;
		default:
			return charged_damage >= 25.0f;
		}
		break;
	}
	case ClassID::CTFSniperRifleClassic:
		return false; // NO!!! IT CANNOT!!! Fuck classic.
	}
	return false;
}

bool BulletTime() {
	float tickbase = (float)(GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.nTickBase)) * interfaces::gvars->interval_per_tick;
	float nextattack = GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flNextPrimaryAttack);
	return nextattack <= tickbase;
}

// TODO casting
QAngle VectorToQAngle(Vector in) {
	return QAngle(in.x, in.y, in.z);
}

Vector QAngleToVector(QAngle in) {
	return Vector(in.x, in.y, in.z);
}

void AimAt(Vector origin, Vector target, CUserCmd* cmd) {
	Vector angles;
	Vector tr = (target - origin);
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
}

void AimAtHitbox(IClientEntity* ent, int hitbox, CUserCmd* cmd) {
	Vector r = ent->GetAbsOrigin();
	GetHitboxPosition(ent, hitbox, r);
	AimAt(g_pLocalPlayer->v_Eye, r, cmd);
	//logging::Info("Aiming at %f %f %f", r.x, r.y, r.z);
}

bool IsEntityVisiblePenetration(IClientEntity* entity, int hb) {
	if (!trace::g_pFilterPenetration) {
		trace::g_pFilterPenetration = new trace::FilterPenetration();
	}
	trace_t trace_visible;
	Ray_t ray;
	IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
	trace::g_pFilterPenetration->SetSelf(local);
	trace::g_pFilterPenetration->Reset();
	Vector hit;
	int ret = GetHitboxPosition(entity, hb, hit);
	if (ret) {
		return false;
	}
	ray.Init(local->GetAbsOrigin() + GetEntityValue<Vector>(local, eoffsets.vViewOffset), hit);
	interfaces::trace->TraceRay(ray, 0x4200400B, trace::g_pFilterPenetration, &trace_visible);
	bool s = false;
	if (trace_visible.m_pEnt) {
		s = ((IClientEntity*)trace_visible.m_pEnt) == entity;
	}
	if (!s) return false;
	interfaces::trace->TraceRay(ray, 0x4200400B, trace::g_pFilterDefault, &trace_visible);
	if (trace_visible.m_pEnt) {
		IClientEntity* ent = (IClientEntity*)trace_visible.m_pEnt;
		if (ent) {
			if (ent->GetClientClass()->m_ClassID == ClassID::CTFPlayer) {
				if (ent == entity) return false;
				if (trace_visible.hitbox >= 0) {
					return true;
				}
			}
		}
	}
	return false;
}


class CMoveData;

void RunEnginePrediction(IClientEntity* ent, CUserCmd *ucmd) {
	// we are going to require some helper functions for this to work
	// notably SetupMove, FinishMove and ProcessMovement


	// setup the types of the functions
	typedef void(*SetupMoveFn)(IClientEntity *, CUserCmd *, class IMoveHelper *, CMoveData *);
	typedef void(*FinishMoveFn)(IClientEntity *, CUserCmd*, CMoveData*);
	typedef void(*ProcessMovementFn)(IClientEntity *, CMoveData *);
	typedef void(*StartTrackPredictionErrorsFn)(IClientEntity *);
	typedef void(*FinishTrackPredictionErrorsFn)(IClientEntity *);

	// get the vtable
	void **predictionVtable = *(void ***)interfaces::prediction;
	logging::Info("predictionVtable 0x%08x", predictionVtable);
	// get the functions
	SetupMoveFn oSetupMove = (SetupMoveFn) predictionVtable[19];
	FinishMoveFn oFinishMove = (FinishMoveFn) predictionVtable[20];

	// get the vtable
	void **gameMovementVtable = *(void ***)interfaces::gamemovement;
	logging::Info("gameMovementVtable 0x%08x", gameMovementVtable);
	// get the functions
	ProcessMovementFn oProcessMovement = (ProcessMovementFn) gameMovementVtable[2];
	StartTrackPredictionErrorsFn oStartTrackPredictionErrors = (StartTrackPredictionErrorsFn) gameMovementVtable[3];
	FinishTrackPredictionErrorsFn oFinishTrackPredictionErrors = (FinishTrackPredictionErrorsFn) gameMovementVtable[4];

	// use this as movedata (should be big enough - otherwise the stack will die!)
	unsigned char moveData[2048];
	CMoveData *pMoveData = (CMoveData *)&(moveData[0]);
	logging::Info("pMoveData 0x%08x", pMoveData);

	// back up globals
	float frameTime = interfaces::gvars->frametime;
	float curTime = interfaces::gvars->curtime;

	CUserCmd defaultCmd;
	if(ucmd == NULL)
	{
		ucmd = &defaultCmd;
	}

	// set the current command
	SetEntityValue<void *>(ent, 0x105C, ucmd);

	// set up the globals
	interfaces::gvars->curtime =  interfaces::gvars->interval_per_tick * GetEntityValue<int>(ent, eoffsets.nTickBase);
	interfaces::gvars->frametime = interfaces::gvars->interval_per_tick;

	oStartTrackPredictionErrors(ent);

	logging::Info("StartTrackPredictionErrors(ent)");
	oSetupMove(ent, ucmd, NULL, pMoveData);
	logging::Info("oSetupMove");
	oProcessMovement(ent, pMoveData);
	logging::Info("oProcessMovement");
	oFinishMove(ent, ucmd, pMoveData);
	logging::Info("oFinishMove");

	oFinishTrackPredictionErrors(ent);
	logging::Info("oFinishTrackPredictionErrors");
	// reset the current command
	SetEntityValue<void *>(ent, 0x105C, NULL);

	// restore globals
	interfaces::gvars->frametime = frameTime;
	interfaces::gvars->curtime = curTime;

	return;
}

char* strfmt(const char* fmt, ...) {
	char* buf = new char[1024];
	va_list list;
	va_start(list, fmt);
	vsprintf(buf, fmt, list);
	va_end(list);
	return buf;
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

uint32 friends[256];
uint32 rage[256];

int n_friends = 0;
int n_rage = 0;

const char* tfclasses[] = {
	"[NULL]",
	"Scout",
	"Sniper",
	"Soldier",
	"Demoman",
	"Medic",
	"Heavy",
	"Pyro",
	"Spy",
	"Engineer"
};

const char* packs[] = {
	"+",
	"++",
	"+++"
};
