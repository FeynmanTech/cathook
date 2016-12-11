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

bool IsReflectableProjectile(IClientEntity* ent) {
	if (!ent) return false;
	switch (ent->GetClientClass()->m_ClassID) {
	case ClassID::CTFProjectile_Arrow:
	case ClassID::CTFProjectile_Cleaver:
	case ClassID::CTFProjectile_Flare:
	case ClassID::CTFProjectile_HealingBolt:
	case ClassID::CTFProjectile_Jar:
	case ClassID::CTFProjectile_JarMilk:
	case ClassID::CTFProjectile_Rocket:
	case ClassID::CTFProjectile_SentryRocket:
	case ClassID::CTFProjectile_EnergyBall:
		return true;
	case ClassID::CTFGrenadePipebombProjectile:

	}
	return false;
}

// Hack Methods

AutoReflect::AutoReflect() {
	v_bEnabled = CreateConVar("u_reflect_enabled", "0", "Autoreflect");
	v_iReflectDistance = CreateConVar("u_reflect_distance", "200", "Autoreflect distance");
	v_bDisableWhenAttacking = CreateConVar("u_reflect_only_idle", "0", "Autoreflect active only when not shooting");
	v_bReflectStickies = CreateConVar("u_reflect_stickybombs", "0", "Reflect stickies");
}
// TODO
bool AutoReflect::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;
	if (g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID != ClassID::CTFFlameThrower) return true;

	CachedEntity* closest = 0;
	float closest_dist = 0.0f;

	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (!ent || ent->m_bNULL || ent->m_bDormant) continue;
		if (!IsReflectableProjectile(ent->m_pEntity)) continue;
		//if (ent->Var<Vector>(eoffsets.vVelocity).IsZero(1.0f)) continue;
		if (ent->Var<int>(eoffsets.iTeamNum) == g_pLocalPlayer->team) continue;
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
	if (!(cmd->buttons & IN_ATTACK))
		cmd->buttons |= IN_ATTACK2;

	return true;
}

void AutoReflect::PaintTraverse(void*, unsigned int, bool, bool) {}

