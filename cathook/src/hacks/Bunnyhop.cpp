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

Bunnyhop::Bunnyhop() {
	this->v_bEnabled = new CatVar(CV_SWITCH, "bhop_enabled", "0", "Enable", NULL, "Enable Bunnyhop");
	this->v_iPerfectJumpLimit = new CatVar(CV_SWITCH, "bhop_pjumps", "0", "Perfect Jump Limit", NULL, "Bunny Hop perfect jump limit. 0 = Disabled");
	this->v_bAutoJump = new CatVar(CV_SWITCH, "bhop_autojump", "0", "AutoJump", NULL, "Autojump if you reach certain speed");
	this->v_iAutoJumpSpeed = new CatVar(CV_INT, "bhop_autojump_speed", "300", "AutoJump speed", NULL, "Minimum autojump speed");
	this->v_bImperfect = new CatVar(CV_INT, "bhop_imperfect", "0", "Miss bhop jumps randomly", NULL, "To avoid auto bans");
}

bool bDoubleJumpFix = false;
int iTicksFlying = 0;
int iTicksLastJump = 0;
int iPerfectJumps = 0;

void Bunnyhop::ProcessUserCmd(CUserCmd* cmd) {
	m_bFakeLagFix = false;
	if (!this->v_bEnabled->GetBool()) return;

	int flags = CE_INT(g_LocalPlayer->entity, netvar.iFlags);
	bool ground = (flags & (1 << 0));

	if (HasCondition(g_LocalPlayer->entity, TFCond_GrapplingHook)) return;


	if (v_bAutoJump->GetBool()) {
		Vector vel = CE_VECTOR(g_LocalPlayer->entity, netvar.vVelocity);
		if ((vel.x * vel.x + vel.y * vel.y) > SQR(v_iAutoJumpSpeed->GetInt())) {
			cmd->buttons |= IN_JUMP;
		}
	}

	if (!ground) {
		if (v_bImperfect->GetBool()) {
			if (g_LocalPlayer->clazz != tf_scout) {
				if (rand() % 3) cmd->buttons &= ~IN_JUMP;
				else cmd->buttons |= IN_JUMP;
			}
		}
	}

	bool jump = (cmd->buttons & IN_JUMP);

	if (ground) {
		iTicksFlying = 0;
	} else {
		iTicksFlying++;
	}

	if (ground && jump) {
		if (v_iPerfectJumpLimit->GetBool() && iPerfectJumps > v_iPerfectJumpLimit->GetInt()) {
			iPerfectJumps = 0;
			cmd->buttons &= ~IN_JUMP;
		}
		m_bFakeLagFix = true;
		if (v_iPerfectJumpLimit->GetBool()) iPerfectJumps++;
	}

	if (!ground && jump) {
		if (iTicksLastJump++ >= 20) cmd->buttons = cmd->buttons &~ IN_JUMP;
	}
	if (!jump) iTicksLastJump = 0;
	return;
}
