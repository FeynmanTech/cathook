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
	team = NET_INT(entity, netvar.iTeamNum);
	life_state = NET_BYTE(entity, netvar.iLifeState);
	v_ViewOffset = NET_VECTOR(entity, netvar.vViewOffset);
	v_Origin = entity->GetAbsOrigin();
	v_Eye = v_Origin + v_ViewOffset;
	cond_0 = NET_INT(entity, netvar.iCond);
	clazz = NET_INT(entity, netvar.iClass);
	health = NET_INT(entity, netvar.iHealth);
	this->bUseSilentAngles = false;
	bZoomed = NET_INT(entity, netvar.iFOV) == 20; //!= NET_INT(entity, netvar.iDefaultFOV);
	if (bZoomed) {
		if (flZoomBegin == 0.0f) flZoomBegin = interfaces::gvars->curtime;
	} else {
		flZoomBegin = 0.0f;
	}


	int hWeapon = NET_INT(entity, netvar.hActiveWeapon);
	if (hWeapon && (hWeapon & 0xFFF) < HIGHEST_ENTITY) {
		weapon = ENTITY(hWeapon & 0xFFF);
		if (weapon)
			bIsReloading = (NET_INT(weapon, netvar.iReloadMode) == 1);
	} else {
		weapon = 0;
		bIsReloading = false;
	}
}

LocalPlayer* g_pLocalPlayer = 0;
