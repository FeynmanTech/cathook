/*
 * AntiDisguise.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: nullifiedcat
 */

#include "AntiDisguise.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(AntiDisguise);

AntiDisguise::AntiDisguise() {
	v_bEnabled = new CatVar_DEPRECATED(CV_SWITCH, "antidisguise", "0", "Remove spy disguise", 0, "Remove disguise from all spies");
}

void AntiDisguise::Draw() {
	if (!v_bEnabled->GetBool()) return;
	for (int i = 0; i < 64 && i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		if (ent->m_Type == ENTITY_PLAYER) {
			if (CE_INT(ent, netvar.iClass) == tf_class::tf_spy) {
				RemoveCondition(ent, TFCond_Disguised);
			}
		}
	}
}
