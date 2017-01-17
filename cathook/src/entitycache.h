/*
 * entitycache.h
 *
 *  Created on: Nov 7, 2016
 *      Author: nullifiedcat
 */

#ifndef ENTITYCACHE_H_
#define ENTITYCACHE_H_

#include "enums.h"
#include "fixsdk.h"
#include <mathlib/vector.h>

struct matrix3x4_t;

#define ENTITY_CACHE_PROFILER false

class IClientEntity;
class Color;
struct ESPStringCompound;
struct player_info_s;
struct model_t;
struct mstudiohitboxset_t;
struct mstudiobbox_t;

#define MAX_STRINGS 16

#define CE_VAR(entity, offset, type) \
	NET_VAR(entity->m_pEntity, offset, type)

#define CE_INT(entity, offset) CE_VAR(entity, offset, int)
#define CE_FLOAT(entity, offset) CE_VAR(entity, offset, float)
#define CE_BYTE(entity, offset) CE_VAR(entity, offset, unsigned char)
#define CE_VECTOR(entity, offset) CE_VAR(entity, offset, Vector)

#define CE_GOOD_NO_DORMANT_CHECK(entity) (!g_Settings.bInvalid && dynamic_cast<CachedEntity*>(entity) && dynamic_cast<IClientEntity*>(entity->m_pEntity))
#define CE_GOOD(entity) (!g_Settings.bInvalid && dynamic_cast<CachedEntity*>(entity) && dynamic_cast<IClientEntity*>(entity->m_pEntity) && !entity->m_pEntity->IsDormant())
#define CE_BAD(entity) (!CE_GOOD(entity))

#define IDX_GOOD(idx) (idx >= 0 && idx < HIGHEST_ENTITY)
#define IDX_BAD(idx) !IDX_GOOD(idx)

#define PROXY_ENTITY false

#if PROXY_ENTITY == true
#define RAW_ENT(ce) ce->InternalEntity()
#else
#define RAW_ENT(ce) ce->m_pEntity
#endif


// This will be used later. maybe.
#define ENTITY_ITERATE_INT(iterator, entity, max) \
	for (int iterator = 0; iterator < max; iterator++) { \
		CachedEntity* entity = gEntityCache.GetEntity(iterator); \
		if (CE_BAD(entity)) continue;

#define ENTITY_ITERATE_EVERYTHING(iterator, entity) \
	ENTITY_ITERATE_INT(iterator, entity, gEntityCache.m_nMax)

#define ENTITY_ITERATE_PLAYERS(iterator, entity) \
	ENTITY_ITERATE_INT(iterator, entity, MIN(gEntityCache.m_nMax, 64))

#define END_ENTITY_ITERATING }

#define HIGHEST_ENTITY gEntityCache.m_nMax
#define ENTITY(idx) gEntityCache.GetEntity(idx)

struct CachedHitbox {
	Vector min;
	Vector max;
	Vector center;
	mstudiobbox_t* bbox;
};

#define CACHE_MAX_HITBOXES 64

class EntityHitboxCache {
public:
	EntityHitboxCache(CachedEntity* parent);
	~EntityHitboxCache();

	CachedHitbox* GetHitbox(int id);
	void Update();
	void InvalidateCache();
	bool VisibilityCheck(int id);

	bool* m_VisCheckValidationFlags;
	bool* m_VisCheck;

	int m_nNumHitboxes;
	model_t* m_pLastModel;
	bool m_bModelSet;
	mstudiohitboxset_t* m_pHitboxSet;
	bool m_bSuccess;
	CachedEntity* m_pParentEntity;
	bool* m_CacheValidationFlags;
	CachedHitbox* m_CacheInternal;
};

class CachedEntity {
public:
	CachedEntity();
	~CachedEntity();

	void Update(int idx);
	void AddESPString(Color color, Color background, const char* string, ...);
	ESPStringCompound GetESPString(int idx);

	// Entity fields start here.

	EntityType m_Type;

	int m_iClassID;
	float m_flDistance;

	bool m_bCritProjectile;
	bool m_bGrenadeProjectile;

	bool m_bAnyHitboxVisible;
	bool m_bVisCheckComplete;
	bool IsVisible();

	Vector m_vecOrigin;

	int  m_iTeam;
	bool m_bAlivePlayer;
	bool m_bEnemy;
	int m_iMaxHealth;
	int m_iHealth;

	unsigned long m_lSeenTicks;
	unsigned long m_lLastSeen;

	player_info_s* m_pPlayerInfo;
	matrix3x4_t* m_Bones;
	bool m_bBonesSetup;
	matrix3x4_t* GetBones();

	// Players, Buildings, Stickies


	// Entity fields end here.

	EntityHitboxCache* m_pHitboxCache;
	int m_IDX;
	IClientEntity* InternalEntity();
	IClientEntity* m_pEntity;
	ESPStringCompound* m_Strings;
	int m_nESPStrings;
	Vector m_ESPOrigin;
};

class EntityCache {
public:
	EntityCache();
	~EntityCache();

	void Update();
	void Invalidate();
	CachedEntity* GetEntity(int idx);

	CachedEntity* m_pArray;
	int m_nMax;
};

enum ECPNodes {
	ECPN_UPDATE,
	ECPN_HITBOX_UPDATE,
	ECPN_VISCHECK,
	ECPN_TOTAL
};

class EntityCacheProfiling {
public:
	EntityCacheProfiling();
	~EntityCacheProfiling();
	void Reset();
	long CurrentTime();
	void StoreData(int id, long time);
	void DoLog();
	long  m_nLastReset;
	long  m_nLastLog;
	long* m_DataAvg;
	long  m_DataAvgAmount;
	long* m_DataMax;
};

extern EntityCacheProfiling gECP;

extern EntityCache gEntityCache;

#if ENTITY_CACHE_PROFILER == true
	class CatVar;
	extern CatVar* g_vEntityCacheProfiling;
#endif

#endif /* ENTITYCACHE_H_ */
