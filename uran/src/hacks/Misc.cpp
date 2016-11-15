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
#include "../logging.h"
#include "../interfaces.h"
#include "../entity.h"
#include "../usercmd.h"
#include "../sdk/in_buttons.h"

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

void CC_AddFriend(const CCommand& args) {
	if (args.ArgC() < 1) return;
	if (!atoi(args[1])) return;
	logging::Info("Adding friend %s", args.Arg(1));
	friends[n_friends++] = atoi(args[1]);
}

void CC_AddRage(const CCommand& args) {
	if (args.ArgC() < 1) return;
	if (!atoi(args[1])) return;
	logging::Info("Adding rage %s", args.Arg(1));
	rage[n_rage++] = atoi(args[1]);
}


void Misc::Create() {
	v_bDbWeaponInfo = CreateConVar("u_misc_debug_weapon", "0", "Debug info: Weapon");
	v_bSemiAuto = CreateConVar("u_misc_semiauto", "0", "Force semi-auto");
	c_SayLine = CreateConCommand("u_say_lines", CC_SayLines, "Uses ^ as a newline character");
	c_Shutdown = CreateConCommand("u_shutdown", CC_Shutdown, "Stops the hack");
	c_AddFriend = CreateConCommand("u_addfriend", CC_AddFriend, "Adds a friend");
	c_AddRage = CreateConCommand("u_addrage", CC_AddRage, "Adds player to rage list");
}

int sa_switch = 0;

bool Misc::CreateMove(void*, float, CUserCmd* cmd) {
	if (v_bSemiAuto->GetBool()) {
		if (cmd->buttons & IN_ATTACK) {
			if (sa_switch > 5) {
				cmd->buttons = cmd->buttons &~ IN_ATTACK;
			}
			sa_switch++;
		} else {
			sa_switch = 0;
		}
	}
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
			draw::DrawString(10, y, draw::white, false, "flNextAttack: %f", GetEntityValue<float>(g_pLocalPlayer->entity, eoffsets.flNextAttack));
			y += 14;
			draw::DrawString(10, y, draw::white, false, "nTickBase: %i", GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.nTickBase));
			y += 14;
		}
	}
}
