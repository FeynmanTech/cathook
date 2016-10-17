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
	team = GetEntityValue<int>(entity, entityvars.iTeamNum);
	life_state = GetEntityValue<char>(entity, entityvars.iLifeState);
	v_ViewOffset = GetEntityValue<Vector>(entity, entityvars.vViewOffset);
	v_Origin = entity->GetAbsOrigin();
	v_Eye = v_Origin + v_ViewOffset;
}

LocalPlayer* g_pLocalPlayer = 0;
