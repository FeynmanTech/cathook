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

const char* AntiDisguise::GetName() {
	return "ANTI-DISGUISE";
}

AntiDisguise::AntiDisguise() {
	v_bEnabled = CREATE_CV(CV_SWITCH, "antidisguise", "0", "Remove spy disguise");
}

void AntiDisguise::PaintTraverse(void*, unsigned int, bool, bool) {
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

bool AntiDisguise::CreateMove(void*, float, CUserCmd*) {return true;}
