/*
 * Misc.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: nullifiedcat
 */

#include "Misc.h"

#include "../hack.h"
#include "../helpers.h"
#include "../drawing.h"
#include "../localplayer.h"
#include "../interfaces.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <client_class.h>
#include <Color.h>
#include <cdll_int.h>

void CC_SayLines(const CCommand& args) {
	char cmd[256];
	sprintf(cmd, "say %s", args.ArgS());
	for (int i = 0; i < strlen(cmd); i++) {
		if (cmd[i] == '^') cmd[i] = '\n';
	}
	interfaces::engineClient->ServerCmd(cmd);
}

void CC_Shutdown(const CCommand& args) {
	hack::Shutdown();
}

void Misc::Create() {
	v_bDbWeaponInfo = CreateConVar("u_misc_debug_weapon", "0", "Debug info: Weapon");
	c_SayLine = CreateConCommand("u_say_lines", CC_SayLines, "Uses ^ as a newline character");
	c_Shutdown = CreateConCommand("u_shutdown", CC_Shutdown, "Stops the hack");

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
