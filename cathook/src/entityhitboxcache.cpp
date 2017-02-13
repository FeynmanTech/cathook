/*
 * entityhitboxcache.cpp
 *
 *  Created on: Dec 23, 2016
 *      Author: nullifiedcat
 */

#include "common.h"

EntityHitboxCache::EntityHitboxCache(CachedEntity& entity) : parent(entity) {
	MarkDirty();
	m_bModelSet = false;
}

int EntityHitboxCache::HitboxCount() {
	if (dirty) Init();
	if (!success) return 0;
	return m_nNumHitboxes;
}

void EntityHitboxCache::MarkDirty() {
	dirty = true;
}

void EntityHitboxCache::Init() {
	success = false;
	dirty = false;
	model_t* model = 0;
	if (parent.bad) return;
	SAFE_CALL(model = (model_t*)parent.entptr->GetModel());
	if (!model) return;
	if (!m_bModelSet || model != m_pLastModel) {
		studiohdr_t* shdr = g_IModelInfo->GetStudiomodel(model);
		if (!shdr) return;
		mstudiohitboxset_t* set = shdr->pHitboxSet(parent.var<int>(netvar.iHitboxSet));
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
	for (int i = 0; i < m_nNumHitboxes; i++) vdirty[i] = true;
	success = true;
}

bool EntityHitboxCache::VisibilityCheck(int id) {
	if (dirty) Init();
	if (!success) return false;
	if (id < 0 || id >= m_nNumHitboxes) return false;
	if (!vdirty[id]) return vcheck[id];
	// TODO corners
	CachedHitbox& hitbox = GetHitbox(id);
	vcheck[id] = IsEntityVectorVisible(parent, hitbox.center);
	vdirty[id] = false;
	return vcheck[id];
}

CachedHitbox& EntityHitboxCache::GetHitbox(int id) {
	if (dirty) Init();
	if (!success) throw std::exception("GetHitbox: failure!");
	if (id < 0 || id >= m_nNumHitboxes) throw std::out_of_range();
	if (cdirty[id]) {
		mstudiobbox_t* box = m_pHitboxSet->pHitbox(id);
		if (!box) return 0;
		if (box->bone < 0 || box->bone >= MAXSTUDIOBONES) return std::runtime_error("corrupt box");
		CachedHitbox& newbox = &cache[id];
		VectorTransform(box->bbmin, parent.GetBones()[box->bone], newbox.min);
		VectorTransform(box->bbmax, parent.GetBones()[box->bone], newbox.max);
		newbox.bbox = box;
		newbox.center = (newbox.min + newbox.max) / 2;
		cdirty[id] = false;
	}
	return &cache[id];
}
