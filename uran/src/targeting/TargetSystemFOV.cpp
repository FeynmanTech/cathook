/*
 * TargetSystemFOV.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#include "TargetSystemFOV.h"

bool TargetSystemFOV::ShouldTarget(int idx) {
	//CachedEntity* ent = gEntityCache.GetEntity(idx);
	//float fov = GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, GetHitboxPosition(ent->m_pEntity, ));
	return false;
}

int TargetSystemFOV::GetScore(int idx) {
	return 0;
}
