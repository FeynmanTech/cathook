/*
 * Glow.cpp
 *
 *  Created on: Jan 16, 2017
 *      Author: nullifiedcat
 */

#include "Glow.h"
#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(Glow);

const char* Glow::GetName() { return "GLOW"; }

Glow::Glow() {
	v_bEnabled = CREATE_CV(CV_SWITCH, "glow_enabled", "0", "Glow ESP - DO NOT USE!");
	m_bEnabledOnce = false;
}

bool Glow::CreateMove(void*, float, CUserCmd*) {
	if (v_bEnabled->GetBool()) m_bEnabledOnce = true;
	if (!m_bEnabledOnce) return true;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (!CE_GOOD_NO_DORMANT_CHECK(ent)) continue;
		//logging::Info("Processing Glow %i P %i", ent->m_IDX, ent->m_Type == ENTITY_PLAYER ? (v_bEnabled->GetBool() && ent != LOCAL_E && !ent->m_pEntity->IsDormant() && !CE_BYTE(ent, netvar.iLifeState)) : 2);
		if (ent->m_Type == ENTITY_PLAYER)
			CE_BYTE(ent, netvar.bGlowEnabled) = (v_bEnabled->GetBool() && (ent != LOCAL_E) && !ent->m_pEntity->IsDormant() && !CE_BYTE(ent, netvar.iLifeState));
		//if (ent->m_Type == ENTITY_BUILDING)
		//	CE_BYTE(ent, netvar.bGlowEnabled) = (v_bEnabled->GetBool() && !ent->m_pEntity->IsDormant());
	}
	return true;
}

void Glow::PaintTraverse(void*, unsigned int, bool, bool) {}
void Glow::LevelInit(const char*) {
	m_bEnabledOnce = false;
}

void Glow::LevelShutdown() {

}
