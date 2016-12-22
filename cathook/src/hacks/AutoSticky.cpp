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
	this->v_flDetonateDistance = CREATE_CV(CV_INT, "sticky_distance", "200", "Distance");
	this->v_bBuildings = CREATE_CV(CV_SWITCH, "sticky_buildings", "1", "Detonate buildings");
	this->v_bEnabled = CREATE_CV(CV_SWITCH, "sticky_enabled", "0", "Enable");
	this->v_bScottish = CREATE_CV(CV_SWITCH, "sticky_scottish", "0", "Scottish");
}

bool AutoSticky::ShouldDetonate(CachedEntity* bomb) {
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (CE_BAD(ent)) continue;
		if (ent->m_Type != ENTITY_PLAYER && (ent->m_Type != ENTITY_BUILDING || !this->v_bBuildings->GetBool())) continue;
		if (ent->m_iTeam == CE_INT(bomb, netvar.iTeamNum)) continue;
		if (ent->m_vecOrigin.DistToSqr(bomb->m_vecOrigin) > SQR(this->v_flDetonateDistance->GetFloat())) continue;
		return true;
	}
	return false;
}

bool AutoSticky::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (CE_BAD(g_pLocalPlayer->entity)) return true;
	for (int i = 0; i < gEntityCache.m_nMax; i++) {
		CachedEntity* ent = gEntityCache.GetEntity(i);
		if (CE_BAD(ent)) continue;
		if (ent->m_iClassID != ClassID::CTFGrenadePipebombProjectile) continue;
		if (CE_INT(ent, netvar.iPipeType) != 1) continue;
		if ((CE_INT(ent, netvar.hThrower)) & 0xFFF != g_pLocalPlayer->entity->m_IDX) continue;
		if (ShouldDetonate(ent)) {
			cmd->buttons |= IN_ATTACK2;
		}
	}
	return true;
}

void AutoSticky::PaintTraverse(void*, unsigned int, bool, bool) {

}
