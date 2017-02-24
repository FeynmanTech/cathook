/*
 * entitycache.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"
#include "profiler.h"

#include <time.h>

EntityCache::EntityCache() : array {} {
	for (int i = 0; i < array.size(); i++) {
		array.at(i).SetIDX(i);
	}
}

void EntityCache::Update() {
	max = g_IEntityList->GetHighestEntityIndex();
	for (int i = 0; i < max && i < MAX_ENTITIES; i++) {
		array.at(i).Update();
	}
}

void EntityCache::Zero() {
	for (auto entity : array) entity.Zero();
}

CachedEntity& EntityCache::Entity(int idx) {
	return &array[idx];
}

EntityCache gEntityCache;
