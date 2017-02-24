/*
 * AutoSticky.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: nullifiedcat
 */

#include "AutoSticky.h"

#include "../common.h"
#include "../sdk.h"

namespace hacks { namespace tf { namespace autosticky {

CatVar enabled(CV_SWITCH, "sticky_enabled", "0", "AutoSticky", NULL, "Automatically detonate sticky bombs if they are near enemy players/buildings");
CatVar buildings(CV_SWITCH, "sticky_buildings", "1", "Detonate buildings", NULL, "Stickies react at buildings");
CatVar distance(CV_INT, "sticky_distance", "200", "Distance", NULL, "Maximum distance to detonate");

std::vector<CachedEntity*> targets;
std::vector<CachedEntity*> bombs;

bool IsATarget(CachedEntity& entity) {
	if (entity.Type() != ENTITY_PLAYER) return false;
	if (entity.var<bool>(netvar.iLifeState)) return false;
	if (!entity.Enemy()) return false;
	if (IsPlayerInvulnerable(&entity)) return false;
	return true;
}

bool IsABomb(CachedEntity& entity) {
	if (entity.Projectile() != PROJ_STICKY) return false;
	if (entity.var<int>(netvar.hThrower) & 0xFFF != g_LocalPlayer.entity->m_IDX) return false;
	return true;
}

void Reset() {
	targets.clear();
	bombs.clear();
}

void ProcessEntity(CUserCmd* cmd, CachedEntity& entity) {
	if (!enabled) return;
	if (IsATarget(entity)) targets.push_back(&entity);
	if (IsABomb(entity)) bombs.push_back(&entity);
}

void CalculateAndDetonate(CUserCmd* cmd) {
	if (g_LocalPlayer.entity->Class() != tf_demoman) return;
	bool detonate = false;
	for (auto sticky : bombs) {
		for (auto target : targets) {
			if (sticky->Origin().DistToSqr(target->Origin()) < SQR((float)distance)) {
				detonate = true;
			}
			if (detonate) break;
		}
		if (detonate) break;
	}
	if (detonate) {
		cmd->buttons |= IN_ATTACK2;
	}
}

}}}
