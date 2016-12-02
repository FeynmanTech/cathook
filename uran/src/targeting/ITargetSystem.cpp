/*
 * ITargetSystem.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#include "ITargetSystem.h"
#include "../helpers.h"

TargetingConVars* g_pTargetingConVars = 0;

ITargetSystem::~ITargetSystem() {}

void InitTargetingConVars() {
	g_pTargetingConVars = new TargetingConVars();
	g_pTargetingConVars->bTargetInvisible = CreateConVar("u_target_ignore_cloak", "0", "Ignore cloak when targeting");
	g_pTargetingConVars->flMaxFOV = CreateConVar("u_target_max_fov", "0", "Maximum FOV when targeting");
	g_pTargetingConVars->iMaxDistance = CreateConVar("u_target_max_distance", "0", "Maximum distance when targeting");
}


