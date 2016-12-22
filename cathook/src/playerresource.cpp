/*
 * playerresource.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: nullifiedcat
 */

#include "common.h"

void TFPlayerResource::Update() {
	m_pEntity = 0;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		IClientEntity* ent = ENTITY(i);
		if (ent && ent->GetClientClass()->m_ClassID == ClassID::CTFPlayerResource) {
			m_pEntity = ent;
		}
	}
}

int TFPlayerResource::GetMaxHealth(IClientEntity* player) {
	if (!player) return 0;
	if (!m_pEntity) return 0;
	int idx = player->entindex();
	if (idx >= 64 || idx < 0) return 0;
	return *(int*)((unsigned int)m_pEntity + netvar.iMaxHealth + 4 * idx);
}

int TFPlayerResource::GetMaxBuffedHealth(IClientEntity* player) {
	if (!player) return 0;
	if (!m_pEntity) return 0;
	int idx = player->entindex();
	if (idx >= 64 || idx < 0) return 0;
	return *(int*)((unsigned int)m_pEntity + netvar.iMaxBuffedHealth + 4 * idx);
}


TFPlayerResource* g_pPlayerResource = 0;
