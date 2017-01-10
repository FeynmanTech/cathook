/*
 * prediction.cpp
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#include "prediction.h"

#include "common.h"
#include "sdk.h"

// TODO there is a Vector() object created each call.

Vector SimpleLatencyPrediction(CachedEntity* ent, int hb) {
	if (!ent) return Vector();
	Vector result;
	GetHitbox(ent, hb, result);
	float latency = interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) +
			interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	result += CE_VECTOR(ent, netvar.vVelocity) * latency;
	return result;
}

Vector ProjectilePrediction(CachedEntity* ent, int hb, float speed, float gravitymod) {
	if (!ent) return Vector();
	Vector result = ent->m_vecOrigin;
	if (speed == 0.0f) return Vector();
	float dtg = DistanceToGround(result);
	int flags = CE_INT(ent, netvar.iFlags);
	bool ground = (flags & (1 << 0));
	if (ground) dtg = 0;
	GetHitbox(ent, hb, result);
	Vector vel = CE_VECTOR(ent, netvar.vVelocity);
	// TODO ProjAim
	float ott = g_pLocalPlayer->v_Eye.DistTo(result) / speed + interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) +
			interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	float tt = ott - 0.5f;
	if (tt <= 0.0f) tt = 0.01f;
	float besttime = tt;
	float mindelta = 65536.0f;
	Vector bestpos = result;
	int maxsteps = 100;
	for (int steps = 0; steps < maxsteps; steps++, tt += ((float)1 / (float)maxsteps)) {
		Vector curpos = result;
		curpos += vel * tt;
		if (dtg > 0.0f) {
			curpos.z -= tt * tt * 400;
			if (curpos.z < result.z - dtg) curpos.z = result.z - dtg;
		}
		float rockettime = g_pLocalPlayer->v_Eye.DistTo(curpos) / speed + interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) +
				interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
		//logging::Info("RocketTime: %.2f TT: %.2f Step: %i BestTime: %.2f", rockettime, tt, steps, besttime);
		if (fabs(rockettime - tt) < mindelta) {
			//if (mindelta != 65536.0) logging::Info("got better delta: %.2f at step %i (time: %.2f)", fabs(rockettime - tt), steps, tt);
			besttime = tt;
			bestpos = curpos;
			mindelta = fabs(rockettime - tt);
		}
	}

	/*float dtt = g_pLocalPlayer->v_Eye.DistTo(result);
	float ttt = dtt / speed + interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) +
		interfaces::engineClient->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	float oz = result.z;
	int flags = CE_INT(ent, netvar.iFlags);
	bool ground = (flags & (1 << 0));
	if (!ground) result.z -= ttt * ttt * 400;
	result += vel * ttt;
	if (!ground) if (oz - result.z > dtg) { result.z = oz - dtg; }
	*/
	bestpos.z += (400 * besttime * besttime * gravitymod);
	// S = at^2/2 ; t = sqrt(2S/a)*/
	return bestpos;
}

float DistanceToGround(Vector origin) {
	static trace_t* ground_trace = new trace_t();
	Ray_t ray;
	Vector endpos = origin;
	endpos.z -= 8192;
	ray.Init(origin, endpos);
	interfaces::trace->TraceRay(ray, 0x4200400B, trace::g_pFilterNoPlayer, ground_trace);
	return ground_trace->startpos.DistTo(ground_trace->endpos);
}
