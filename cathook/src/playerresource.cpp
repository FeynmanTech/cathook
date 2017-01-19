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
		CachedEntity* ent = ENTITY(i);
		if (CE_GOOD(ent) && ent->m_iClassID == ClassID::CTFPlayerResource) {
			m_pEntity = ent;
		}
	}
}

int TFPlayerResource::GetMaxHealth(CachedEntity* player) {
	if (!m_pEntity) return 0;
	int idx = player->m_IDX;
	if (idx >= 64 || idx < 0) return 0;
	return *(int*)((unsigned int)RAW_ENT(m_pEntity) + netvar.iMaxHealth + 4 * idx);
}

int TFPlayerResource::GetMaxBuffedHealth(CachedEntity* player) {
	if (!m_pEntity) return 0;
	int idx = player->m_IDX;
	if (idx >= 64 || idx < 0) return 0;
	return *(int*)((unsigned int)RAW_ENT(m_pEntity) + netvar.iMaxBuffedHealth + 4 * idx);
}

int TFPlayerResource::GetClass(CachedEntity* player) {
	if (CE_BAD(m_pEntity)) return 0;
	int idx = player->m_IDX;
	if (idx >= 64 || idx < 0) return 0;
	return *(int*)((unsigned int)RAW_ENT(m_pEntity) + netvar.iPlayerClass + 4 * idx);
}


TFPlayerResource* g_pPlayerResource = 0;
