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

bool AutoReflect::ShouldReflect(CachedEntity* ent) {
	if (ent->m_Type != ENTITY_PROJECTILE) return false;
	if (CE_INT(ent, netvar.iTeamNum) == g_pLocalPlayer->team) return false;
	// If projectile is already deflected, don't deflect it again.
	if (CE_INT(ent, ent->m_bGrenadeProjectile ?
			/* NetVar for grenades */ netvar.Grenade_iDeflected :
			/* For rockets */ netvar.Rocket_iDeflected)) return false;
	if (ent->m_iClassID == ClassID::CTFGrenadePipebombProjectile) {
		if (CE_INT(ent, netvar.iPipeType) == 1) {
			if (!v_bReflectStickies->GetBool()) return false;
		}
	}
	return true;
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
	if (CE_BAD(g_pLocalPlayer->weapon()) || CE_BAD(g_pLocalPlayer->entity)) return true;
	if (g_pLocalPlayer->weapon()->m_iClassID != ClassID::CTFFlameThrower) return true;
	if (v_bDisableWhenAttacking->GetBool() && (cmd->buttons & IN_ATTACK)) return false;

	CachedEntity* closest = 0;
	float closest_dist = 0.0f;

	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		if (!ShouldReflect(ent)) continue;
		//if (ent->Var<Vector>(eoffsets.vVelocity).IsZero(1.0f)) continue;
		float dist = ent->m_vecOrigin.DistToSqr(g_pLocalPlayer->v_Origin);
		if (dist < closest_dist || !closest) {
			closest = ent;
			closest_dist = dist;
		}
	}

	if (closest_dist == 0 || closest_dist > SQR(v_iReflectDistance->GetInt())) return true;

	Vector tr = (closest->m_vecOrigin - g_pLocalPlayer->v_Eye);
	Vector angles;
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
	g_pLocalPlayer->bUseSilentAngles = true;
	cmd->buttons |= IN_ATTACK2;

	return true;
}

void AutoReflect::PaintTraverse(void*, unsigned int, bool, bool) {}

