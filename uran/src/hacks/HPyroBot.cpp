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

unsigned long g_nTick = 0;
int g_nTargetID = 0;
//IClientEntity* eTarget;
bool bFollowingTeammate;
unsigned long nPilotSearch = 0;

void SetTarget(IClientEntity* ent) {
	if (!ent) return;
	g_nTargetID = ent->entindex();
}

IClientEntity* GetTarget() {
	return interfaces::entityList->GetClientEntity(g_nTargetID);
}

void HPyroBot::ProcessEntity(IClientEntity* entity, bool enemy) {
	IClientEntity* target = GetTarget();

	if (!target) {
		if (!enemy && v_iForceFollow->GetInt() != -1 && v_bForceFollowOnly->GetBool() && entity->entindex() != v_iForceFollow->GetInt()) return;
		target = entity;
		bFollowingTeammate = !enemy;
	}
	if (enemy) {
		if (!bFollowingTeammate) {
			if (DistToSqr(entity) <= DistToSqr(target)) {
				target = entity;
			}
		} else {
			target = entity;
		}
		bFollowingTeammate = false;
	} else {
		if (nPilotSearch < 100) return;
		if (v_iForceFollow->GetInt() != -1) {
			if (bFollowingTeammate && entity->entindex() == v_iForceFollow->GetInt()) {
				target = entity;
			} else {
				return;
			}
		}
		if (GetEntityValue<Vector>(entity, eoffsets.vVelocity).IsZero(1.0f)) return;
		if (bFollowingTeammate && DistToSqr(entity) < DistToSqr(target)) {
			target = entity;
		}
	}
	SetTarget(target);
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

bool HPyroBot::ShouldTarget(IClientEntity* ent) {
	if (!ent || ent->IsDormant()) return false;
	if (ent->GetClientClass()->m_ClassID != 241) return false;
	if (GetEntityValue<char>(ent, eoffsets.iLifeState)) return false;
	bool enemy = GetEntityValue<int>(ent, eoffsets.iTeamNum) != g_pLocalPlayer->team;
	if (enemy) {
		if (IsPlayerInvulnerable(ent)) return false;
	}
	if (DistToSqr(ent) > v_iMaxDistance->GetInt() * v_iMaxDistance->GetInt()) return false;
	if (!IsEntityVisible(ent, 4)) return false;
	return true;
}

void HPyroBot::Tick(CUserCmd* cmd) {
	if (!g_pLocalPlayer->entity || g_pLocalPlayer->entity->IsDormant()) return;
	if (g_pLocalPlayer->life_state) return;
	IClientEntity* target = GetTarget();
	IClientEntity* target_old = target;
	if (!ShouldTarget(target)) {
		SetTarget(0);
		target = 0;
	}
	if (!target) {
		nPilotSearch++;
		/*if (nPilotSearch >= 100) {
			interfaces::engineClient->ExecuteClientCmd("-forward");
			interfaces::engineClient->ExecuteClientCmd("-attack");
		}*/
	} else {
		nPilotSearch = 0;
		interfaces::engineClient->ExecuteClientCmd("+forward");
	}
	if (g_nTick % 150 == 0 || !target) {
		SetTarget(0);
		for (int i = 0; i < 64 && i < interfaces::entityList->GetMaxEntities(); i++) {
			IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
			if (!ShouldTarget(ent)) continue;
			bool enemy = GetEntityValue<int>(ent, eoffsets.iTeamNum) != g_pLocalPlayer->team;
			ProcessEntity(ent, enemy);
		}
	}
	target = GetTarget();
	if (!target) {
		if (target_old != 0) {
			logging::Info("Target lost!");
			if (v_bChat->GetBool()) {
				interfaces::engineClient->ServerCmd("say Target lost!");
			}
		}
		return;
	}
	if (target != target_old) {
		player_info_s info;
		interfaces::engineClient->GetPlayerInfo(target->entindex(), &info);
		logging::Info("Following %s", info.name);
		if (v_bChat->GetBool()) {
			char* cmd = new char[256];
			if (bFollowingTeammate)
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
	if (DistToSqr(target) < 200 * 200 && bFollowingTeammate) {
		interfaces::engineClient->ExecuteClientCmd("-forward");
	}
	if (DistToSqr(target) < (v_iShootDistance->GetInt() * v_iShootDistance->GetInt()) && !bFollowingTeammate) {
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

void HPyroBot::Create() {
	logging::Info("Creating PyroBot");
	v_bEnabled = CreateConVar("u_pyrobot_enabled", "0", "Enables WM1 bot");
	v_iForceFollow = CreateConVar("u_pyrobot_force_follow", "-1", "Force follow a teammate");
	v_bForceFollowOnly = CreateConVar("u_pyrobot_force_follow_only", "1", "only follow force");
	v_iMaxDeltaY = CreateConVar("u_pyrobot_max_height", "450", "Max dY");
	v_iMaxDistance = CreateConVar("u_pyrobot_aim_distance", "1300", "Distance");
	v_iShootDistance = CreateConVar("u_pyrobot_shoot_distance", "800", "Shoot distance");
	v_bChat = CreateConVar("u_pyrobot_chat", "0", "Enable chat");
}

void HPyroBot::Destroy() {}
void HPyroBot::PaintTraverse(void*, unsigned int, bool, bool) {}
