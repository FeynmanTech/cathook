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

	for (int i = 0; i < 64 && i < HIGHEST_ENTITY; i++) {
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
	CachedEntity* ent = ENTITY(idx);
	int priority = 0;
	int health = CE_INT(ent, netvar.iHealth);
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
	CachedEntity* ent = ENTITY(idx);
	if (!ent) return false;
	if (ent->m_Type != ENTITY_PLAYER) return false;
	if (interfaces::engineClient->GetLocalPlayer() == idx) return false;
	if (!ent->m_bAlivePlayer) return false;
	if (ent->m_bEnemy) return false;
	if (ent->m_flDistance > 420) return false;
	// TODO visible any hitbox
	if (!IsEntityVisible(ent, 7)) return false;
	if (IsPlayerInvisible(ent)) return false;
	return true;
}

AutoHeal::AutoHeal() {
	this->v_bEnabled = CREATE_CV(CV_SWITCH, "autoheal_enabled", "0", "Enable");
	this->v_bSilent = CREATE_CV(CV_SWITCH, "autoheal_silent", "1", "Silent");
	m_iCurrentHealingTarget = -1;
}

bool AutoHeal::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (CE_BAD(g_pLocalPlayer->entity) || CE_BAD(g_pLocalPlayer->weapon())) return true;
	if (g_pLocalPlayer->life_state) return true;
	if (GetWeaponMode(g_pLocalPlayer->entity) != weapon_medigun) return true;
	int old_target = m_iCurrentHealingTarget;
	m_iCurrentHealingTarget = GetBestHealingTarget();
	if (m_iNewTarget > 0 && m_iNewTarget < 10) m_iNewTarget++;
	else m_iNewTarget = 0;
	bool new_target = (old_target != m_iCurrentHealingTarget);
	if (new_target) {
		m_iNewTarget = 1;
	}
	if (m_iCurrentHealingTarget == -1) return true;
	CachedEntity* target = ENTITY(m_iCurrentHealingTarget);
	Vector out;
	GetHitbox(target, 7, out);
	AimAt(g_pLocalPlayer->v_Eye, out, cmd);
	if (v_bSilent->GetBool()) g_pLocalPlayer->bUseSilentAngles = true;
	if (!m_iNewTarget && (interfaces::gvars->tickcount % 60)) cmd->buttons |= IN_ATTACK;
	return false;
}

void AutoHeal::PaintTraverse(void*, unsigned int, bool, bool) {
	//if (m_iCurrentHealingTarget >= 0)
		//gEntityCache.GetEntity(m_iCurrentHealingTarget)->AddESPString(colors::white, colors::black, "Healing priority: %i", GetHealingPriority(m_iCurrentHealingTarget));
}

void AutoHeal::LevelInit(const char*) {}
void AutoHeal::LevelShutdown() {}
