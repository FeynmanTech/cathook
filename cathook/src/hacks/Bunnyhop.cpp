/*
 * HBunnyhop.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "Bunnyhop.h"

#include "../common.h"
#include "../sdk.h"

namespace hacks { namespace shared { namespace bunnyhop {

CatVar enabled(CV_SWITCH, "bhop_enabled", "0", "Bunnyhop", NULL, "Enable Bunnyhop");
CatVar perfect_jumps(CV_SWITCH, "bhop_pjumps", "0", "Perfect Jump Limit", NULL, "Bunny Hop perfect jump limit. 0 = Disabled");
CatVar imperfect(CV_INT, "bhop_imperfect", "0", "Miss bhop jumps randomly", NULL, "To avoid auto bans");
CatVar autojump(CV_SWITCH, "bhop_autojump", "0", "AutoJump", NULL, "Autojump if you reach certain speed");
CatVar autojump_speed(CV_INT, "bhop_autojump_speed", "300", "AutoJump speed", NULL, "Minimum autojump speed");

bool jumping = false;

bool disabled = false;
int jump_ticks = 0;
int fly_ticks = 0;
int pjumps = 0;

void ProcessUserCmd(CUserCmd* cmd) {
	jumping = false;
	if (!enabled) return;

	int flags = g_LocalPlayer.entity->var<int>(netvar.iFlags);
	bool ground = (flags & FL_ONGROUND);

	if (HasCondition(g_LocalPlayer.entity, TFCond_GrapplingHook)) return;

	if (autojump) {
		Vector vel = g_LocalPlayer.entity->var<Vector>(netvar.vVelocity);
		if ((vel.x * vel.x + vel.y * vel.y) > SQR((int)autojump_speed)) {
			cmd->buttons |= IN_JUMP;
		}
	}

	if (!ground) {
		if (imperfect) {
			static bool ip_switch = false;
			if (g_LocalPlayer.entity->Class() != tf_scout) {
				if (rand() % 20) ip_switch = !ip_switch;
				if (ip_switch) cmd->buttons &= ~IN_JUMP;
				else cmd->buttons |= IN_JUMP;
			}
		}
	}

	bool jump = (cmd->buttons & IN_JUMP);

	if (ground) {
		fly_ticks = 0;
	} else {
		fly_ticks++;
	}

	if (jump) disabled = false;
	if (disabled) return;

	if (ground && jump) {
		if (perfect_jumps && pjumps > perfect_jumps) {
			pjumps = 0;
			disabled = true;
		}
		jumping = true;
		if (perfect_jumps) pjumps++;
	}

	if (!ground && jump) {
		if (jump_ticks >= 20) cmd->buttons = cmd->buttons &~ IN_JUMP;
	}
	if (!jump) jump_ticks = 0;
	return;
}

}}}
