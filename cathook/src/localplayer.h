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

class IClientEntity;

class LocalPlayer {
public:
	void Update();
	int team;
	int health;
	int flags;
	char life_state;
	int clazz;
	int cond_0;
	int cond_1;
	int cond_2;
	int cond_3;

	bool bZoomed;
	float flZoomBegin;

	bool bIsReloading;

	Vector v_ViewOffset;
	Vector v_Origin;
	Vector v_Eye;
	int entity_idx;
	CachedEntity* entity;
	CachedEntity* weapon;
	Vector v_OrigViewangles;
	Vector v_SilentAngles;
	bool bUseSilentAngles;
	bool bAttackLastTick;
};

extern LocalPlayer* g_pLocalPlayer;

#endif /* LOCALPLAYER_H_ */
