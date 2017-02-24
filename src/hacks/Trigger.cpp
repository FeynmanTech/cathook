/*
 * HTrigger.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#include "Trigger.h"

#include "../common.h"
#include "../sdk.h"

namespace hacks { namespace shared { namespace triggerbot {

CatVar enabled(CV_SWITCH, "trigger_enabled", "0", "Enable trigger bot", NULL, "Master trigger bot switch");
CatVar teammates(CV_SWITCH, "trigger_teammates", "0", "Trigger on teammates", NULL, "Triggerbot works on teammates");
CatVar hitbox(CV_ENUM, "trigger_hitbox", "-1", "Hitbox", new CatEnum({
	"ANY", "HEAD", "PELVIS", "SPINE 0", "SPINE 1", "SPINE 2", "SPINE 3", "UPPER ARM L", "LOWER ARM L",
	"HAND L", "UPPER ARM R", "LOWER ARM R", "HAND R", "HIP L", "KNEE L", "FOOT L", "HIP R",
	"KNEE R", "FOOT R"
}, -1), "Triggerbot hitbox. Only useful settings are ANY and HEAD. Use ANY for scatter or any other shotgun-based weapon, HEAD for ambassador/sniper rifle");
CatVar respect_cloak(CV_SWITCH, "trigger_respect_cloak", "1", "Respect cloak", NULL, "Don't shoot cloaked spies");
CatVar zoomed_only(CV_SWITCH, "trigger_zoomed", "1", "Zoomed only", NULL, "Don't shoot if you aren't zoomed in");
CatVar bodyshot(CV_SWITCH, "trigger_bodyshot", "1", "Bodyshot", NULL, "Triggerbot will bodyshot enemies if you have enough charge to 1tap them");
CatVar finishing(CV_SWITCH, "trigger_finish", "1", "Noscope weak enemies", NULL, "If enemy has <50 HP, noscope them");
CatVar range(CV_INT, "trigger_range", "0", "Max range", NULL, "Triggerbot won't shoot if enemy is too far away", true, 4096);
CatVar buildings(CV_SWITCH, "trigger_buildings", "1", "Trigger at buildings", NULL, "Shoot buildings");
CatVar vaccinator(CV_SWITCH, "trigger_respect_vaccinator", "1", "Respect vaccinator", NULL, "Don't shoot at bullet-vaccinated enemies");
CatVar ambassador(CV_SWITCH, "trigger_ambassador", "1", "Smart Ambassador", NULL, "Don't shoot if yuor ambassador can't headshot yet");
CatVar accuracy(CV_SWITCH, "trigger_accuracy", "0", "Improve accuracy (WORK IN PROGRESS! DEBUG! DO NOT TOUCH!)", NULL, "Might cause more lag (NOT WORKING! DEBUG! DO NOT TOUCH!)");

void ProcessUserCmd(CUserCmd* cmd) {
	if (!enabled) return;
	if (g_LocalPlayer.weaponmode() != weapon_hitscan) return;
	if (TF2 && ambassador) {
		if (IsAmbassador(g_LocalPlayer.weapon())) {
			if ((g_pGlobals->curtime - g_LocalPlayer.weapon()->var<float>(netvar.flLastFireTime)) <= 1.0) {
				return;
			}
		}
	}
	Ray_t ray;
	trace::g_pFilterDefault->SetSelf(g_LocalPlayer.entity->entptr);
	Vector forward;
	float sp, sy, cp, cy;
	sy = sinf(DEG2RAD(cmd->viewangles[1]));
	cy = cosf(DEG2RAD(cmd->viewangles[1]));
	sp = sinf(DEG2RAD(cmd->viewangles[0]));
	cp = cosf(DEG2RAD(cmd->viewangles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 8192.0f + g_LocalPlayer.v_Eye;
	ray.Init(g_LocalPlayer.v_Eye, forward);
	static trace_t* trace = new trace_t();
	g_ITrace->TraceRay(ray, 0x4200400B, trace::g_pFilterDefault, trace);

	IClientEntity* raw_entity = (IClientEntity*)(trace->m_pEnt);
	if (!raw_entity) return;
	CachedEntity& entity = gEntityCache.Entity(raw_entity->entindex());

	k_EEntityType type = entity.Type();

	switch (type) {
	case ENTITY_PLAYER:
		if (IsPlayerInvulnerable(&entity)) return;
		if (vaccinator && HasCondition(&entity, TFCond_UberBulletResist)) return;
		if (respect_cloak && IsPlayerInvisible(&entity)) return;
		break;
	case ENTITY_BUILDING:
		if (!buildings) return;
		break;
	default:
		return;
	}

	if (!teammates && !entity.Enemy()) return;

	if (range) {
		if (entity.Distance() > range) return;
	}
	if (type == ENTITY_BUILDING) {
		cmd->buttons |= IN_ATTACK;
		return;
	}
	/*relation rel = GetRelation(entity);
	if (rel == relation::FRIEND || rel == relation::DEVELOPER) return;*/
	if (TF) {
		int health = entity.Health();
		bool dobody = false;
		if (g_LocalPlayer.entity->Class() == tf_class::tf_sniper) {
			if (finishing && health <= 50) {
				dobody = true;
			}
			if (!dobody && bodyshot) {
				float bdmg = g_LocalPlayer.weapon()->var<float>(netvar.flChargedDamage);
				if (CanHeadshot() && (bdmg) >= health) {
					dobody = true;
				}
			}
			if (zoomed_only && !dobody && !CanHeadshot()) return;
		}
	}

	//debug->AddBoxOverlay(enemy_trace->endpos, Vector(-1.0f, -1.0f, -1.0f), Vector(1.0f, 1.0f, 1.0f), QAngle(0, 0, 0), 255, 0, 0, 255, 2.0f);
	//IClientEntity* weapon;
	/*CachedHitbox* hb = entity->m_pHitboxCache->GetHitbox(enemy_trace->hitbox);
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
	}*/
	cmd->buttons |= IN_ATTACK;
}

}}};
