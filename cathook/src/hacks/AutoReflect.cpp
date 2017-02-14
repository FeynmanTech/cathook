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

bool AutoReflect::ShouldReflect(CachedEntity* ent) {
	if (CE_BAD(ent)) return false;
	if (ent->m_Type != ENTITY_PROJECTILE) return false;
	if (CE_INT(ent, netvar.iTeamNum) == g_LocalPlayer->team) return false;
	// If projectile is already deflected, don't deflect it again.
	if (CE_INT(ent, (ent->m_bGrenadeProjectile ?
			/* NetVar for grenades */ netvar.Grenade_iDeflected :
			/* For rockets */ netvar.Rocket_iDeflected))) return false;
	if (ent->clazz == g_pClassID->CTFGrenadePipebombProjectile) {
		if (CE_INT(ent, netvar.iPipeType) == 1) {
			if (!v_bReflectStickies->GetBool()) return false;
		}
	}
	return true;
}

// Hack Methods

AutoReflect::AutoReflect() {
	v_bEnabled = new CatVar(CV_SWITCH, "reflect_enabled", "0", "Enable", NULL, "Master AutoReflect switch");
	v_iReflectDistance = new CatVar(CV_INT, "reflect_distance", "200", "Distance", NULL, "Maximum distance to reflect at", true, 300.0f);
	v_bDisableWhenAttacking = new CatVar(CV_SWITCH, "reflect_only_idle", "0", "Only when not shooting", NULL, "Don't AutoReflect if you're holding M1");
	v_bReflectStickies = new CatVar(CV_SWITCH, "reflect_stickybombs", "0", "Reflect stickies", NULL, "Reflect Stickybombs");
}
// TODO
void AutoReflect::ProcessUserCmd(CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return;
	if (CE_BAD(g_LocalPlayer->weapon()) || CE_BAD(g_LocalPlayer->entity)) return;
	if (g_LocalPlayer->life_state) return;
	if (g_LocalPlayer->weapon()->clazz != g_pClassID->CTFFlameThrower) return;
	if (v_bDisableWhenAttacking->GetBool() && (cmd->buttons & IN_ATTACK)) return;

	CachedEntity* closest = 0;
	float closest_dist = 0.0f;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		if (!ShouldReflect(ent)) continue;
		//if (ent->Var<Vector>(eoffsets.vVelocity).IsZero(1.0f)) continue;
		float dist = ent->m_vecOrigin.DistToSqr(g_LocalPlayer->v_Origin);
		if (dist < closest_dist || !closest) {
			closest = ent;
			closest_dist = dist;
		}
	}
	if (CE_BAD(closest)) return;
	if (closest_dist == 0 || closest_dist > SQR(v_iReflectDistance->GetInt())) return;

	Vector tr = (closest->m_vecOrigin - g_LocalPlayer->v_Eye);
	Vector angles;
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
	g_LocalPlayer->bUseSilentAngles = true;
	cmd->buttons |= IN_ATTACK2;
	return;
}
