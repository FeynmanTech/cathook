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

Vector eye;
trace_t* enemy_trace;
trace::FilterDefault* filter;

Triggerbot::Triggerbot() {
	filter = new trace::FilterDefault();
	enemy_trace = new trace_t();
	this->v_bBodyshot = new CatVar(CV_SWITCH, "trigger_bodyshot", "1", "Bodyshot", NULL, "Triggerbot will bodyshot enemies if you have enough charge to 1tap them");
	this->v_bEnabled = new CatVar(CV_SWITCH, "trigger_enabled", "0", "Enable", NULL, "Master Triggerbot switch");
	this->v_bFinishingHit = new CatVar(CV_SWITCH, "trigger_finish", "1", "Noscope weak enemies", NULL, "If enemy has <50 HP, noscope them");
	this->v_bRespectCloak = new CatVar(CV_SWITCH, "trigger_respect_cloak", "1", "Respect cloak", NULL, "Don't shoot at cloaked spies");
	this->v_bZoomedOnly = new CatVar(CV_SWITCH, "trigger_zoomed", "1", "Zoomed only", NULL, "Don't shoot if you aren't zoomed in");
	this->v_iHitbox = new CatVar(CV_ENUM, "trigger_hitbox", "-1", "Hitbox", new CatEnum({
		"ANY", "HEAD", "PELVIS", "SPINE 0", "SPINE 1", "SPINE 2", "SPINE 3", "UPPER ARM L", "LOWER ARM L",
		"HAND L", "UPPER ARM R", "LOWER ARM R", "HAND R", "HIP L", "KNEE L", "FOOT L", "HIP R",
		"KNEE R", "FOOT R"
	}, -1), "Triggerbot hitbox. Only useful settings are ANY and HEAD. Use ANY for scatter or any other shotgun-based weapon, HEAD for ambassador/sniper rifle");
	this->v_iMaxRange = new CatVar(CV_INT, "trigger_range", "0", "Max range", NULL, "Triggerbot won't shoot if enemy is too far away", true, 4096.0f);
	this->v_bBuildings = new CatVar(CV_SWITCH, "trigger_buildings", "1", "Trigger at buildings", NULL, "Shoot buildings");
	this->v_bIgnoreVaccinator = new CatVar(CV_SWITCH, "trigger_respect_vaccinator", "1", "Respect vaccinator", NULL, "Don't shoot at bullet-vaccinated enemies");
	this->v_bAmbassadorCharge = new CatVar(CV_SWITCH, "trigger_ambassador", "1", "Smart Ambassador", NULL, "Don't shoot if yuor ambassador can't headshot yet");
	this->v_bImproveAccuracy = new CatVar(CV_SWITCH, "trigger_accuracy", "0", "Improve accuracy (NOT WORKING)", NULL, "Might cause more lag (NOT WORKING YET!)");
}

void Triggerbot::ProcessUserCmd(CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return;
	if (GetWeaponMode(g_LocalPlayer->entity) != weapon_hitscan) return;
	if (v_bAmbassadorCharge->GetBool()) {
		if (IsAmbassador(g_LocalPlayer->weapon())) {
			if ((g_pGlobals->curtime - CE_FLOAT(g_LocalPlayer->weapon(), netvar.flLastFireTime)) <= 1.0) {
				return;
			}
		}
	}
	Ray_t ray;
	filter->SetSelf(RAW_ENT(g_LocalPlayer->entity));
	eye = g_LocalPlayer->v_Eye;
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
	g_ITrace->TraceRay(ray, 0x4200400B, filter, enemy_trace);

	IClientEntity* raw_entity = (IClientEntity*)(enemy_trace->m_pEnt);
	if (!raw_entity) return;
	CachedEntity* entity = ENTITY(raw_entity->entindex());
	if (!entity->m_bEnemy) return;

	bool isPlayer = false;
	switch (entity->m_Type) {
	case k_EEntityType::ENTITY_PLAYER:
		isPlayer = true; break;
	case k_EEntityType::ENTITY_BUILDING:
		if (!this->v_bBuildings->GetBool()) return;
		break;
	default:
		return;
	};

	Vector enemy_pos = entity->m_vecOrigin;
	Vector my_pos = g_LocalPlayer->entity->m_vecOrigin;
	if (v_iMaxRange->GetInt() > 0) {
		if (entity->m_flDistance > v_iMaxRange->GetInt()) return;
	}
	if (!isPlayer) {
		cmd->buttons |= IN_ATTACK;
		return;
	}
	if (HasCondition(entity, TFCond_UberBulletResist) && v_bIgnoreVaccinator->GetBool()) return;
	relation rel = GetRelation(entity);
	if (rel == relation::FRIEND || rel == relation::DEVELOPER) return;
	if (IsPlayerInvulnerable(entity)) return;
	if (this->v_bRespectCloak->GetBool() &&
		(IsPlayerInvisible(entity))) return;
	int health = CE_INT(entity, netvar.iHealth);
	bool bodyshot = false;
	if (g_LocalPlayer->clazz == tf_class::tf_sniper) {
		// If sniper..
		if (health <= 50 && this->v_bFinishingHit->GetBool()) {
			bodyshot = true;
		}
		// If we need charge...
		if (!bodyshot && this->v_bBodyshot->GetBool()) {
			float bdmg = CE_FLOAT(g_LocalPlayer->weapon(), netvar.flChargedDamage);
			if (CanHeadshot() && (bdmg) >= health) {
				bodyshot = true;
			}
		}

	}
	if (!bodyshot && (g_LocalPlayer->clazz == tf_class::tf_sniper) && this->v_bZoomedOnly->GetBool() &&
		!((g_LocalPlayer->bZoomed) && CanHeadshot())) {
		return;
	}
	//debug->AddBoxOverlay(enemy_trace->endpos, Vector(-1.0f, -1.0f, -1.0f), Vector(1.0f, 1.0f, 1.0f), QAngle(0, 0, 0), 255, 0, 0, 255, 2.0f);
	//IClientEntity* weapon;
	CachedHitbox* hb = entity->m_pHitboxCache->GetHitbox(enemy_trace->hitbox);
	//logging::Info("hitbox: %i 0x%08x", enemy_trace->hitbox, hb);

	if (v_bImproveAccuracy->GetBool()) {
		if (hb) {
			Vector siz = hb->max - hb->min;
			Vector mns = hb->min + siz * 0.2f;
			Vector mxs = hb->max - siz * 0.2f;
			g_IDebugOverlay->AddLineOverlay(enemy_trace->startpos, forward, 0, 0, 255, true, -1.0f);
			if (LineIntersectsBox(mns, mxs, enemy_trace->startpos, forward)) {
				g_IDebugOverlay->AddBoxOverlay(mns, Vector(0, 0, 0), mxs - mns, QAngle(0, 0, 0), 0, 255, 0, 255, 1.0f);
				g_IDebugOverlay->AddLineOverlay(enemy_trace->startpos, forward, 255, 0, 0, true, 1.0f);
				//logging::Info("%.2f %.2f %.2f", hb->center.DistToSqr(enemy_trace->endpos), SQR(hb->min.DistToSqr(hb->min)), SQR(hb->min.DistToSqr(hb->min) * 0.9f));

			} else {
				g_IDebugOverlay->AddBoxOverlay(hb->min, Vector(0, 0, 0), hb->max - hb->min, QAngle(0, 0, 0), 0, 255, 255, 255, -1.0f);
				g_IDebugOverlay->AddBoxOverlay(mns, Vector(0, 0, 0), mxs - mns, QAngle(0, 0, 0), 255, 255, 0, 255, 0.5f);
				return;
			}
		} else return;
	}
	if (this->v_iHitbox->GetInt() >= 0 && !bodyshot) {
		if (enemy_trace->hitbox != this->v_iHitbox->GetInt()) return;
	}
	cmd->buttons |= IN_ATTACK;
}

Triggerbot::~Triggerbot() {
	delete filter;
	delete enemy_trace;
}
