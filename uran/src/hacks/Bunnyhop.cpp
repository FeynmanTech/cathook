/*
 * HBunnyhop.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "Bunnyhop.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(Bunnyhop);

const char* Bunnyhop::GetName() {
	return "BUNNYHOP";
}

Bunnyhop::Bunnyhop() {
	this->v_bEnabled = CreateConVar("u_bhop_enabled", "1", "Enable/Disable BunnyHop");
}

bool bDoubleJumpFix = false;

bool Bunnyhop::CreateMove(void* thisptr, float sampling, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	int player = interfaces::engineClient->GetLocalPlayer();
	IClientEntity* entity = interfaces::entityList->GetClientEntity(player);
	int cond3 = GetEntityValue<int>(entity, eoffsets.iCond3);
	if (cond3 & cond_ex3::grappling) return true;
	int flags = GetEntityValue<int>(entity, eoffsets.iFlags);
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

void Bunnyhop::PaintTraverse(void*, unsigned int, bool, bool) {}
