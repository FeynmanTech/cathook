/*
 * localplayer.cpp
 *
 *  Created on: Oct 15, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"

void LocalPlayer::Update() {
	entity_idx = g_IEngine->GetLocalPlayer();
	entity = ENTITY(entity_idx);
	if (CE_BAD(entity)) {
		return;
	}
	team = CE_INT(entity, netvar.iTeamNum);
	life_state = CE_BYTE(entity, netvar.iLifeState);
	v_ViewOffset = CE_VECTOR(entity, netvar.vViewOffset);
	v_Origin = entity->m_vecOrigin;
	v_Eye = v_Origin + v_ViewOffset;
	clazz = CE_INT(entity, netvar.iClass);
	health = CE_INT(entity, netvar.iHealth);
	this->bUseSilentAngles = false;
	bZoomed = CE_INT(entity, netvar.iFOV) == 20; //!= NET_INT(entity, netvar.iDefaultFOV);
	if (bZoomed) {
		if (flZoomBegin == 0.0f) flZoomBegin = g_pGlobals->curtime;
	} else {
		flZoomBegin = 0.0f;
	}
}

CachedEntity* LocalPlayer::weapon() {
	if (CE_BAD(entity)) return 0;
	int handle = CE_INT(entity, netvar.hActiveWeapon);
	int eid = handle & 0xFFF;
	if (IDX_BAD(eid)) return 0;
	return ENTITY(eid);
}

LocalPlayer* g_pLocalPlayer = 0;
