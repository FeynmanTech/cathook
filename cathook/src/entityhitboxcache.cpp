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
	m_VisCheck = new bool[CACHE_MAX_HITBOXES];
	m_VisCheckValidationFlags = new bool[CACHE_MAX_HITBOXES];
	InvalidateCache();
	m_pParentEntity = parent;
	m_bModelSet = false;
	m_nNumHitboxes = 0;
}

int EntityHitboxCache::GetNumHitboxes() {
	if (!m_bInit) Init();
	if (!m_bSuccess) return 0;
	return m_nNumHitboxes;
}

EntityHitboxCache::~EntityHitboxCache() {
	delete [] m_CacheInternal;
	delete [] m_CacheValidationFlags;
	delete [] m_VisCheck;
	delete [] m_VisCheckValidationFlags;
}

void EntityHitboxCache::InvalidateCache() {
	for (int i = 0; i < CACHE_MAX_HITBOXES; i++) {
		m_CacheValidationFlags[i] = false;
		m_VisCheckValidationFlags[i] = false;
	}
	m_bInit = false;
	m_bSuccess = false;
}

void EntityHitboxCache::Update() {
	SAFE_CALL(InvalidateCache());
	if (CE_BAD(m_pParentEntity)) return;
}

void EntityHitboxCache::Init() {
	m_bInit = true;
	model_t* model = 0;
	if (CE_BAD(m_pParentEntity)) return;
	SAFE_CALL(model = (model_t*)RAW_ENT(m_pParentEntity)->GetModel());
	if (!model) return;
	if (!m_bModelSet || model != m_pLastModel) {
		studiohdr_t* shdr = interfaces::model->GetStudiomodel(model);
		if (!shdr) return;
		mstudiohitboxset_t* set = shdr->pHitboxSet(CE_INT(m_pParentEntity, netvar.iHitboxSet));
		if (!dynamic_cast<mstudiohitboxset_t*>(set)) return;
		m_pLastModel = model;
		m_pHitboxSet = set;
		m_nNumHitboxes = 0;
		if (set) {
			SAFE_CALL(m_nNumHitboxes = set->numhitboxes);
		}
		if (m_nNumHitboxes > CACHE_MAX_HITBOXES) m_nNumHitboxes = CACHE_MAX_HITBOXES;
		m_bModelSet = true;
	}
	m_bSuccess = true;
}

bool EntityHitboxCache::VisibilityCheck(int id) {
	if (!m_bInit) Init();
	if (id < 0 || id >= m_nNumHitboxes) return 0;
	if (!m_bSuccess) return 0;
	if (m_VisCheckValidationFlags[id]) return m_VisCheck[id];
	// TODO corners
	CachedHitbox* hitbox = GetHitbox(id);
	if (!hitbox) return 0;
	SAFE_CALL(m_VisCheck[id] = (IsEntityVectorVisible(m_pParentEntity, hitbox->center)));
	m_VisCheckValidationFlags[id] = true;
	return m_VisCheck[id];
}

CachedHitbox* EntityHitboxCache::GetHitbox(int id) {
	if (id < 0 || id >= m_nNumHitboxes) return 0;
	if (!m_bInit) Init();
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
