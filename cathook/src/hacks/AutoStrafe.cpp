/*
 * AutoStrafe.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#include "AutoStrafe.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(AutoStrafe);

const char* AutoStrafe::GetName() {
	return "AUTOSTRAFE";
}

AutoStrafe::AutoStrafe() {
	v_bEnabled = CREATE_CV(CV_SWITCH, "autostrafe", "0", "Enable AutoStrafe");
}

bool AutoStrafe::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;
	if (CE_GOOD(g_pLocalPlayer->entity) && !g_pLocalPlayer->life_state) {
		// TODO FL_ONGROUND
		if (CE_INT(g_pLocalPlayer->entity, netvar.iFlags) & (1 << 0)) return true;
		if (abs(cmd->mousedx) > 1) {
			cmd->sidemove = (cmd->mousedx) < 0.0f ? -450.0f : 450.0f;
		}
	}
	return true;
}

void AutoStrafe::PaintTraverse(void*, unsigned int, bool, bool) {}

void AutoStrafe::LevelInit(const char*) {}
void AutoStrafe::LevelShutdown() {}
