/*
 * HuntsmanCompensation.cpp
 *
 *  Created on: Dec 4, 2016
 *      Author: nullifiedcat
 */

#include "HuntsmanCompensation.h"

#include "../helpers.h"
#include "../localplayer.h"
#include "../trace.h"
#include "../interfaces.h"
#include "../usercmd.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(HuntsmanCompensation);

const char* HuntsmanCompensation::GetName() {
	return "HUNTSMAN COMPENSATION";
}

HuntsmanCompensation::HuntsmanCompensation() {
	this->v_bEnabled = CreateConVar("u_hc_enabled", "0", "Enable huntsman compensation");
}

bool HuntsmanCompensation::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;
	if (!g_pLocalPlayer->weapon || (g_pLocalPlayer->weapon->GetClientClass()->m_ClassID != ClassID::CTFCompoundBow)) return true;
	if (g_pLocalPlayer->life_state) return true;
	static trace_t* view_trace = new trace_t();
	Ray_t ray;
	trace::g_pFilterDefault->SetSelf(g_pLocalPlayer->entity);
	Vector forward;
	float sp, sy, cp, cy;
	sy = sinf(DEG2RAD(cmd->viewangles[1])); // yaw
	cy = cosf(DEG2RAD(cmd->viewangles[1]));

	sp = sinf(DEG2RAD(cmd->viewangles[0])); // pitch
	cp = cosf(DEG2RAD(cmd->viewangles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 8192.0f + g_pLocalPlayer->v_Eye;
	ray.Init(g_pLocalPlayer->v_Eye, forward);
	interfaces::trace->TraceRay(ray, 0x4200400B, trace::g_pFilterDefault, view_trace);
	float speed, grav;
	bool arc;
	GetProjectileData(g_pLocalPlayer->weapon, speed, arc, grav);
	float ttf = view_trace->startpos.DistTo(view_trace->endpos) / speed;
	Vector resv = view_trace->endpos;
	resv.z += ttf * ttf * (grav * 400);
	AimAt(g_pLocalPlayer->v_Eye, resv, cmd);
	g_pLocalPlayer->bUseSilentAngles = true;
	return true;
}

void HuntsmanCompensation::PaintTraverse(void*, unsigned int, bool, bool) {}
