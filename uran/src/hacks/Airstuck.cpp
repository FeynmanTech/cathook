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

Airstuck::Airstuck() {
	v_bStuck = CreateConVar("u_airstuck", "0", "Toggle airstuck");
}

const char* Airstuck::GetName() {
	return "AIRSTUCK";
}

bool Airstuck::CreateMove(void*, float, CUserCmd* cmd) {
	// TODO Airstuck.
	return true;
	/*if (v_bStuck->GetBool()) {
		if (cmd->buttons & (IN_ATTACK | IN_ATTACK2)) {
			return true;
		}

	}
	return true;*/
}

void Airstuck::PaintTraverse(void*, unsigned int, bool, bool) {};

Airstuck* g_phAirstuck = 0;
