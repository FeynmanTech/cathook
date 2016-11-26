/*
 * AntiDisguise.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: nullifiedcat
 */

#include "AntiDisguise.h"

#include "../helpers.h"
#include "../entity.h"
#include "../interfaces.h"
#include "../fixsdk.h"
#include <icliententitylist.h>
#include <client_class.h>
#include <icliententity.h>

void AntiDisguise::Create() {
	v_bEnabled = CreateConVar("u_antidisguise", "1", "Disables spy disguise");
}

void AntiDisguise::PaintTraverse(void*, unsigned int, bool, bool) {
	if (!v_bEnabled->GetBool()) return;
	for (int i = 0; i < 64 && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
		if (!ent) continue;
		if (ent->GetClientClass()->m_ClassID == ClassID::CTFPlayer) {
			if (GetEntityValue<int>(ent, eoffsets.iClass) == tf_class::tf_spy) {
				SetEntityValue<int>(ent, eoffsets.iCond, GetEntityValue<int>(ent, eoffsets.iCond) &~ cond::disguised);
			}
		}
	}
}

bool AntiDisguise::CreateMove(void*, float, CUserCmd*) {return true;}
void AntiDisguise::Destroy() {}

AntiDisguise* g_phAntiDisguise = 0;
