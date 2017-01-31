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
	v_bEnabled = new CatVar(CV_SWITCH, "autostrafe", "0", "Enable AutoStrafe", NULL, "AutoStrafe switch. Doesn't work.");
}


bool AutoStrafe::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;
	bool sw = false;
	if (CE_GOOD(g_pLocalPlayer->entity) && !g_pLocalPlayer->life_state) {
		// TODO FL_ONGROUND
		if (CE_INT(g_pLocalPlayer->entity, netvar.iFlags) & (1 << 0)) return true;
		cmd->sidemove += (sw) ? -30.0f : 30.0f;
		cmd->viewangles.y += sw ? -15.0f : 15.0f;
		g_pLocalPlayer->bUseSilentAngles = true;
		sw = !sw;
	}
	return true;
}

void AutoStrafe::PaintTraverse(void*, unsigned int, bool, bool) {}

void AutoStrafe::LevelInit(const char*) {}
void AutoStrafe::LevelShutdown() {}
