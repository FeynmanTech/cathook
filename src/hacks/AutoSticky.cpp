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

// TODO scottish cyclops
AutoSticky::AutoSticky() {
	this->v_flDetonateDistance = new CatVar(CV_INT, "sticky_distance", "200", "Distance", NULL, "Maximum distance to detonate");
	this->v_bBuildings = new CatVar(CV_SWITCH, "sticky_buildings", "1", "Detonate buildings", NULL, "Stickies react at buildings");
	this->v_bEnabled = new CatVar(CV_SWITCH, "sticky_enabled", "0", "Enable", NULL, "Master AutoSticky switch");
	this->v_bScottish = new CatVar(CV_SWITCH, "sticky_scottish", "0", "Scottish", NULL, "Scottish Resistance mode - NOT YET IMPLEMENTED");
}

bool AutoSticky::ShouldDetonate(CachedEntity* bomb) {
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		if (ent->m_Type != ENTITY_PLAYER && (ent->m_Type != ENTITY_BUILDING || !this->v_bBuildings->GetBool())) continue;
		if (ent->m_iTeam == CE_INT(bomb, netvar.iTeamNum)) continue;
		if (ent->m_vecOrigin.DistToSqr(bomb->m_vecOrigin) > SQR(this->v_flDetonateDistance->GetFloat())) continue;
		return true;
	}
	return false;
}

void AutoSticky::ProcessUserCmd(CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return;
	if (CE_BAD(g_pLocalPlayer->entity)) return;
	if (CE_BAD(g_pLocalPlayer->weapon())) return;
	if (g_pLocalPlayer->life_state) return;
	if (g_pLocalPlayer->clazz != tf_demoman) return;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		if (ent->m_iClassID != g_pClassID->CTFGrenadePipebombProjectile) continue;
		if (CE_INT(ent, netvar.iPipeType) != 1) continue;
		if ((CE_INT(ent, netvar.hThrower) & 0xFFF) != g_pLocalPlayer->entity->m_IDX) continue;
		if (ShouldDetonate(ent)) {
			cmd->buttons |= IN_ATTACK2;
		}
	}
	return;
}
