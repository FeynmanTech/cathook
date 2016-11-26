/*
 * HPyroBot.cpp
 *
 *  Created on: Oct 30, 2016
 *      Author: nullifiedcat
 */

#include "FollowBot.h"

#include <signal.h>

#include "../interfaces.h"
#include "../entity.h"
#include "../helpers.h"
#include "../localplayer.h"
#include "../logging.h"
#include "../usercmd.h"
#include "../playerresource.h"
#include "../sdk/in_buttons.h"
#include "../followbot/ipcctl.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <icliententitylist.h>
#include <client_class.h>
#include <cdll_int.h>
#include <igameevents.h>

unsigned long g_nTick = 0;

// TODO
bool FollowBot::ShouldPopUber(bool force) {
	int health_my = g_pLocalPlayer->health;
	int health_tr = GetEntityValue<int>(interfaces::entityList->GetClientEntity(this->m_hTargetHealing), eoffsets.iHealth);
	if (health_my < 30) return true;
	bool other_bots_have_uber = false;
	for (int i = 0; i < 64 && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
		if (ent == g_pLocalPlayer->entity) continue;
		if (IsFriendlyBot(ent)) {
			if (GetEntityValue<char>(ent, eoffsets.iLifeState)) continue;
			IClientEntity* medigun;
			// TODO
		}
	}
	return false;
}

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

// TODO
void FollowBot::ProcessEntity(IClientEntity* entity, bool enemy) {
	return;
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

// TODO
int FollowBot::ShouldNotTarget(IClientEntity* ent, bool notrace) {
	if (!ent || ent->IsDormant()) return 1;
	if (ent->GetClientClass()->m_ClassID != 241) return 2;
	if (GetEntityValue<char>(ent, eoffsets.iLifeState)) return 3;
	bool enemy = GetEntityValue<int>(ent, eoffsets.iTeamNum) != g_pLocalPlayer->team;
	if (enemy) return 4;

	if (!this->IsOwner(ent)) {
		return 7;
	}

	if (!notrace) {
		bool a = false;
		for (int i = 0; i < 17; i++) {
			if (IsEntityVisible(ent, i)) a = true;
		}
		if (!a) return 6;
	}

	return 0;
}

void FollowBot::Tick(CUserCmd* cmd) {
	if (!g_pLocalPlayer->entity || g_pLocalPlayer->entity->IsDormant()) return;
	if (g_pLocalPlayer->life_state) return;

	IClientEntity* owner_entity = 0;
	for (int i = 0; i < 64 && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		if (IsOwner(interfaces::entityList->GetClientEntity(i))) {
			m_hTargetFollowing = i;
			owner_entity = interfaces::entityList->GetClientEntity(i);
		}
	}

	switch (v_iBotPackage->GetInt()) {
	case botpackage::BOT_FOLLOW: {

		break;
	}
	case botpackage::BOT_MEDIC: {
		cmd->buttons |= IN_ATTACK;
		if ((g_nTick % 100) == 0) {
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
		break;
	}
	case botpackage::BOT_SNIPER: {
		if (!owner_entity) break;
		//bool owner_zoomed = (GetEntityValue<int>(owner_entity, eoffsets.iCond) & cond::zoomed);
		//
		if (IClientEntity* weapon = interfaces::entityList->GetClientEntity(GetEntityValue<int>(owner_entity, eoffsets.hActiveWeapon) & 0xFFF)) {
			if (weapon) {
				if (weapon->GetClientClass()->m_ClassID == ClassID::CTFSniperRifle || weapon->GetClientClass()->m_ClassID == ClassID::CTFSniperRifle) {
					bool bot_zoomed = (GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.iCond) & cond::zoomed);
					if (!bot_zoomed) {
						cmd->buttons |= IN_ATTACK2;
					}
				} else {
					bool bot_zoomed = (GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.iCond) & cond::zoomed);
					if (bot_zoomed) {
						cmd->buttons |= IN_ATTACK2;
					}
				}
			}
		}
		break;
	}
	}

	if (v_iBotPackage->GetInt() == botpackage::BOT_MEDIC) {
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
	}

	if (owner_entity && (0 == (g_nTick % 20))) {
		static bool forward = false;
		static bool jump = false;
		if (!jump && GetEntityValue<Vector>(g_pLocalPlayer->entity, eoffsets.vVelocity).IsZero(10.0f) && !(GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.iCond) & cond::zoomed)) {
			interfaces::engineClient->ExecuteClientCmd("+jump");
			jump = true;
		} else if (jump) {
			interfaces::engineClient->ExecuteClientCmd("-jump");
			jump = false;
		}

		if (forward && DistToSqr(owner_entity) < (60 * 60)) {
			interfaces::engineClient->ExecuteClientCmd("-forward");
			forward = false;
		} else if (!forward) {
			interfaces::engineClient->ExecuteClientCmd("+forward");
			forward = true;
		}
	}

	return;
}

void FollowBot::ActuallyCreateMove(CUserCmd* cmd) {
	IClientEntity* tr_follow = interfaces::entityList->GetClientEntity(this->m_hTargetFollowing);
	QAngle angles = VectorToQAngle(cmd->viewangles);

	if (tr_follow) {
		AimAtHitbox(tr_follow, 4, cmd);
		angles = VectorToQAngle(cmd->viewangles);
		g_pLocalPlayer->v_OrigViewangles = cmd->viewangles;
	}

	if (v_iBotPackage->GetInt() == botpackage::BOT_MEDIC) {
		IClientEntity* tr_heal = interfaces::entityList->GetClientEntity(this->m_hTargetHealing);
		if (tr_heal) {
			AimAtHitbox(tr_heal, 4, cmd);
			g_pLocalPlayer->bUseSilentAngles = true;
		}
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

	/*ipc_bot_seg* seg_g = m_pIPC->GetBotCommand(0);
	ipc_bot_seg* seg_l = m_pIPC->GetBotCommand(m_pIPC->bot_id);

	if (seg_g->command_number > this->last_command_global) {
		interfaces::engineClient->ExecuteClientCmd(seg_g->command_buffer);
		this->last_command_global = seg_g->command_number;
	}
	if (seg_l->command_number > this->last_command_global) {
		interfaces::engineClient->ExecuteClientCmd(seg_l->command_buffer);
		this->last_command_global = seg_l->command_number;
	}*/

	Tick(cmd);
	g_nTick++;
	this->ActuallyCreateMove(cmd);
	return false;
}

int FollowBot::GetHealingPriority(IClientEntity* ent) {
	if (!ent) return 0;
	int result = 0;

	if (GetEntityValue<char>(ent, eoffsets.iLifeState)) return 0;
	if (GetEntityValue<int>(ent, eoffsets.iTeamNum) != g_pLocalPlayer->team) return 0;
	if (!IsEntityVisible(ent, 4)) return 0;

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
	//logging::Info("Bot Status: %i, ID: %i, Search: %i", g_bState, g_nTargetID, nPilotSearch);
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

void CC_BotCommand(const CCommand& args) {
	if (args.ArgC() < 2) return;
	int bot_id = strtol(args.Arg(1), 0, 0);
	if (!bot_id) {
		logging::Info("Executing command `%s` on EACH bot.", args.Arg(2));
	} else {
		logging::Info("Executing command `%s` on bot %i.", args.Arg(2), bot_id);
	}
	g_phFollowBot->m_pIPC->WriteBotCommand(bot_id, (char*)args.Arg(2));
}

void CC_IPCList(const CCommand& args) {
	for (int i = 1; i < MAX_SEGS; i++) {
		ipc_bot_seg seg = g_phFollowBot->m_pIPC->mem->segments[i];
		if (seg.initialized && (time(0) - seg.last_access_time < ACCESS_TIMEOUT)) {
			if (kill(seg.owner_pid, 0) == -1) {
				if (errno == ESRCH) continue;
			}
			logging::Info("[%i]: pid=%i name=`%s`", i, seg.owner_pid, seg.name);
		}
	}
}

void FollowBot::Create() {
	v_bEnabled = CreateConVar("u_bot_enabled", "0", "Enables followbot");
	v_iBotPackage = CreateConVar("u_bot_ai", "1", "AI Package (FOLLOW, MEDIC, SNIPER)");
	c_AddBotID = CreateConCommand("u_bot_addbot", CC_AddBotID, "Adds another bot's ID");
	c_SetOwner = CreateConCommand("u_bot_owner", CC_SetOwner, "Set followbot owner");
	c_ResetList = CreateConCommand("u_bot_reset", CC_ResetList, "Resets bot list");
	c_BotCommand = CreateConCommand("u_bot_command", CC_BotCommand, "Sends bot commands");
	c_IPCList = CreateConCommand("u_bot_ipclist", CC_IPCList, "Lists IPC status");

	m_nOwnerID = 0;

	this->last_command_global = 0;
	this->last_command_local = 0;

	cmd_Status = CreateConCommand("u_bot_status", CC_BotStatus, "Status");
	g_pListener = new MedicCallListener();
	interfaces::eventManager->AddListener(g_pListener, "player_calledformedic", false);

	logging::Info("Creating shared memory for bot");
	m_pIPC = new ipcctl();
	if (m_pIPC->Init()) {
		logging::Info("Successfully allocated shared memory!");
		logging::Info("Bot ID: %i", m_pIPC->bot_id);
	} else {
		logging::Info("Failed to allocate shared memory for bot.");
	}
}

void FollowBot::Destroy() {
	interfaces::eventManager->RemoveListener(g_pListener);
	m_pIPC->Detach();
}

void FollowBot::PaintTraverse(void*, unsigned int, bool, bool) {}

FollowBot* g_phFollowBot = 0;
