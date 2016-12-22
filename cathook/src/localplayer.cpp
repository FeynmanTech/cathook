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
	entity = interfaces::entityList->GetClientEntity(entity_idx);
	team = GetVar<int>(entity, netvar.iTeamNum);
	life_state = GetVar<char>(entity, netvar.iLifeState);
	v_ViewOffset = GetVar<Vector>(entity, netvar.vViewOffset);
	v_Origin = entity->GetAbsOrigin();
	v_Eye = v_Origin + v_ViewOffset;
	cond_0 = GetVar<int>(entity, netvar.iCond);
	clazz = GetVar<int>(entity, netvar.iClass);
	health = GetVar<int>(entity, netvar.iHealth);
	this->bUseSilentAngles = false;
	bZoomed = GetVar<int>(entity, netvar.iFOV) == 20; //!= GetVar<int>(entity, netvar.iDefaultFOV);
	if (bZoomed) {
		if (flZoomBegin == 0.0f) flZoomBegin = interfaces::gvars->curtime;
	} else {
		flZoomBegin = 0.0f;
	}


	int hWeapon = GetVar<int>(entity, netvar.hActiveWeapon);
	if (hWeapon) {
		weapon = interfaces::entityList->GetClientEntity(hWeapon & 0xFFF);
		bIsReloading = (GetVar<int>(weapon, netvar.iReloadMode) == 1);
	} else {
		weapon = 0;
		bIsReloading = false;
	}
}

LocalPlayer* g_pLocalPlayer = 0;
