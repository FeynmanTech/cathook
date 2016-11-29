/*
 * targethelper.cpp
 *
 *  Created on: Oct 16, 2016
 *      Author: nullifiedcat
 */

#include "enums.h"
#include "entity.h"
#include "localplayer.h"
#include "helpers.h"

#include "fixsdk.h"
#include <icliententity.h>
#include <client_class.h>

/*
 * Targeting priorities:
 * passive bullet vacc medic
 * zoomed snipers ALWAYS
 * medics
 * snipers
 * spies
 */

/* Assuming given entity is a valid target range 0 to 100 */
int GetScoreForEntity(IClientEntity* entity) {
	if (!entity) return 0;
	// TODO
	if (IsBuilding(entity)) {
		switch (entity->GetClientClass()->m_ClassID) {
		case ClassID::CObjectSentrygun:
			float distance = (g_pLocalPlayer->v_Origin - entity->GetAbsOrigin()).Length();
			// TODO
			int total = 1;
			if (distance != 0) {
				int distance_factor = (4096 / distance) * 4;
				total += distance_factor;
			}
			return 15;
		}
		return 0;
	}
	int clazz = GetEntityValue<int>(entity, eoffsets.iClass);
	int health = GetEntityValue<int>(entity, eoffsets.iHealth);
	float distance = (g_pLocalPlayer->v_Origin - entity->GetAbsOrigin()).Length();
	bool zoomed = (GetEntityValue<int>(entity, eoffsets.iCond) & cond::zoomed);
	int condx = (GetEntityValue<int>(entity, eoffsets.iCond1));
	int condx2 = (GetEntityValue<int>(entity, eoffsets.iCond2));
	bool pbullet = (condx & cond_ex::vacc_pbullet);
	bool special = false;
	bool kritz = IsPlayerCritBoosted(entity);
	int total = 0;
	switch(clazz) {
	case tf_sniper:
		total += 25;
		if (zoomed) {
			total += 50;
		}
		special = true;
	break;
	case tf_medic:
		total += 50;
		if (pbullet) return 100;
	break;
	case tf_spy:
		total += 20;
		if (distance < 400) total += 60;
		special = true;
	break;
	case tf_soldier:
		if (condx2 & cond_ex2::rocket_jumping) total += 30;
	break;
	}
	if (!special) {
		if (pbullet) {
			total += 50;
		}
		if (kritz) {
			total += 99;
		}
		if (distance != 0) {
			int distance_factor = (4096 / distance) * 4;
			total += distance_factor;
			if (health != 0) {
				int health_factor = (450 / health) * 4;
				if (health_factor > 30) health_factor = 30;
				total += health_factor;
			}
		}
	}
	if (total > 99) total = 99;
	if (GetRelation(entity) == relation::RAGE) total = 999;
	if (IsSentryBuster(entity)) total = 0;
	//if (clazz == tf_medic) total = 999; // TODO only for mvm
	return total;
}
