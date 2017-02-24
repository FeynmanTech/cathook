/*
 * entityhitboxcache.h
 *
 *  Created on: Feb 13, 2017
 *      Author: nullifiedcat
 */

#ifndef ENTITYHITBOXCACHE_H_
#define ENTITYHITBOXCACHE_H_

#include "beforecheaders.h"
#include <array>
#include "aftercheaders.h"

#include <mathlib/vector.h>

#define CACHE_MAX_HITBOXES 64

struct mstudiobbox_t;
struct mstudiohitboxset_t;
struct model_t;
class CachedEntity;

struct CachedHitbox {
	Vector min;
	Vector max;
	Vector center;
	mstudiobbox_t* bbox;
};

class EntityHitboxCache {
public:
	EntityHitboxCache(CachedEntity& parent);

	void MarkDirty();
	CachedHitbox& GetHitbox(int id);
	bool VisibilityCheck(int id);
	void Init();
	int HitboxCount();

	bool dirty;
	std::array<bool, CACHE_MAX_HITBOXES> vdirty;
	std::array<bool, CACHE_MAX_HITBOXES> vcheck;
	std::array<bool, CACHE_MAX_HITBOXES> cdirty;
	std::array<CachedHitbox, CACHE_MAX_HITBOXES> cache;

	int m_nNumHitboxes;

	mstudiohitboxset_t* m_pHitboxSet;
	model_t* m_pLastModel;

	bool m_bModelSet;
	bool success;
	CachedEntity& parent;
};

#endif /* ENTITYHITBOXCACHE_H_ */
