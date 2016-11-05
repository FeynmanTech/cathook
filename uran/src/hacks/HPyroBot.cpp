/*
 * HPyroBot.cpp
 *
 *  Created on: Oct 30, 2016
 *      Author: nullifiedcat
 */

#include "HPyroBot.h"

#include "../interfaces.h"
#include "../entity.h"
#include "../helpers.h"
#include "../localplayer.h"
#include "../logging.h"
#include "../usercmd.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <icliententitylist.h>
#include <client_class.h>
#include <cdll_int.h>

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

void HPyroBot::ProcessEntity(IClientEntity* entity, bool enemy) {
	IClientEntity* target = GetTarget();

	if (!target) {
		if (!enemy) {
			if (v_iForceFollow->GetInt() != -1) {
				if (v_bForceFollowOnly->GetBool() && (v_iForceFollow->GetInt() != entity->entindex())) {
					return;
				}
			}
		}
		target = entity;
		SetTarget(target);
	}

	if (enemy) {
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

void AimAt(IClientEntity* entity, CUserCmd* cmd) {
	Vector hit;
	Vector angles;
	if (GetHitboxPosition(entity, 4, hit)) return;
	Vector tr = (hit - g_pLocalPlayer->v_Eye);
	fVectorAngles(tr, angles);
	fClampAngle(angles);
	cmd->viewangles = angles;
	QAngle a;
	a.x = angles.x;
	a.y = angles.y;
	a.z = angles.z;
	interfaces::engineClient->SetViewAngles(a);
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

int HPyroBot::ShouldTarget(IClientEntity* ent, bool notrace) {
	if (!ent || ent->IsDormant()) return 1;
	if (ent->GetClientClass()->m_ClassID != 241) return 2;
	if (GetEntityValue<char>(ent, eoffsets.iLifeState)) return 3;
	bool enemy = GetEntityValue<int>(ent, eoffsets.iTeamNum) != g_pLocalPlayer->team;
	if (enemy) {
		if (IsPlayerInvulnerable(ent)) return 4;
	}
	if (DistToSqr(ent) > (v_iMaxDistance->GetInt() * v_iMaxDistance->GetInt())) return 5;
	if (!notrace) {
		bool a = false;
		for (int i = 0; i < 17; i++) {
			if (IsEntityVisible(ent, i)) a = true;
		}
		if (!a) return 6;
	}

	if (abs(ent->GetAbsOrigin().z - g_pLocalPlayer->v_Origin.z) > v_iMaxDeltaY->GetInt()) return 7;
	return 0;
}

void HPyroBot::Tick(CUserCmd* cmd) {
	if (!g_pLocalPlayer->entity || g_pLocalPlayer->entity->IsDormant()) return;
	if (g_pLocalPlayer->life_state) return;

	IClientEntity* target = GetTarget();
	IClientEntity* target_old = target;

	//bool target_lost = (!target || ShouldTarget(target) || (g_bState != bot_state_t::FOLLOWING_ENEMY && g_bState != bot_state_t::FOLLOWING_TEAMMATE));
	bool target_lost = (!target || ShouldTarget(target, true) || (g_bState != bot_state_t::FOLLOWING_ENEMY && g_bState != bot_state_t::FOLLOWING_TEAMMATE));

	if (target_lost && g_bState != bot_state_t::PILOT_SEARCH) logging::Info("Target lost! Target: %i, State: %i, ShouldTarget: %i", target, g_bState, ShouldTarget(target, true));

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
			if (ShouldTarget(ent, false)) continue;
			bool enemy = GetEntityValue<int>(ent, eoffsets.iTeamNum) != g_pLocalPlayer->team;
			ProcessEntity(ent, enemy);
		}
	}

	target = GetTarget();
	target_lost = (!target || ShouldTarget(target, true) || g_bState == bot_state_t::IDLE || g_bState == bot_state_t::PILOT_SEARCH || g_bState == bot_state_t::TARGET_LOST);

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

	AimAt(target, cmd);

	if (DistToSqr(target) < (200 * 200) && g_bState == bot_state_t::FOLLOWING_TEAMMATE) {
		interfaces::engineClient->ExecuteClientCmd("-forward");
	}

	if (DistToSqr(target) < (v_iShootDistance->GetInt() * v_iShootDistance->GetInt()) && g_bState == bot_state_t::FOLLOWING_ENEMY) {
		interfaces::engineClient->ExecuteClientCmd("+attack");
	} else {
		interfaces::engineClient->ExecuteClientCmd("-attack");
	}
}

bool HPyroBot::CreateMove(void*, float, CUserCmd* cmd) {
	if (!v_bEnabled->GetBool()) return true;
	Tick(cmd);
	g_nTick++;
	return true;
}

void CC_Status(const CCommand& args) {
	logging::Info("W+M1 State: %i, ID: %i, Search: %i", g_bState, g_nTargetID, nPilotSearch);
}

void HPyroBot::Create() {
	logging::Info("Creating PyroBot");
	v_bEnabled = CreateConVar("u_pyrobot_enabled", "0", "Enables WM1 bot");
	v_iForceFollow = CreateConVar("u_pyrobot_force_follow", "-1", "Force follow a teammate");
	v_bForceFollowOnly = CreateConVar("u_pyrobot_force_follow_only", "1", "only follow force");
	v_iMaxDeltaY = CreateConVar("u_pyrobot_max_height", "450", "Max dY");
	v_iMaxDistance = CreateConVar("u_pyrobot_aim_distance", "1300", "Distance");
	v_iShootDistance = CreateConVar("u_pyrobot_shoot_distance", "800", "Shoot distance");
	v_bChat = CreateConVar("u_pyrobot_chat", "0", "Enable chat");
	cmd_Status = CreateConCommand("u_pyrobot_status", CC_Status, "Status");
}

void HPyroBot::Destroy() {}
void HPyroBot::PaintTraverse(void*, unsigned int, bool, bool) {}
