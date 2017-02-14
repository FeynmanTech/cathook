/*
 * localplayer.h
 *
 *  Created on: Oct 15, 2016
 *      Author: nullifiedcat
 */

#ifndef LOCALPLAYER_H_
#define LOCALPLAYER_H_

#include "fixsdk.h"
#include <mathlib/vector.h>

class CachedEntity;

class LocalPlayer {
public:
	void Update();
	bool bad;
	int flags;
	bool dead;
	bool bZoomed;
	float flZoomBegin;

	k_EWeaponmode weaponmode();
	k_EWeaponmode m_weaponmode;
	Vector v_ViewOffset;
	Vector v_Eye;
	int entity_idx;
	CachedEntity* entity;
	CachedEntity* weapon();
	Vector v_OrigViewangles;
	Vector v_SilentAngles;
	bool bUseSilentAngles;
	bool bAttackLastTick;
};

#define LOCAL_E g_pLocalPlayer->entity
#define LOCAL_W g_pLocalPlayer->weapon()

extern LocalPlayer g_LocalPlayer;

#endif /* LOCALPLAYER_H_ */
