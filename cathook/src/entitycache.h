/*
 * entitycache.h
 *
 *  Created on: Nov 7, 2016
 *      Author: nullifiedcat
 */

#ifndef ENTITYCACHE_H_
#define ENTITYCACHE_H_

#include "enums.h"
#include "itemtypes.h"
#include <mathlib/vector.h>
#include <mathlib/mathlib.h>
#include <cdll_int.h>

#define ENTITY_CACHE_PROFILER false

class IClientEntity;
struct ESPStringCompound;
struct model_t;
struct mstudiohitboxset_t;
struct mstudiobbox_t;
class ClientClass;

#include "cachedentity.h"

#define MAX_ENTITIES 2048

class EntityCache {
public:
	EntityCache();

	void Update();
	void Zero();

	std::array<CachedEntity, MAX_ENTITIES> array;
	CachedEntity& Entity(int idx);

	int max;
};

extern EntityCache gEntityCache;

#endif /* ENTITYCACHE_H_ */
