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
#include "../localplayer.h"
#include "../usercmd.h"
#include "../sdk/in_buttons.h"
#include "../logging.h"
#include "../fixsdk.h"
#include <icliententitylist.h>
#include <icliententity.h>

DEFINE_HACK_SINGLETON(AutoSticky);

const char* AutoSticky::GetName() {
	return "AUTO-STICKY";
}

// TODO scottish cyclops
AutoSticky::AutoSticky() {
	this->v_flDetonateDistance = CreateConVar("u_sticky_distance", "200", "Sticky detonation distance");
	this->v_bBuildings = CreateConVar("u_sticky_buildings", "1", "Stickies detonate at enemies' buildings");
	this->v_bEnabled = CreateConVar("u_sticky_enabled", "1", "Enable stickybot");
	this->v_bScottish = CreateConVar("u_sticky_scottish", "0", "Enable stickybot scottish resistance compatability");
}

bool AutoSticky::ShouldDetonate(IClientEntity* bomb) {
	//logging::Info("Should detonate?");
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (ent->m_bNULL) continue;
		if (ent->m_iClassID != ClassID::CTFPlayer && !(this->v_bBuildings->GetBool() && IsBuilding(ent->m_pEntity))) continue;
		if (ent->m_iTeam == GetEntityValue<int>(bomb, eoffsets.iTeamNum)) continue;
		if (ent->m_pEntity->GetAbsOrigin().DistToSqr(bomb->GetAbsOrigin()) > this->v_flDetonateDistance->GetFloat() * this->v_flDetonateDistance->GetFloat()) continue;
		return true;
	}
	return false;
}

bool AutoSticky::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (ent->m_bNULL) continue;
		if (ent->m_iClassID != ClassID::CTFGrenadePipebombProjectile) continue;
		if (g_pLocalPlayer->entity) {
			IClientEntity* owner = interfaces::entityList->GetClientEntity(ent->Var<int>(eoffsets.hThrower) & 0xFFF);
			//logging::Info("Owner: 0x%08x", owner);
			if (!owner) continue;
			if (owner != g_pLocalPlayer->entity) continue;
		} else continue;
		if (ShouldDetonate(ent->m_pEntity)) {
			//logging::Info("Detonate!");
			cmd->buttons |= IN_ATTACK2;
		}
	}
	return true;
}

void AutoSticky::PaintTraverse(void*, unsigned int, bool, bool) {

}
