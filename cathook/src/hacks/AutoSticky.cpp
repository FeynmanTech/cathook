/*
 * AutoSticky.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: nullifiedcat
 */

#include "AutoSticky.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(AutoSticky);

const char* AutoSticky::GetName() {
	return "AUTO-STICKY";
}

// TODO scottish cyclops
AutoSticky::AutoSticky() {
	this->v_flDetonateDistance = CreateConVar(CON_PREFIX "sticky_distance", "200", "Distance");
	this->v_bBuildings = CreateConVar(CON_PREFIX "sticky_buildings", "1", "Detonate buildings");
	this->v_bEnabled = CreateConVar(CON_PREFIX "sticky_enabled", "0", "Enable");
	this->v_bScottish = CreateConVar(CON_PREFIX "sticky_scottish", "0", "Scottish");
}

bool AutoSticky::ShouldDetonate(IClientEntity* bomb) {
	//logging::Info("Should detonate?");
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (ent->m_bNULL) continue;
		if (ent->m_iClassID != ClassID::CTFPlayer && !(this->v_bBuildings->GetBool() && IsBuilding(ent->m_pEntity))) continue;
		if (ent->m_iTeam == GetEntityValue<int>(bomb, netvar.iTeamNum)) continue;
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
			IClientEntity* owner = interfaces::entityList->GetClientEntity(ent->Var<int>(netvar.hThrower) & 0xFFF);
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
