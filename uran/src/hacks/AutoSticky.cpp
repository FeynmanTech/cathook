/*
 * AutoSticky.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: nullifiedcat
 */

#include "AutoSticky.h"

#include "../entity.h"
#include "../interfaces.h"
#include "../entitycache.h"
#include "../helpers.h"
#include "../fixsdk.h"
#include <icliententitylist.h>
#include <icliententity.h>

const char* AutoSticky::GetName() {
	return "AUTO-STICKY";
}

// TODO scottish cyclops
AutoSticky::AutoSticky() {
	this->v_bBuildings = CreateConVar("u_sticky_buildings", "1", "Stickies detonate at enemies' buildings");
	this->v_bEnabled = CreateConVar("u_sticky_enabled", "1", "Enable stickybot");
	this->v_bScottish = CreateConVar("u_sticky_scottish", "0", "Enable stickybot scottish resistance compatability");
}

bool AutoSticky::ShouldDetonate(IClientEntity* bomb) {
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (ent->m_iClassID != ClassID::CTFPlayer && !(this->v_bBuildings->GetBool() && IsBuilding(ent->m_pEntity))) return false;
		if (ent->m_iTeam == GetEntityValue<int>(bomb, eoffsets.iTeamNum)) return false;
		if (ent->m_pEntity->GetAbsOrigin().DistToSqr(bomb->GetAbsOrigin())) return false;
		return true;
	}
	return false;
}

bool AutoSticky::CreateMove(void*, float, CUserCmd* cmd) {
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (ent->m_iClassID != ClassID::CTFGrenadePipebombProjectile) return true;

	}
	return true;
}

void AutoSticky::PaintTraverse(void*, unsigned int, bool, bool) {

}
