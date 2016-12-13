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
	team = GetEntityValue<int>(entity, eoffsets.iTeamNum);
	life_state = GetEntityValue<char>(entity, eoffsets.iLifeState);
	v_ViewOffset = GetEntityValue<Vector>(entity, eoffsets.vViewOffset);
	v_Origin = entity->GetAbsOrigin();
	v_Eye = v_Origin + v_ViewOffset;
	cond_0 = GetEntityValue<int>(entity, eoffsets.iCond);
	clazz = GetEntityValue<int>(entity, eoffsets.iClass);
	health = GetEntityValue<int>(entity, eoffsets.iHealth);
	this->bUseSilentAngles = false;
	if (cond_0 & cond::zoomed) {
		if (flZoomBegin == 0.0f) flZoomBegin = interfaces::gvars->curtime;
	} else {
		flZoomBegin = 0.0f;
	}


	int hWeapon = GetEntityValue<int>(entity, eoffsets.hActiveWeapon);
	if (hWeapon) {
		weapon = interfaces::entityList->GetClientEntity(hWeapon & 0xFFF);
		bIsReloading = (GetEntityValue<int>(weapon, eoffsets.iReloadMode) == 1);
	} else {
		weapon = 0;
		bIsReloading = false;
	}
}

LocalPlayer* g_pLocalPlayer = 0;
