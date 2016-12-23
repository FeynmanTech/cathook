/*
 * entityhitboxcache.cpp
 *
 *  Created on: Dec 23, 2016
 *      Author: nullifiedcat
 */

#include "common.h"

EntityHitboxCache::EntityHitboxCache(CachedEntity* parent) {
	m_CacheInternal = new CachedHitbox[CACHE_MAX_HITBOXES];
	m_CacheValidationFlags = new bool[CACHE_MAX_HITBOXES];
	InvalidateCache();
	m_pParentEntity = parent;
	m_bModelSet = false;
}

EntityHitboxCache::~EntityHitboxCache() {
	delete [] m_CacheInternal;
	delete [] m_CacheValidationFlags;
}

void EntityHitboxCache::InvalidateCache() {
	for (int i = 0; i < CACHE_MAX_HITBOXES; i++) {
		m_CacheValidationFlags[i] = false;
	}
}

void EntityHitboxCache::Update() {
	SAFE_CALL(InvalidateCache());
	if (CE_BAD(m_pParentEntity)) return;
	model_t* model;
	model = (model_t*)RAW_ENT(m_pParentEntity)->GetModel();
	if (!model) return;
	if (!m_bModelSet || model != m_pLastModel) {
		studiohdr_t* shdr = interfaces::model->GetStudiomodel(model);
		if (!shdr) return;
		mstudiohitboxset_t* set = shdr->pHitboxSet(CE_INT(m_pParentEntity, netvar.iHitboxSet));
		if (!set) return;
		m_pLastModel = model;
		m_pHitboxSet = set;
		m_nNumHitboxes = set->numhitboxes;
		if (m_nNumHitboxes > CACHE_MAX_HITBOXES) m_nNumHitboxes = CACHE_MAX_HITBOXES;
		m_bSuccess = true;
		m_bModelSet = true;
	}
}

CachedHitbox* EntityHitboxCache::GetHitbox(int id) {
	if (id < 0 || id >= m_nNumHitboxes) return 0;
	if (!m_bSuccess) return 0;
	if (!m_CacheValidationFlags[id]) {
		mstudiobbox_t* box = m_pHitboxSet->pHitbox(id);
		if (!box) return 0;
		if (box->bone < 0 || box->bone >= MAXSTUDIOBONES) return 0;
		VectorTransform(box->bbmin, m_pParentEntity->GetBones()[box->bone], m_CacheInternal[id].min);
		VectorTransform(box->bbmax, m_pParentEntity->GetBones()[box->bone], m_CacheInternal[id].max);
		m_CacheInternal[id].bbox = box;
		m_CacheInternal[id].center = (m_CacheInternal[id].min + m_CacheInternal[id].max) / 2;
		m_CacheValidationFlags[id] = true;
	}
	return &m_CacheInternal[id];
}
