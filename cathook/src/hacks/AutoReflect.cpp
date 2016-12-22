/*
 * AutoReflect.cpp
 *
 *  Created on: Nov 18, 2016
 *      Author: nullifiedcat
 */

#include "AutoReflect.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(AutoReflect);

const char* AutoReflect::GetName() {
	return "AUTO-REFLECT";
}

bool AutoReflect::ShouldReflect(IClientEntity* ent) {
	if (!ent) return false;
	switch (ent->GetClientClass()->m_ClassID) {
	case ClassID::CTFProjectile_Arrow:
	case ClassID::CTFProjectile_Flare:
	case ClassID::CTFProjectile_HealingBolt:
	case ClassID::CTFProjectile_Rocket:
	case ClassID::CTFProjectile_SentryRocket:
	case ClassID::CTFProjectile_EnergyBall: {
		int deflected = GetVar<int>(ent, netvar.Rocket_iDeflected);
		if (deflected) return false; // TODO deflected by enemy player
		if (GetVar<int>(ent, netvar.iTeamNum) == g_pLocalPlayer->team) return false;
		return true;
	} break;
	case ClassID::CTFProjectile_Cleaver:
	case ClassID::CTFProjectile_Jar:
	case ClassID::CTFProjectile_JarMilk: {
		int deflected = GetVar<int>(ent, netvar.Grenade_iDeflected);
		if (deflected) return false;
		if (GetVar<int>(ent, netvar.iTeamNum) == g_pLocalPlayer->team) return false;
		return true;
	} break;
	case ClassID::CTFGrenadePipebombProjectile: {
		int deflected = GetVar<int>(ent, netvar.Grenade_iDeflected);
		if (deflected) return false;
		if (GetVar<int>(ent, netvar.iTeamNum) == g_pLocalPlayer->team) return false;
		if (GetVar<int>(ent, netvar.iPipeType) == 1) {
			if (!v_bReflectStickies->GetBool()) return false;
		}
		return true;
	} break;
	}
	return false;
}

// Hack Methods

AutoReflect::AutoReflect() {
	v_bEnabled = CREATE_CV(CV_SWITCH, "reflect_enabled", "0", "Enable");
	v_iReflectDistance = CREATE_CV(CV_INT, "reflect_distance", "200", "Distance");
	v_bDisableWhenAttacking = CREATE_CV(CV_SWITCH, "reflect_only_idle", "0", "Only when not shooting");
	v_bReflectStickies = CREATE_CV(CV_SWITCH, "reflect_stickybombs", "0", "Reflect stickies");
}
// TODO
bool AutoReflect::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;
	if (g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID != ClassID::CTFFlameThrower) return true;

	if (v_bDisableWhenAttacking->GetBool() && (cmd->buttons & IN_ATTACK)) return false;

	CachedEntity* closest = 0;
	float closest_dist = 0.0f;

	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (!ent || ent->m_bNULL || ent->m_bDormant) continue;
		if (!ShouldReflect(ent->m_pEntity)) continue;
		//if (ent->Var<Vector>(eoffsets.vVelocity).IsZero(1.0f)) continue;
		float dist = ent->m_pEntity->GetAbsOrigin().DistToSqr(g_pLocalPlayer->v_Origin);
		if (dist < closest_dist || !closest) {
			closest = ent;
			closest_dist = dist;
		}
	}

	if (closest_dist == 0 || closest_dist > v_iReflectDistance->GetInt() * v_iReflectDistance->GetInt()) return true;

	Vector tr = (closest->m_pEntity->GetAbsOrigin() - g_pLocalPlayer->v_Eye);
	Vector angles;
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
	g_pLocalPlayer->bUseSilentAngles = true;
	cmd->buttons |= IN_ATTACK2;

	return true;
}

void AutoReflect::PaintTraverse(void*, unsigned int, bool, bool) {}

