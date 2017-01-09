/*
 * localplayer.cpp
 *
 *  Created on: Oct 15, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"

void LocalPlayer::Update() {
	entity_idx = interfaces::engineClient->GetLocalPlayer();
	entity = ENTITY(entity_idx);
	if (CE_BAD(entity)) {
		logging::Info("Local Player is BAD CACHED ENTITY!");
		return;
	}
	team = CE_INT(entity, netvar.iTeamNum);
	life_state = CE_BYTE(entity, netvar.iLifeState);
	v_ViewOffset = CE_VECTOR(entity, netvar.vViewOffset);
	v_Origin = entity->m_vecOrigin;
	v_Eye = v_Origin + v_ViewOffset;
	cond_0 = CE_INT(entity, netvar.iCond);
	cond_1 = CE_INT(entity, netvar.iCond1);
	cond_2 = CE_INT(entity, netvar.iCond1);
	cond_3 = CE_INT(entity, netvar.iCond3);
	clazz = CE_INT(entity, netvar.iClass);
	health = CE_INT(entity, netvar.iHealth);
	this->bUseSilentAngles = false;
	bZoomed = CE_INT(entity, netvar.iFOV) == 20; //!= NET_INT(entity, netvar.iDefaultFOV);
	if (bZoomed) {
		if (flZoomBegin == 0.0f) flZoomBegin = interfaces::gvars->curtime;
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
