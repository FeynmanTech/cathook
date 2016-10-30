/*
 * AntiAim.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: nullifiedcat
 */

#include "AntiAim.h"
#include "../helpers.h"

void AntiAim::Create() {
	this->v_bEnabled = CreateConVar("u_antiaim_enabled", "0", "Enable AntiAim");
}

bool AntiAim::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	return false;
}

void AntiAim::Destroy() {}
void AntiAim::PaintTraverse(void*, unsigned int, bool, bool) {}
