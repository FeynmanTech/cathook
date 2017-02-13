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

Glow::Glow() {
	v_bEnabled = new CatVar(CV_SWITCH, "glow_enabled", "0", "Glow ESP", NULL, "Simply adds glow to all entities - DO NOT USE!");
	m_bEnabledOnce = false;
}

void Glow::ProcessUserCmd(CUserCmd*) {
	if (v_bEnabled->GetBool()) m_bEnabledOnce = true;
	if (!m_bEnabledOnce) return;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (!CE_GOOD_NO_DORMANT_CHECK(ent)) continue;
		if (ent->m_Type == ENTITY_PLAYER)
			CE_BYTE(ent, netvar.bGlowEnabled) = (v_bEnabled->GetBool() && (ent != LOCAL_E) && !ent->entptr->IsDormant() && !CE_BYTE(ent, netvar.iLifeState));
	}
	return;
}

void Glow::OnLevelInit() {
	m_bEnabledOnce = false;
}

void Glow::OnLevelShutdown() {
	m_bEnabledOnce = false;
}
