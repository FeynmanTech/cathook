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
	this->v_bEnabled = new CatVar(CV_SWITCH, "spyalert_enabled", "0", "Enable", NULL, "Master SpyAlert switch");
	this->v_flWarningDistance = new CatVar(CV_FLOAT, "spyalert_warning", "500.0", "Warning distance", NULL, "Distance where yellow warning shows");
	this->v_flBackstabDistance = new CatVar(CV_FLOAT, "spyalert_backstab", "200.0", "Backstab distance", NULL, "Distance where red warning shows");
}

bool SpyAlert::CreateMove(void*, float, CUserCmd* cmd) {
	return true;
}

void SpyAlert::PaintTraverse(void*, unsigned int, bool, bool) {
	if (CE_BAD(g_pLocalPlayer->entity)) return;
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
			AddCenterString(colors::red, "BACKSTAB WARNING! %im", (int)(distance / 64 * 1.22f));
		} else if (distance < this->v_flWarningDistance->GetFloat()) {
			AddCenterString(colors::yellow, "Incoming spy! %im", (int)(distance / 64 * 1.22f));
		}
	}
}

void SpyAlert::LevelInit(const char*) {}
void SpyAlert::LevelShutdown() {}
