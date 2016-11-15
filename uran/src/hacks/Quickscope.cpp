/*
 * Quickscope.cpp
 *
 *  Created on: Nov 12, 2016
 *      Author: nullifiedcat
 */

#include "Quickscope.h"

#include "../localplayer.h"
#include "../helpers.h"
#include "../entity.h"
#include "../usercmd.h"
#include "../sdk/in_buttons.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <client_class.h>

void Quickscope::Create() {
	v_bEnabled = CreateConVar("u_quickscope", "1", "Quickscope mode");
}

bool Quickscope::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;
	if (g_pLocalPlayer->clazz == tf_class::tf_sniper) {
		if (!g_pLocalPlayer->weapon) return false;
		switch (g_pLocalPlayer->weapon->GetClientClass()->m_ClassID) {
		case ClassID::CTFSniperRifle:
			break;
		default:
			return true;
		}
		float bdmg = GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flChargedDamage);
		if (bdmg <= 15.0f && (g_pLocalPlayer->cond_0 & cond::zoomed)) {
			cmd->buttons = cmd->buttons &~ IN_ATTACK;
		}
	}
	return true;
}

void Quickscope::Destroy() {}
void Quickscope::PaintTraverse(void*, unsigned int, bool, bool) { }
