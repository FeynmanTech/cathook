/*
 * Misc.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: nullifiedcat
 */

#include "Misc.h"
#include "../helpers.h"
#include "../drawing.h"
#include "../localplayer.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <client_class.h>
#include <Color.h>

void Misc::Create() {
	v_bDbWeaponInfo = CreateConVar("u_misc_debug_weapon", "0", "Debug info: Weapon");
}

bool Misc::CreateMove(void*, float, CUserCmd* cmd) {
	return true;
}

void Misc::Destroy() {

}

void Misc::PaintTraverse(void*, unsigned int, bool, bool) {
	int y = 10;
	if (v_bDbWeaponInfo->GetBool()) {
		if (g_pLocalPlayer->weapon) {
			IClientEntity* weapon = g_pLocalPlayer->weapon;
			draw::DrawString(10, y, draw::white, false, "Weapon: %s [%i]", weapon->GetClientClass()->GetName(), weapon->GetClientClass()->m_ClassID);
			y += 14;
		}
	}
}
