/*
 * localplayer.cpp
 *
 *  Created on: Oct 15, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"

k_EWeaponmode LocalPlayer::weaponmode() {
	if (m_weaponmode == weapon_invalid) {
		m_weaponmode = GetWeaponMode(entity);
	}
	return m_weaponmode;
}

void LocalPlayer::Update() {
	bad = true;
	entity_idx = g_IEngine->GetLocalPlayer();
	entity = gEntityCache.Entity(entity_idx);
	if (entity->bad) {
		return;
	}
	dead = entity->var<unsigned char>(netvar.iLifeState);
	v_ViewOffset = entity->var<Vector>(netvar.vViewOffset);
	v_Eye = entity->Origin() + v_ViewOffset;
	this->bUseSilentAngles = false;
	bZoomed = entity->var<int>(netvar.iFOV) == 20;
	if (bZoomed) {
		if (flZoomBegin == 0.0f) flZoomBegin = g_pGlobals->curtime;
	} else {
		flZoomBegin = 0.0f;
	}
}

// When calling any functions always assume that local player is not bad.
CachedEntity* LocalPlayer::weapon() {
	int handle = entity->var<int>(netvar.hActiveWeapon);
	int eid = handle & 0xFFF;
	if (eid < 0 || eid > gEntityCache.max) return 0;
	return gEntityCache.Entity(eid);
}

LocalPlayer g_LocalPlayer;
