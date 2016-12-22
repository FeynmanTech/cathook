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

const char* psza__HitboxT[] = {
	"ANY", "HEAD", "PELVIS", "SPINE 0", "SPINE 1", "SPINE 2", "SPINE 3", "UPPER ARM L", "LOWER ARM L",
	"HAND L", "UPPER ARM R", "LOWER ARM R", "HAND R", "HIP L", "KNEE L", "FOOT L", "HIP R",
	"KNEE R", "FOOT R"
};

Triggerbot::Triggerbot() {
	filter = new trace::FilterDefault();
	enemy_trace = new trace_t();
	this->v_bBodyshot = CREATE_CV(CV_SWITCH, "trigger_bodyshot", "1", "Bodyshot");
	this->v_bEnabled = CREATE_CV(CV_SWITCH, "trigger_enabled", "0", "Enable");
	this->v_bFinishingHit = CREATE_CV(CV_SWITCH, "trigger_finish", "1", "Noscope weak enemies");
	this->v_bIgnoreCloak = CREATE_CV(CV_SWITCH, "trigger_cloak", "0", "Ignore cloak");
	this->v_bZoomedOnly = CREATE_CV(CV_SWITCH, "trigger_zoomed", "1", "Zoomed only");
	this->v_iHitbox = CREATE_CV(new CatEnum(psza__HitboxT, ARRAYSIZE(psza__HitboxT), -1), "trigger_hitbox", "-1", "Hitbox");
	this->v_iMinRange = CREATE_CV(CV_INT, "trigger_range", "0", "Max range");
	this->v_bBuildings = CREATE_CV(CV_SWITCH, "trigger_buildings", "1", "Trigger @ Buildings");
}

bool Triggerbot::CreateMove(void* thisptr, float sampl, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (g_pLocalPlayer->life_state) return true;
	/*IClientEntity* local = interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer());
	if (!local) return;
	if (GetVar<char>(local, entityvars.iLifeState)) return;*/
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
	int eteam = GetVar<int>(entity, netvar.iTeamNum);
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
		(IsPlayerInvisible(entity))) return true;
	int health = GetVar<int>(entity, netvar.iHealth);
	bool bodyshot = false;
	if (g_pLocalPlayer->clazz == tf_class::tf_sniper) {
		// If sniper..
		if (health <= 50 && this->v_bFinishingHit->GetBool()) {
			bodyshot = true;
		}
		// If we need charge...
		if (!bodyshot && this->v_bBodyshot->GetBool()) {
			float bdmg = GetVar<float>(g_pLocalPlayer->weapon, netvar.flChargedDamage);
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
