/*
 * targeting.cpp
 *
 *  Created on: Feb 18, 2017
 *      Author: nullifiedcat
 */

#include "targeting.h"

#include "../cachedentity.h"

namespace targeting {

CatVar teammates("target_teammates", "0", "Target teammates", nullptr, "Target teammates?");
CatVar buildings("target_buildings", "1", "Target buildings", nullptr, "Target buildings?");
CatVar friendly_buildings("target_buildings_friendly", "0", "Target friendly objects", nullptr, "Target friendly buildings?");
CatVar stickybombs("target_stickybombs", "0", "Target stickybombs", nullptr, "Target stickybombs?");
CatVar taunting("target_taunting", "1", "Target taunting", nullptr, "Target taunting players?");
CatVar invisible("target_invisible", "0", "Target invisible", nullptr, "Target invisible players?");

bool ShouldTarget(CachedEntity& entity) {
	k_EEntityType type = entity.Type();
	switch (type) {
	case ENTITY_BUILDING:
		if (!buildings) return false;
		break;
	case ENTITY_PROJECTILE:
		if (!(stickybombs && entity.Projectile() == PROJ_STICKY)) {
			return false;
		}
		break;
	case ENTITY_PLAYER:

	}
}

}
