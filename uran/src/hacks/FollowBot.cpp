/*
 * HPyroBot.cpp
 *
 *  Created on: Oct 30, 2016
 *      Author: nullifiedcat
 */

#include "FollowBot.h"

#include "../interfaces.h"
#include "../entity.h"
#include "../helpers.h"
#include "../localplayer.h"
#include "../logging.h"
#include "../usercmd.h"
#include "../playerresource.h"
#include "../sdk/in_buttons.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <icliententitylist.h>
#include <client_class.h>
#include <cdll_int.h>
#include <igameevents.h>

enum bot_state_t {
	IDLE = 0,
	FOLLOWING_TEAMMATE,
	FOLLOWING_ENEMY,
	PILOT_SEARCH,
	TARGET_LOST
};

unsigned long g_nTick = 0;
int g_nTargetID = 0;
unsigned long nPilotSearch = 0;
unsigned long SEARCH_TICKS = 250;
bot_state_t g_bState = IDLE;

void SetTarget(IClientEntity* ent) {
	if (!ent) {
		g_nTargetID = -1;
		return;
	}
	g_nTargetID = ent->entindex();
}

IClientEntity* GetTarget() {
	if (g_nTargetID == -1) return 0;
	return interfaces::entityList->GetClientEntity(g_nTargetID);
}

int called = 0;

class MedicCallListener : public IGameEventListener2 {
public:
	MedicCallListener() {}
	void FireGameEvent(IGameEvent* pEvent) {
		if (!g_phFollowBot->v_bEnabled->GetBool()) return;
		if (strcmp("player_calledformedic", pEvent->GetName())) return;
		int id = interfaces::engineClient->GetPlayerForUserID(pEvent->GetInt("userid", -1));
		logging::Info("%i called for medic");
		player_info_s info;
		interfaces::engineClient->GetPlayerInfo(id, &info);
		if (info.friendsID == g_phFollowBot->m_nOwnerID) {
			g_phFollowBot->m_iShouldUbercharge = 1;
		}
	}
};

void CC_ResetList(const CCommand& args) {
	g_phFollowBot->ResetBotList();
}

MedicCallListener* g_pListener;

void FollowBot::ProcessEntity(IClientEntity* entity, bool enemy) {
	IClientEntity* target = GetTarget();

	if (!target) {
		if (!enemy || v_bMediBot->GetBool()) {
			if (v_iForceFollow->GetInt() != -1) {
				if (v_bForceFollowOnly->GetBool() && (v_iForceFollow->GetInt() != entity->entindex())) {
					return;
				}
			}
		}
		target = entity;
		SetTarget(target);
	}

	if (enemy && !v_bMediBot->GetBool()) {
		if (g_bState == bot_state_t::FOLLOWING_ENEMY) {
			if (DistToSqr(entity) <= DistToSqr(target)) {
				target = entity;
				SetTarget(target);
			}
		} else {
			target = entity;
			SetTarget(target);
		}
		g_bState = bot_state_t::FOLLOWING_ENEMY;
	} else {
		//if (g_bState == bot_state_t::PILOT_SEARCH) {

		//}
		if (g_bState == bot_state_t::FOLLOWING_ENEMY) return;
		if (GetEntityValue<Vector>(entity, eoffsets.vVelocity).IsZero(1.0f)) return;
		if (v_iForceFollow->GetInt() != -1) {
			if (entity->entindex() == v_iForceFollow->GetInt()) {
				target = entity;
				SetTarget(target);
				g_bState = bot_state_t::FOLLOWING_TEAMMATE;
			} else {
				return;
			}
		}
		if (DistToSqr(entity) <= DistToSqr(target)) {
			target = entity;
			SetTarget(target);
			g_bState = bot_state_t::FOLLOWING_TEAMMATE;
		}
	}
}

/*
 * pick target
 * if can't pick target:
 *  forward
 * follow
 * if can't follow:
 * 	goto pick target
 * if at shooting distance:
 *  shoot
 */

int FollowBot::ShouldNotTarget(IClientEntity* ent, bool notrace) {
	if (!ent || ent->IsDormant()) return 1;
	if (ent->GetClientClass()->m_ClassID != 241) return 2;
	if (GetEntityValue<char>(ent, eoffsets.iLifeState)) return 3;
	bool enemy = GetEntityValue<int>(ent, eoffsets.iTeamNum) != g_pLocalPlayer->team;
	if (enemy) {
		if (IsPlayerInvulnerable(ent)) return 4;
	}

	if (!notrace) {
		bool a = false;
		for (int i = 0; i < 17; i++) {
			if (IsEntityVisible(ent, i)) a = true;
		}
		if (!a) return 6;
	}

	if (v_bMediBot->GetBool()) {
		if (!this->IsOwner(ent)) {
			return 7;
		}
	} else {
		if (DistToSqr(ent) > (v_iMaxDistance->GetInt() * v_iMaxDistance->GetInt())) return 5;
		if (abs(ent->GetAbsOrigin().z - g_pLocalPlayer->v_Origin.z) > v_iMaxDeltaY->GetInt()) return 8;
	}

	return 0;
}

void FollowBot::Tick(CUserCmd* cmd) {
	if (!g_pLocalPlayer->entity || g_pLocalPlayer->entity->IsDormant()) return;
	if (g_pLocalPlayer->life_state) return;

	IClientEntity* target = GetTarget();
	IClientEntity* target_old = target;

	if (v_bMediBot->GetBool()) {
		cmd->buttons |= IN_ATTACK;
		if ((g_nTick % 20) == 0) {
			interfaces::engineClient->ExecuteClientCmd("slot2");
			cmd->buttons &= ~IN_ATTACK;
		}
		if (this->m_iShouldUbercharge && this->m_iShouldUbercharge < 30) {
			cmd->buttons |= IN_ATTACK2;
			interfaces::engineClient->ExecuteClientCmd("voicemenu 2 1");
			this->m_iShouldUbercharge++;
		} else {
			this->m_iShouldUbercharge = 0;
		}

		if (!target || ShouldNotTarget(target, true)) {
			//g_bState = bot_state_t::PILOT_SEARCH;
			nPilotSearch++;
		} else {
			//g_bState = bot_state_t::FOLLOWING_TEAMMATE;
			nPilotSearch = 0;
		}

		// TODO movements with cmd
		if (nPilotSearch < SEARCH_TICKS) {
			//interfaces::engineClient->ExecuteClientCmd("+forward");
		} else {
			//interfaces::engineClient->ExecuteClientCmd("-forward");
		}
		IClientEntity* ptf = 0;
		for (int i = 0; i < 64 && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
			if (IsOwner(interfaces::entityList->GetClientEntity(i))) {
				m_hTargetFollowing = i;
				ptf = interfaces::entityList->GetClientEntity(i);
			}
		}

		IClientEntity* healtr = this->GetBestHealingTarget();
		m_hTargetHealing = (healtr ? healtr->entindex() : 0);
		if (healtr) {
			if (GetEntityValue<int>(healtr, eoffsets.iHealth) < 35 && !GetEntityValue<char>(healtr, eoffsets.iLifeState)) {
				m_iShouldUbercharge = 1;
			}
			if (g_pLocalPlayer->health < 35) {
				m_iShouldUbercharge = 1;
			}
		}

		if (!ptf) return;

		if (GetEntityValue<Vector>(g_pLocalPlayer->entity, eoffsets.vVelocity).IsZero(10.0f) && (0 == (g_nTick % 20))) {
			interfaces::engineClient->ExecuteClientCmd("+jump");
		} else if (0 == (g_nTick % 20)) {
			interfaces::engineClient->ExecuteClientCmd("-jump");
		}

		if (DistToSqr(ptf) < (100 * 100) && (0 == (g_nTick % 20))) {
			interfaces::engineClient->ExecuteClientCmd("-forward");
		} else if (0 == (g_nTick % 20)) {
			interfaces::engineClient->ExecuteClientCmd("+forward");
		}
		return;
	} else {
		return;
	}

	// OLD CODE

	/*bool target_lost = (!target || ShouldNotTarget(target, true) || (g_bState != bot_state_t::FOLLOWING_ENEMY && g_bState != bot_state_t::FOLLOWING_TEAMMATE));

	if (target_lost && g_bState != bot_state_t::PILOT_SEARCH) logging::Info("Target lost! Target: %i, State: %i, ShouldTarget: %i", target, g_bState, ShouldNotTarget(target, true));

	if (target_lost && g_bState == bot_state_t::FOLLOWING_ENEMY) {
		g_bState = bot_state_t::PILOT_SEARCH;
		nPilotSearch++;
		//SetTarget(0);
		//target = 0;
	}
	if (target_lost && (g_bState == bot_state_t::FOLLOWING_TEAMMATE || g_bState == bot_state_t::PILOT_SEARCH)) {
		g_bState = bot_state_t::PILOT_SEARCH;
		nPilotSearch++;
	}

	if (!target_lost) {
		nPilotSearch = 0;
	}

	if (nPilotSearch < SEARCH_TICKS) {
		interfaces::engineClient->ExecuteClientCmd("+forward");
	} else {
		interfaces::engineClient->ExecuteClientCmd("-forward");
	}

	if ((g_nTick % 300 == 0) || target_lost) {
		for (int i = 0; i < 64 && i < interfaces::entityList->GetMaxEntities(); i++) {
			IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
			if (ShouldNotTarget(ent, false)) continue;
			bool enemy = GetEntityValue<int>(ent, eoffsets.iTeamNum) != g_pLocalPlayer->team;
			ProcessEntity(ent, enemy);
		}
	}

	target = GetTarget();
	target_lost = (!target || ShouldNotTarget(target, true) || g_bState == bot_state_t::IDLE || g_bState == bot_state_t::PILOT_SEARCH || g_bState == bot_state_t::TARGET_LOST);

	if (target_lost) {
		if (target_old != 0) {
			if (v_bChat->GetBool()) {
				//char* cmd = new char[256];
				//sprintf(cmd, "say Target lost!", g_bState);
				//interfaces::engineClient->ServerCmd(cmd);
			}
			else {
				//logging::Info("Target lost! State: %i, %i", !!target, !!ShouldTarget(target));
			}
		}
		//if (ShouldTarget(target, true) != 6 && (nPilotSearch < 100)) return;
		interfaces::engineClient->ExecuteClientCmd("-attack");
		return;
	}

	if (target != target_old) {
		player_info_s info;
		interfaces::engineClient->GetPlayerInfo(target->entindex(), &info);
		logging::Info("Following %s", info.name);
		if (v_bChat->GetBool()) {
			char* cmd = new char[256];
			if (g_bState == bot_state_t::FOLLOWING_TEAMMATE)
				sprintf(cmd, "say Following: %s", info.name);
			else
				sprintf(cmd, "say Attacking: %s", info.name);
			interfaces::engineClient->ServerCmd(cmd);
		}
	}

	if (GetEntityValue<Vector>(g_pLocalPlayer->entity, eoffsets.vVelocity).IsZero(10.0f)) {
		interfaces::engineClient->ExecuteClientCmd("+jump");
	} else {
		interfaces::engineClient->ExecuteClientCmd("-jump");
	}

	Vector hb;
	if (GetHitboxPosition(target, 4, hb)) return;
	AimAt(g_pLocalPlayer->v_Eye, hb, cmd);
	QAngle angles = VectorToQAngle(cmd->viewangles);
	interfaces::engineClient->SetViewAngles(angles);

	if (DistToSqr(target) < (200 * 200) && g_bState == bot_state_t::FOLLOWING_TEAMMATE) {
		interfaces::engineClient->ExecuteClientCmd("-forward");
	}

	if (DistToSqr(target) < (v_iShootDistance->GetInt() * v_iShootDistance->GetInt()) && (g_bState == bot_state_t::FOLLOWING_ENEMY || v_bMediBot->GetBool())) {
		interfaces::engineClient->ExecuteClientCmd("+attack");
	} else {
		interfaces::engineClient->ExecuteClientCmd("-attack");
	}*/
}

void FollowBot::ActuallyCreateMove(CUserCmd* cmd) {
	IClientEntity* tr_follow = interfaces::entityList->GetClientEntity(this->m_hTargetFollowing);
	IClientEntity* tr_heal = interfaces::entityList->GetClientEntity(this->m_hTargetHealing);

	QAngle angles = VectorToQAngle(cmd->viewangles);

	if (v_bMediBot->GetBool()) {
		if (tr_follow) {
			AimAtHitbox(tr_follow, 4, cmd);
			angles = VectorToQAngle(cmd->viewangles);
			g_pLocalPlayer->v_OrigViewangles = cmd->viewangles;
		}

		if (tr_heal) {
			AimAtHitbox(tr_heal, 4, cmd);
			g_pLocalPlayer->bUseSilentAngles = true;
		}
	} else {
		return;
	}
	interfaces::engineClient->SetViewAngles(angles);
}

// TODO optimize, cache or something
bool FollowBot::IsOwner(IClientEntity* ent) {
	if (!ent) return false;
	if (ent->GetClientClass()->m_ClassID != ClassID::CTFPlayer) return false;
	player_info_t info;
	if (!interfaces::engineClient->GetPlayerInfo(ent->entindex(), &info)) return false;
	return (info.friendsID == this->m_nOwnerID);
}

void FollowBot::ResetBotList() {
	this->m_nOtherBots = 0;
}

bool FollowBot::IsFriendlyBot(IClientEntity* ent) {
	if (!ent) return false;
	if (ent->GetClientClass()->m_ClassID != ClassID::CTFPlayer) return false;
	player_info_t info;
	if (!interfaces::engineClient->GetPlayerInfo(ent->entindex(), &info)) return false;
	for (unsigned i = 0; i < this->m_nOtherBots && i < 32; i++) {
		if (info.friendsID == this->m_OtherBots[i]) return true;
	}
	return false;
}

IClientEntity* FollowBot::GetBestHealingTarget() {
	IClientEntity* best = 0;
	int best_score = -65536;

	for (int i = 0; i < 64 && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* cur = interfaces::entityList->GetClientEntity(i);
		if (cur && cur->GetClientClass()->m_ClassID == ClassID::CTFPlayer) {
			if (GetEntityValue<int>(cur, eoffsets.iTeamNum) != g_pLocalPlayer->team) continue;
			if (GetEntityValue<char>(cur, eoffsets.iLifeState)) continue;
			if (cur == g_pLocalPlayer->entity) continue;
			int score = this->GetHealingPriority(cur);
			if (score > best_score && score != 0) {
				best = cur;
				best_score = score;
			}
		}
	}

	return best;
}

bool FollowBot::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;
	Tick(cmd);
	g_nTick++;
	this->ActuallyCreateMove(cmd);
	return false;
}

int FollowBot::GetHealingPriority(IClientEntity* ent) {
	if (!ent) return 0;
	int result = 0;

	int health = GetEntityValue<int>(ent, eoffsets.iHealth);
	int maxhealth = g_pPlayerResource->GetMaxHealth(ent);
	int maxbuffedhealth = maxhealth * 1.5;
	int maxoverheal = maxbuffedhealth - maxhealth;
	int overheal = maxoverheal - (maxbuffedhealth - health);
	float overhealp = ((float)overheal / (float)maxoverheal);
	float healthp = ((float)health / (float)maxhealth);
	if (ent->GetAbsOrigin().DistToSqr(g_pLocalPlayer->v_Eye) > 1000 * 1000) return 0;

	if (this->IsFriendlyBot(ent)) {
		// Friendly bot medics must be constantly at 100%+ hp
		if (healthp < 0.75) {
			result += 60;
		}
		result += (maxbuffedhealth - health);
	} else if (this->IsOwner(ent)) {
		if (overhealp > 0.75) result -= 25;
		result += (maxbuffedhealth - health) * 1.25;
	} else {
		result += (maxhealth - health);
		if (result > 50) result = 50;
	}

	//logging::Info("idx %i score %i | HP %i MH %i MBH %i MO %i O %i", ent->entindex(), result, health, maxhealth, maxbuffedhealth, maxoverheal, overheal);
	return result;
}

void FollowBot::AddBotID(uint32 id) {
	logging::Info("Added bot: %lu", id);
	this->m_OtherBots[this->m_nOtherBots++] = id;
}

void FollowBot::SetOwner(uint32 id) {
	logging::Info("Owner set: %lu", id);
	this->m_nOwnerID = id;
}

// TODO
void CC_BotStatus(const CCommand& args) {
	logging::Info("Bot Status: %i, ID: %i, Search: %i", g_bState, g_nTargetID, nPilotSearch);
}

void CC_AddBotID(const CCommand& args) {
	if (args.ArgC() < 1) return;
	uint32 id = strtoul(args.Arg(1), 0, 0);
	if (!id) return;
	g_phFollowBot->AddBotID(id);
}

void CC_SetOwner(const CCommand& args) {
	if (args.ArgC() < 1) return;
	uint32 id = strtoul(args.Arg(1), 0, 0);
	if (!id) return;
	g_phFollowBot->m_nOwnerID = id;
}

void FollowBot::Create() {
	v_bEnabled = CreateConVar("u_bot_enabled", "0", "Enables followbot");
	v_iForceFollow = CreateConVar("u_bot_force_follow", "-1", "Force follow by UID");
	v_bForceFollowOnly = CreateConVar("u_bot_force_follow_only", "1", "Only follow that player");
	v_iMaxDeltaY = CreateConVar("u_bot_max_height", "450", "Max dY");
	v_iMaxDistance = CreateConVar("u_bot_aim_distance", "1300", "Distance");
	v_iShootDistance = CreateConVar("u_bot_shoot_distance", "800", "Shoot distance");
	v_bChat = CreateConVar("u_bot_chat", "0", "Enable chat");

	v_bMediBot = CreateConVar("u_bot_medic", "1", "Medic mode");
	c_AddBotID = CreateConCommand("u_bot_addbot", CC_AddBotID, "Adds another bot's ID");
	c_SetOwner = CreateConCommand("u_bot_owner", CC_SetOwner, "Set followbot owner");
	c_ResetList = CreateConCommand("u_bot_reset", CC_ResetList, "Resets bot list");
	m_nOwnerID = 0;

	cmd_Status = CreateConCommand("u_bot_status", CC_BotStatus, "Status");
	g_pListener = new MedicCallListener();
	interfaces::eventManager->AddListener(g_pListener, "player_calledformedic", false);
}

void FollowBot::Destroy() {
	interfaces::eventManager->RemoveListener(g_pListener);
}

void FollowBot::PaintTraverse(void*, unsigned int, bool, bool) {}

FollowBot* g_phFollowBot = 0;
