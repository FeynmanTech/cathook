/*
 * HTrigger.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#include "Trigger.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(Triggerbot);

const char* Triggerbot::GetName() {
	return "TRIGGER";
}

Vector eye;
trace_t* enemy_trace;
trace::FilterDefault* filter;

Triggerbot::Triggerbot() {
	filter = new trace::FilterDefault();
	enemy_trace = new trace_t();
	this->v_bBodyshot = CreateConVar(CON_PREFIX "trigger_bodyshot", "1", "Bodyshot");
	this->v_bEnabled = CreateConVar(CON_PREFIX "trigger_enabled", "0", "Enable");
	this->v_bFinishingHit = CreateConVar(CON_PREFIX "trigger_finish", "1", "Noscope weak enemies");
	this->v_bIgnoreCloak = CreateConVar(CON_PREFIX "trigger_cloak", "0", "Ignore cloak");
	this->v_bZoomedOnly = CreateConVar(CON_PREFIX "trigger_zoomed", "1", "Zoomed only");
	this->v_iHitbox = CreateConVar(CON_PREFIX "trigger_hitbox", "-1", "Hitbox");
	this->v_iMinRange = CreateConVar(CON_PREFIX "trigger_range", "0", "Max range");
	this->v_bBuildings = CreateConVar(CON_PREFIX "trigger_buildings", "1", "Trigger @ Buildings");
}

bool Triggerbot::CreateMove(void* thisptr, float sampl, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (g_pLocalPlayer->life_state) return true;
	/*IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
	if (!local) return;
	if (GetEntityValue<char>(local, entityvars.iLifeState)) return;*/
	Ray_t ray;
	filter->SetSelf(g_pLocalPlayer->entity);
	eye = g_pLocalPlayer->v_Eye;
	Vector forward;
	float sp, sy, cp, cy;
	sy = sinf(DEG2RAD(cmd->viewangles[1])); // yaw
	cy = cosf(DEG2RAD(cmd->viewangles[1]));

	sp = sinf(DEG2RAD(cmd->viewangles[0])); // pitch
	cp = cosf(DEG2RAD(cmd->viewangles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 8192.0f + eye;
	ray.Init(eye, forward);
	interfaces::trace->TraceRay(ray, 0x4200400B, filter, enemy_trace);
	IClientEntity* entity = (IClientEntity*)(enemy_trace->m_pEnt);
	if (!entity) return true;
	bool isPlayer = false;
	switch (entity->GetClientClass()->m_ClassID) {
	case ClassID::CTFPlayer:
		isPlayer = true;
	break;
	case ClassID::CObjectTeleporter:
	case ClassID::CObjectSentrygun:
	case ClassID::CObjectDispenser:
		if (!this->v_bBuildings->GetBool()) {
			return true;
		}
	break;
	default:
	return true;
	};
	int team = g_pLocalPlayer->team;
	int eteam = GetEntityValue<int>(entity, netvar.iTeamNum);
	if (team == eteam) return true;
	Vector enemy_pos = entity->GetAbsOrigin();
	Vector my_pos = g_pLocalPlayer->entity->GetAbsOrigin();
	if (v_iMinRange->GetInt() > 0) {
		if ((enemy_pos - my_pos).Length() > v_iMinRange->GetInt()) return true;
	}
	if (!isPlayer) {
		cmd->buttons |= IN_ATTACK;
		return true;
	}
	relation rel = GetRelation(entity);
	if (rel == relation::FRIEND || rel == relation::DEVELOPER) return true;
	if (IsPlayerInvulnerable(entity)) return true;
	if (!this->v_bIgnoreCloak->GetBool() &&
		((GetEntityValue<int>(entity, netvar.iCond)) & cond::cloaked)) return true;
	int health = GetEntityValue<int>(entity, netvar.iHealth);
	bool bodyshot = false;
	if (g_pLocalPlayer->clazz == tf_class::tf_sniper) {
		// If sniper..
		if (health <= 50 && this->v_bFinishingHit->GetBool()) {
			bodyshot = true;
		}
		// If we need charge...
		if (!bodyshot && this->v_bBodyshot->GetBool()) {
			float bdmg = GetEntityValue<float>(g_pLocalPlayer->weapon, netvar.flChargedDamage);
			if (CanHeadshot(g_pLocalPlayer->entity) && (bdmg) >= health) {
				bodyshot = true;
			}
		}

	}
	if (!bodyshot && (g_pLocalPlayer->clazz == tf_class::tf_sniper) && this->v_bZoomedOnly->GetBool() &&
		!((g_pLocalPlayer->bZoomed) && CanHeadshot(g_pLocalPlayer->entity))) {
		return true;
	}
	//IClientEntity* weapon;
	if (this->v_iHitbox->GetInt() >= 0 && !bodyshot) {
		if (enemy_trace->hitbox != this->v_iHitbox->GetInt()) return true;
	}
	cmd->buttons |= IN_ATTACK;
	return true;
}

Triggerbot::~Triggerbot() {
	delete filter;
	delete enemy_trace;
}

void Triggerbot::PaintTraverse(void*, unsigned int, bool, bool) {};
