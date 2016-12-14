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
	v_bEnabled = CreateConVar(CON_PREFIX "antidisguise", "0", "Disables spy disguise");
}

void AntiDisguise::PaintTraverse(void*, unsigned int, bool, bool) {
	if (!v_bEnabled->GetBool()) return;
	for (int i = 0; i < 64 && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
		if (!ent) continue;
		if (ent->GetClientClass()->m_ClassID == ClassID::CTFPlayer) {
			if (GetEntityValue<int>(ent, netvar.iClass) == tf_class::tf_spy) {
				SetEntityValue<int>(ent, netvar.iCond, GetEntityValue<int>(ent, netvar.iCond) &~ cond::disguised);
			}
		}
	}
}

bool AntiDisguise::CreateMove(void*, float, CUserCmd*) {return true;}
