/*
 * localplayer.cpp
 *
 *  Created on: Oct 15, 2016
 *      Author: nullifiedcat
 */

#include "localplayer.h"
#include "interfaces.h"
#include "entity.h"

#include "fixsdk.h"
#include <cdll_int.h>
#include <icliententitylist.h>
#include <mathlib/vector.h>
#include <icliententity.h>

void LocalPlayer::Update() {
	entity_idx = interfaces::engineClient->GetLocalPlayer();
	entity = interfaces::entityList->GetClientEntity(entity_idx);
	team = GetEntityValue<int>(entity, eoffsets.iTeamNum);
	life_state = GetEntityValue<char>(entity, eoffsets.iLifeState);
	v_ViewOffset = GetEntityValue<Vector>(entity, eoffsets.vViewOffset);
	v_Origin = entity->GetAbsOrigin();
	v_Eye = v_Origin + v_ViewOffset;
	cond_0 = GetEntityValue<int>(entity, eoffsets.iCond);
	int hWeapon = GetEntityValue<int>(entity, eoffsets.hActiveWeapon);
	if (hWeapon)
		weapon = interfaces::entityList->GetClientEntity(hWeapon & 0xFFF);
	else weapon = 0;
}

LocalPlayer* g_pLocalPlayer = 0;
