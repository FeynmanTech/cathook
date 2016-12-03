/*
 * Airstuck.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#include "Airstuck.h"

#include "../helpers.h"
#include "../sdk/in_buttons.h"
#include "../usercmd.h"
#include "../logging.h"
#include "../interfaces.h"
#include "../fixsdk.h"
#include <icvar.h>

DEFINE_HACK_SINGLETON(Airstuck);

Airstuck::Airstuck() {
	v_bStuck = CreateConVar("u_airstuck", "0", "Toggle airstuck");
}

const char* Airstuck::GetName() {
	return "AIRSTUCK";
}

bool Airstuck::CreateMove(void*, float, CUserCmd* cmd) {
	// TODO Airstuck.
	interfaces::cvar->FindVar("net_fakeloss")->SetValue(0);
	interfaces::cvar->FindVar("host_timescale")->SetValue(1.0f);
	if (v_bStuck->GetBool()) {
		if (cmd->buttons & (IN_ATTACK | IN_ATTACK2)) {
			return true;
		}
		//cmd->tick_count = 0xFFFF;
		//interfaces::cvar->FindVar("net_fakeloss")->SetValue(99);
		interfaces::cvar->FindVar("host_timescale")->SetValue(0.001f);
	}
	return true;
}

void Airstuck::PaintTraverse(void*, unsigned int, bool, bool) {};
