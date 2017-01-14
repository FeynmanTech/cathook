/*
 * SpyAlert.cpp
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#include "SpyAlert.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(SpyAlert);

const char* SpyAlert::GetName() { return "SPY ALERT"; }

SpyAlert::SpyAlert() {
	this->v_bEnabled = CREATE_CV(CV_SWITCH, "spyalert_enabled", "0", "Enable");
	this->v_flWarningDistance = CREATE_CV(CV_FLOAT, "spyalert_warning", "500.0", "Warning distance");
	this->v_flBackstabDistance = CREATE_CV(CV_FLOAT, "spyalert_backstab", "200.0", "Backstab distance");
}

bool SpyAlert::CreateMove(void*, float, CUserCmd* cmd) {
	return true;
}

void SpyAlert::PaintTraverse(void*, unsigned int, bool, bool) {
	if (CE_BAD(g_pLocalPlayer->entity))
	if (g_pLocalPlayer->life_state) return;
	if (!v_bEnabled->GetBool()) return;
	for (int i = 0; i < HIGHEST_ENTITY && i < 64; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent)) continue;
		if (CE_BYTE(ent, netvar.iLifeState)) continue;
		if (CE_INT(ent, netvar.iClass) != tf_class::tf_spy) continue;
		if (CE_INT(ent, netvar.iTeamNum) == g_pLocalPlayer->team) continue;
		float distance = ent->m_flDistance;
		if (distance < this->v_flBackstabDistance->GetFloat()) {
			AddCenterString(colors::yellow, colors::red, "BACKSTAB WARNING! %im", (int)(distance / 64 * 1.22f));
		} else if (distance < this->v_flWarningDistance->GetFloat()) {
			AddCenterString(colors::yellow, colors::black, "Incoming spy! %im", (int)(distance / 64 * 1.22f));
		}
	}
}
