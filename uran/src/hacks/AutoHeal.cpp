/*
 * AutoHeal.cpp
 *
 *  Created on: Dec 3, 2016
 *      Author: nullifiedcat
 */

#include "AutoHeal.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(AutoHeal);

const char* AutoHeal::GetName() { return "AUTO-HEAL"; }

int AutoHeal::GetBestHealingTarget() {
	int best = -1;
	int best_score = -65536;

	for (int i = 0; i < 64 && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		int score = this->GetHealingPriority(i);
		if (score > best_score && score != -1) {
			best = i;
			best_score = score;
		}
	}

	return best;
}

int AutoHeal::GetHealingPriority(int idx) {
	if (!CanHeal(idx)) return -1;
	IClientEntity* ent = interfaces::entityList->GetClientEntity(idx);
	int priority = 0;
	int health = GetEntityValue<int>(ent, eoffsets.iHealth);
	int maxhealth = g_pPlayerResource->GetMaxHealth(ent);
	int maxbuffedhealth = maxhealth * 1.5;
	int maxoverheal = maxbuffedhealth - maxhealth;
	int overheal = maxoverheal - (maxbuffedhealth - health);
	float overhealp = ((float)overheal / (float)maxoverheal);
	float healthp = ((float)health / (float)maxhealth);
	if (GetRelation(ent) == relation::FRIEND) {
		priority += 70 * (1 - healthp);
		priority += 15 * (1 - overhealp);
	} else {
		priority += 50 * (1 - healthp);
		priority += 10 * (1 - overhealp);
	}
	return priority;
}

bool AutoHeal::CanHeal(int idx) {
	IClientEntity* ent = interfaces::entityList->GetClientEntity(idx);
	if (!ent) return false;
	if (ent->GetClientClass()->m_ClassID != ClassID::CTFPlayer) return false;
	if (GetEntityValue<char>(ent, eoffsets.iLifeState)) return false;
	if (g_pLocalPlayer->team != GetEntityValue<int>(ent, eoffsets.iTeamNum)) return false;
	if (g_pLocalPlayer->v_Origin.DistToSqr(ent->GetAbsOrigin()) > 420 * 420) return false;
	if (!IsEntityVisible(ent, 7)) return false;
	if (GetEntityValue<int>(ent, eoffsets.iCond) & cond::cloaked) return false;
	return true;
}

AutoHeal::AutoHeal() {
	this->v_bEnabled = CreateConVar("u_autoheal_enabled", "0", "Enable AutoHeal");
	this->v_bSilent = CreateConVar("u_autoheal_silent", "1", "Silent AutoHeal");
	m_iCurrentHealingTarget = -1;
}

bool AutoHeal::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (GetWeaponMode(g_pLocalPlayer->entity) != weapon_medigun) return true;
	if (g_pLocalPlayer->life_state) return true;
	int old_target = m_iCurrentHealingTarget;
	m_iCurrentHealingTarget = GetBestHealingTarget();
	if (m_iNewTarget > 0 && m_iNewTarget < 10) m_iNewTarget++;
	else m_iNewTarget = 0;
	bool new_target = (old_target != m_iCurrentHealingTarget);
	if (new_target) {
		m_iNewTarget = 1;
	}
	if (m_iCurrentHealingTarget == -1) return true;
	IClientEntity* target = interfaces::entityList->GetClientEntity(m_iCurrentHealingTarget);
	Vector out;
	GetHitboxPosition(target, 7, out);
	AimAt(g_pLocalPlayer->v_Eye, out, cmd);
	if (v_bSilent->GetBool()) g_pLocalPlayer->bUseSilentAngles = true;
	if (!m_iNewTarget && (interfaces::gvars->tickcount % 60)) cmd->buttons |= IN_ATTACK;
	return false;
}

void AutoHeal::PaintTraverse(void*, unsigned int, bool, bool) {
	//if (m_iCurrentHealingTarget >= 0)
		//gEntityCache.GetEntity(m_iCurrentHealingTarget)->AddESPString(colors::white, colors::black, "Healing priority: %i", GetHealingPriority(m_iCurrentHealingTarget));
}
