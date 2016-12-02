/*
 * TargetSystemDistance.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#include "TargetSystemDistance.h"

bool TargetSystemDistance::ShouldTarget(int idx) {
	//CachedEntity* ent = gEntityCache.GetEntity(idx);
	//float fov = GetFov(g_pLocalPlayer->v_OrigViewangles, g_pLocalPlayer->v_Eye, GetHitboxPosition(ent->m_pEntity, ));
	return false;
}

int TargetSystemDistance::GetScore(int idx) {
	return 0;
}



