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
	this->v_bIgnoreVaccinator = CREATE_CV(CV_SWITCH, "trigger_respect_vaccinator", "1", "Don't shoot at vaccinated enemies");
	this->v_bAmbassadorCharge = CREATE_CV(CV_SWITCH, "trigger_ambassador", "1", "Smart Ambassador");
}

bool Triggerbot::CreateMove(void* thisptr, float sampl, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (g_pLocalPlayer->life_state) return true;
	/*IClientEntity* local = ENTITY(interfaces::engineClient->GetLocalPlayer());
	if (!local) return;
	if (NET_BYTE(local, entityvars.iLifeState)) return;*/
	if (GetWeaponMode(g_pLocalPlayer->entity) != weapon_hitscan) return true;
	if (v_bAmbassadorCharge->GetBool()) {
		if (IsAmbassador(g_pLocalPlayer->weapon())) {
			if ((interfaces::gvars->curtime - CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flLastFireTime)) <= 1.0) {
				return true;
			}
		}
	}
	Ray_t ray;
	filter->SetSelf(RAW_ENT(g_pLocalPlayer->entity));
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
	if (v_iHitbox->GetInt() == -1) {
		interfaces::trace->TraceRay(ray, MASK_SHOT_HULL, filter, enemy_trace);
	} else {
		interfaces::trace->TraceRay(ray, 0x4200400B, filter, enemy_trace);
	}

	IClientEntity* raw_entity = (IClientEntity*)(enemy_trace->m_pEnt);
	if (!raw_entity) return true;
	CachedEntity* entity = ENTITY(raw_entity->entindex());
	if (!entity->m_bEnemy) return true;

	bool isPlayer = false;
	switch (entity->m_Type) {
	case EntityType::ENTITY_PLAYER:
		isPlayer = true; break;
	case EntityType::ENTITY_BUILDING:
		if (!this->v_bBuildings->GetBool()) return true;
		break;
	default:
		return true;
	};

	Vector enemy_pos = entity->m_vecOrigin;
	Vector my_pos = g_pLocalPlayer->entity->m_vecOrigin;
	if (v_iMinRange->GetInt() > 0) {
		if (entity->m_flDistance > v_iMinRange->GetInt()) return true;
	}
	if (!isPlayer) {
		cmd->buttons |= IN_ATTACK;
		return true;
	}
	if (HasCondition(entity, TFCond_UberBulletResist) && v_bIgnoreVaccinator->GetBool()) return true;
	relation rel = GetRelation(entity);
	if (rel == relation::FRIEND || rel == relation::DEVELOPER) return true;
	if (IsPlayerInvulnerable(entity)) return true;
	if (!this->v_bIgnoreCloak->GetBool() &&
		(IsPlayerInvisible(entity))) return true;
	int health = CE_INT(entity, netvar.iHealth);
	bool bodyshot = false;
	if (g_pLocalPlayer->clazz == tf_class::tf_sniper) {
		// If sniper..
		if (health <= 50 && this->v_bFinishingHit->GetBool()) {
			bodyshot = true;
		}
		// If we need charge...
		if (!bodyshot && this->v_bBodyshot->GetBool()) {
			float bdmg = CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flChargedDamage);
			if (CanHeadshot() && (bdmg) >= health) {
				bodyshot = true;
			}
		}

	}
	if (!bodyshot && (g_pLocalPlayer->clazz == tf_class::tf_sniper) && this->v_bZoomedOnly->GetBool() &&
		!((g_pLocalPlayer->bZoomed) && CanHeadshot())) {
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
