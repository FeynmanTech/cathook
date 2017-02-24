/*
 * AutoStrafe.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#include "AutoStrafe.h"

#include "../common.h"
#include "../sdk.h"

/*DEFINE_HACK_SINGLETON(AutoStrafe);

AutoStrafe::AutoStrafe() {
	v_bEnabled = new CatVar(CV_SWITCH, "autostrafe", "0", "Enable AutoStrafe", NULL, "AutoStrafe switch. Doesn't work.");
}


void AutoStrafe::ProcessUserCmd(CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return;
	bool sw = false;
	if (CE_GOOD(g_LocalPlayer->entity) && !g_LocalPlayer->life_state) {
		// TODO FL_ONGROUND
		if (CE_INT(g_LocalPlayer->entity, netvar.iFlags) & (1 << 0)) return;
		cmd->sidemove += (sw) ? -30.0f : 30.0f;
		cmd->viewangles.y += sw ? -15.0f : 15.0f;
		g_LocalPlayer->bUseSilentAngles = true;
		sw = !sw;
	}
	return;
}*/
