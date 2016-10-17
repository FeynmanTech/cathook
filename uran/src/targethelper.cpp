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
	int clazz = GetEntityValue<int>(entity, entityvars.iClass);
	int health = GetEntityValue<int>(entity, entityvars.iHealth);
	float distance = (g_pLocalPlayer->v_Origin - entity->GetAbsOrigin()).Length();
	bool zoomed = (GetEntityValue<int>(entity, entityvars.iCond) & cond::zoomed);
	int condx = (GetEntityValue<int>(entity, entityvars.iCond1));
	int condx2 = (GetEntityValue<int>(entity, entityvars.iCond2));
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
		if (kritz) {
			total += 50;
		}
		if (distance != 0) {
			int distance_factor = (4096 / distance);
			if (clazz == tf_class::tf_scout) {
				distance_factor *= 2;
			}
			total += distance_factor;
			if (health != 0) {
				int health_factor = (450 / health);
				if (health_factor > 30) health_factor = 30;
				total += health_factor;
			}
		}
	}
	if (total > 99) total = 99;
	return total;
}
