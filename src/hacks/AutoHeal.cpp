/*
 * AutoHeal.cpp
 *
 *  Created on: Dec 3, 2016
 *      Author: nullifiedcat
 */

#include "AutoHeal.h"

#include "../common.h"
#include "../sdk.h"

namespace hacks { namespace tf { namespace autoheal {

CatVar enabled(CV_SWITCH, "autoheal_enabled", "0", "AutoHeal", NULL, "Automatically heals nearby teammates");

int oldTarget = 0;
int newTarget = 0;
int bestHealingPriority = 0;

bool CanHeal(CachedEntity& entity) {
	if (entity.Type() != ENTITY_PLAYER) return false;
	if (g_IEngine->GetLocalPlayer() == entity.m_IDX) return false;
	if (entity.var<bool>(netvar.iLifeState)) return false;
	if (entity.Enemy()) return false;
	if (entity.Distance() > 420) return false;
	// TODO visible any hitbox
	if (!IsEntityVisible(&entity, 7)) return false;
	if (IsPlayerInvisible(&entity)) return false;
	return true;
}

int HealingPriority(CachedEntity& entity) {
	int priority = 0;
	int health = entity.Health();
	int maxhealth = entity.MaxHealth();
	int maxbuffedhealth = maxhealth * 1.5;
	int maxoverheal = maxbuffedhealth - maxhealth;
	int overheal = maxoverheal - (maxbuffedhealth - health);
	float overhealp = ((float)overheal / (float)maxoverheal);
	float healthp = ((float)health / (float)maxhealth);
	/*if (GetRelation(ent) == relation::FRIEND) {
		priority += 70 * (1 - healthp);
		priority += 15 * (1 - overhealp);
	} else*/ {
		priority += 50 * (1 - healthp);
		priority += 10 * (1 - overhealp);
	}
	return priority;
}

void Reset() {
	newTarget = 0;
	bestHealingPriority = 0;
}

void ProcessEntity(CUserCmd*, CachedEntity& entity) {
	if (!enabled) return;
	if (CanHeal(entity)) {
		int prio = HealingPriority(entity);
		if (newTarget == nullptr || prio > bestHealingPriority) {
			newTarget = entity.m_IDX;
			bestHealingPriority = prio;
		}
	}
}

void DoHealing(CUserCmd* cmd) {
	if (!enabled) return;
	if (g_LocalPlayer.weaponmode() != weapon_medigun) return;
	if (!newTarget) return;
	CachedEntity& target = gEntityCache.Entity(newTarget);
	try {
		AimAt(g_LocalPlayer.v_Eye, target.hitboxes.GetHitbox(7).center, cmd);
		g_LocalPlayer.bUseSilentAngles = true;
		if (newTarget != oldTarget || g_pGlobals->tickcount % 100) cmd->buttons |= IN_ATTACK;
		oldTarget = newTarget;
	} catch (...) {}
}

}}}
