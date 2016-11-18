/*
 * AutoReflect.cpp
 *
 *  Created on: Nov 18, 2016
 *      Author: nullifiedcat
 */

#include "AutoReflect.h"

#include "../localplayer.h"
#include "../entitycache.h"
#include "../usercmd.h"
#include "../helpers.h"

#include "../sdk/in_buttons.h"

#include "../fixsdk.h"
#include <icliententitylist.h>
#include <icliententity.h>
#include <convar.h>
#include <client_class.h>

// Hack Methods

void AutoReflect::Create() {

}

bool AutoReflect::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;

	CachedEntity* closest = 0;
	float closest_dist = 0.0f;

	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (!ent || ent->m_bNULL || ent->m_bDormant) continue;
		if (!ent->m_iClassID == ClassID::CTFProjectile_Rocket) continue;
		float dist = ent->m_pEntity->GetAbsOrigin().DistToSqr(g_pLocalPlayer->v_Origin);
		if (dist < closest_dist || !closest) {
			closest = ent;
			closest_dist = dist;
		}
	}

	if (closest_dist == 0 || closest_dist > v_iReflectDistance->GetInt()) return true;

	Vector tr = (closest->m_pEntity->GetAbsOrigin() - g_pLocalPlayer->v_Eye);
	Vector angles;
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
	cmd->buttons |= IN_ATTACK2;

	return true;
}

void AutoReflect::PaintTraverse(void*, unsigned int, bool, bool) {}
void AutoReflect::Destroy() {}
