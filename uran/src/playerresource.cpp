/*
 * playerresource.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: nullifiedcat
 */

#include "playerresource.h"
#include "interfaces.h"
#include "enums.h"
#include "entity.h"

#include "fixsdk.h"
#include <icliententity.h>
#include <icliententitylist.h>
#include <client_class.h>

void TFPlayerResource::Update() {
	m_pEntity = 0;
	for (int i = 0; i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
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
	return *(int*)((unsigned int)m_pEntity + eoffsets.iMaxHealth + 4 * idx);
}

TFPlayerResource* g_pPlayerResource = 0;
