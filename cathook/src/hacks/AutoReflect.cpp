/*
 * AutoReflect.cpp
 *
 *  Created on: Nov 18, 2016
 *      Author: nullifiedcat
 */

#include "AutoReflect.h"

#include "../common.h"
#include "../sdk.h"

namespace hacks { namespace tf { namespace autoreflect {

CatVar enabled(CV_SWITCH, "reflect_enabled", "0", "AutoReflect", NULL, "Automatically aim and reflect incoming projectiles");
CatVar distance(CV_FLOAT, "reflect_distance", "200", "Reflect distance", NULL, "Maximum distance between you and projectile");
CatVar idle_only(CV_SWITCH, "reflect_idle_only", "0", "Idle only", NULL, "Don't autoreflect if you are attacking");
CatVar reflect_stickies(CV_SWITCH, "reflect_stickies", "0", "Reflect Stickies", NULL, "Enable this if you want to reflect stickybombs");

float closest_distance = -1.0f;
CachedEntity* closest_entity = nullptr;

void Reset() {
	closest_distance = -1.0f;
	closest_entity = nullptr;
}

void DoReflects(CUserCmd* cmd) {
	if (!closest_entity) return;
	if (closest_distance > distance) return;
	Vector tr = (closest_entity->Origin() - g_LocalPlayer.v_Eye);
	Vector angles;
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
	g_LocalPlayer.bUseSilentAngles = true;
	cmd->buttons |= IN_ATTACK2;
}

void ProcessEntity(CUserCmd* cmd, CachedEntity& entity) {
	if (!ShouldReflect(cmd)) return;
	if (!ShouldReflectEntity(entity)) return;
	float distance = entity.Distance();
	if (closest_distance == -1.0f || distance < closest_distance) {
		closest_distance = distance;
		closest_entity = entity;
	}
}

bool ShouldReflectEntity(CachedEntity& entity) {
	if (entity.Type() != ENTITY_PROJECTILE) return false;
	if (!entity.Enemy()) return false; // TODO teammate reflect
	if (!reflect_stickies && entity.Projectile() == PROJ_STICKY) return false;
	// TODO iDeflected
	return true;
}

bool ShouldReflect(CUserCmd* cmd) {
	if (!enabled) return false;
	if (g_LocalPlayer.weapon()->clazz != g_pClassID->CTFFlameThrower) return false;
	if (idle_only && (cmd->buttons & IN_ATTACK)) return false;
	return true;
}

}}}
