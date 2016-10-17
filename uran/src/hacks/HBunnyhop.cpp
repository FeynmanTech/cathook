/*
 * HBunnyhop.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "HBunnyhop.h"

#include "../usercmd.h"
#include "../interfaces.h"
#include "../entity.h"
#include "../logging.h"
#include "../helpers.h"

#include "../fixsdk.h"
#include "../sdk/in_buttons.h"
#include <cdll_int.h>
#include <icliententitylist.h>
#include <icliententity.h>
#include <tier1/convar.h>

void HBunnyhop::Create() {
	this->v_bEnabled = CreateConVar("u_bhop_enabled", "1", "Enable/Disable BunnyHop");
}

bool bDoubleJumpFix = false;

bool HBunnyhop::CreateMove(void* thisptr, float sampling, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	int player = interfaces::engineClient->GetLocalPlayer();
	IClientEntity* entity = interfaces::entityList->GetClientEntity(player);
	int cond3 = GetEntityValue<int>(entity, entityvars.iCond3);
	if (cond3 & cond_ex3::grappling) return true;
	int flags = GetEntityValue<int>(entity, entityvars.iFlags);
	bool ground = (flags & (1 << 0));
	bool jump = (cmd->buttons & IN_JUMP);

	if (jump && !ground) {
		if (!bDoubleJumpFix) {
			cmd->buttons = cmd->buttons &~ IN_JUMP;
		}
		bDoubleJumpFix = false;
	}
	if (!jump && !ground) {
		bDoubleJumpFix = true;
	}
	if (ground) {
		bDoubleJumpFix = false;
	}
	return true;
}

void HBunnyhop::Destroy() {
}

void HBunnyhop::PaintTraverse(void*, unsigned int, bool, bool) {}


