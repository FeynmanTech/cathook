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
	this->v_bEnabled = CREATE_CV(CV_SWITCH, "bhop_enabled", "0", "Enable");
	this->v_bAutoJump = CREATE_CV(CV_SWITCH, "bhop_autojump", "0", "AutoJump");
	this->v_iAutoJumpSpeed = CREATE_CV(CV_INT, "bhop_autojump_speed", "300", "AutoJump speed");
}

bool bDoubleJumpFix = false;

bool Bunnyhop::CreateMove(void* thisptr, float sampling, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	int player = interfaces::engineClient->GetLocalPlayer();
	IClientEntity* entity = ENTITY(player);
	int cond3 = NET_INT(entity, netvar.iCond3);
	if (cond3 & cond_ex3::grappling) return true;
	int flags = NET_INT(entity, netvar.iFlags);

	if (v_bAutoJump->GetBool()) {
		Vector vel = NET_VECTOR(g_pLocalPlayer->entity, netvar.vVelocity);
		if (sqrt((vel.x * vel.x + vel.y * vel.y)) > v_iAutoJumpSpeed->GetInt()) {
			cmd->buttons |= IN_JUMP;
		}
	}

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
