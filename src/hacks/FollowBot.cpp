/*
 * HPyroBot.cpp
 *
 *  Created on: Oct 30, 2016
 *      Author: nullifiedcat
 */
/*
#include "FollowBot.h"

#include <signal.h>

#include "../common.h"
#include "../ipc/ipcctl.h"
#include "../sdk.h"



DEFINE_HACK_SINGLETON(FollowBot);

unsigned long g_nTick = 0;

const char* FollowBot::GetName() {
	return "FOLLOWBOT";
}

// TODO
bool FollowBot::ShouldPopUber(bool force) {
	int health_my = g_pLocalPlayer->health;
	//int health_tr = NET_INT(ENTITY(this->m_hTargetHealing), eoffsets.iHealth);
	if (health_my < 30) return true;
	//bool other_bots_have_uber = false;
	for (int i = 0; i < 64 && i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (ent == g_pLocalPlayer->entity) continue;
		if (IsFriendlyBot(ent)) {
			if (CE_BYTE(ent, netvar.iLifeState)) continue;
			//IClientEntity* medigun;
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
void FollowBot::ProcessEntity(CachedEntity* entity, bool enemy) {
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
 *//*

// TODO
int FollowBot::ShouldNotTarget(CachedEntity* ent, bool notrace) {
	if (CE_BAD(ent)) return 1;
	if (ent->m_Type != ENTITY_PLAYER) return 2;
	if (!ent->m_bAlivePlayer) return 3;
	if (ent->m_bEnemy) return 4;

	// TODO temporary! W+m1 bot should be back!
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
	if (CE_BAD(g_pLocalPlayer->entity)) return;
	if (g_pLocalPlayer->life_state) return;

	CachedEntity* owner_entity = 0;
	for (int i = 0; i < 64 && i < HIGHEST_ENTITY; i++) {
		if (IsOwner(ENTITY(i))) {
			m_hTargetFollowing = i;
			owner_entity = ENTITY(i);
		}
	}
	if (CE_BAD(owner_entity)) return;
	if (m_iForceHealTicks && m_iForceHealTicks < 20) {
		m_iForceHealTicks++;
		cmd->buttons |= IN_ATTACK;
	} else m_iForceHealTicks = 0;
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
		//bool owner_zoomed = (NET_INT(owner_entity, eoffsets.iCond) & cond::zoomed);
		//
		CachedEntity* owner_weapon = ENTITY(CE_INT(owner_entity, netvar.hActiveWeapon) & 0xFFF);
		if (CE_GOOD(owner_weapon)) {
			if (owner_weapon->m_iClassID == g_pClassID->CTFSniperRifle || owner_weapon->m_iClassID == g_pClassID->CTFSniperRifle) {
				if (!g_pLocalPlayer->bZoomed) {
					cmd->buttons |= IN_ATTACK2;
				}
			} else {
				if (g_pLocalPlayer->bZoomed) {
					cmd->buttons |= IN_ATTACK2;
				}
			}
		}
		break;
	}
	}
	if (v_iBotPackage->GetInt() == botpackage::BOT_DUMMY) return;


	if (v_iBotPackage->GetInt() == botpackage::BOT_MEDIC) {
		CachedEntity* healtr = this->GetBestHealingTarget();
		m_hTargetHealing = (healtr ? healtr->m_IDX : 0);
		if (healtr) {
			if (CE_INT(healtr, netvar.iHealth) < 35 && !CE_BYTE(healtr, netvar.iLifeState)) {
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
		if (!jump && CE_VECTOR(g_pLocalPlayer->entity, netvar.vVelocity).IsZero(10.0f) && !g_pLocalPlayer->bZoomed) {
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
	CachedEntity* tr_follow = ENTITY(this->m_hTargetFollowing);
	QAngle angles = VectorToQAngle(cmd->viewangles);
	if (CE_GOOD(tr_follow)) {
		AimAtHitbox(tr_follow, 4, cmd);
		angles = VectorToQAngle(cmd->viewangles);
		g_pLocalPlayer->v_OrigViewangles = cmd->viewangles;
	}

	if (v_iBotPackage->GetInt() == botpackage::BOT_MEDIC) {
		CachedEntity* tr_heal = ENTITY(this->m_hTargetHealing);
		if (CE_GOOD(tr_heal)) {
			AimAtHitbox(tr_heal, 4, cmd);
			g_pLocalPlayer->bUseSilentAngles = true;
		}
	}
	interfaces::engineClient->SetViewAngles(angles);
}

// TODO optimize, cache or something
bool FollowBot::IsOwner(CachedEntity* ent) {
	if (CE_BAD(ent)) return false;
	return (ent->m_pPlayerInfo && ent->m_pPlayerInfo->friendsID == this->m_nOwnerID);
}

void FollowBot::ResetBotList() {
	this->m_nOtherBots = 0;
}

bool FollowBot::IsFriendlyBot(CachedEntity* ent) {
	if (CE_BAD(ent)) return false;
	if (!ent->m_pPlayerInfo) return false;
	for (unsigned i = 0; i < this->m_nOtherBots && i < 32; i++) {
		if (ent->m_pPlayerInfo->friendsID == this->m_OtherBots[i]) return true;
	}
	return false;
}

CachedEntity* FollowBot::GetBestHealingTarget() {
	CachedEntity* best = 0;
	int best_score = -65536;

	for (int i = 0; i < 64 && i < HIGHEST_ENTITY; i++) {
		CachedEntity* cur = ENTITY(i);
		if (CE_BAD(cur)) continue;
		if (cur->m_Type == ENTITY_PLAYER) {
			if (cur->m_bEnemy) continue;
			if (!cur->m_bAlivePlayer) continue;
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

int FollowBot::GetHealingPriority(CachedEntity* ent) {
	if (!ent) return 0;
	int result = 0;

	if (ent->m_bEnemy) return 0;
	if (!ent->m_bAlivePlayer) return 0;
	if (!IsEntityVisible(ent, 4)) return 0;

	int health = CE_INT(ent, netvar.iHealth);
	int maxhealth = g_pPlayerResource->GetMaxHealth(ent);
	int maxbuffedhealth = maxhealth * 1.5;
	int maxoverheal = maxbuffedhealth - maxhealth;
	int overheal = maxoverheal - (maxbuffedhealth - health);
	float overhealp = ((float)overheal / (float)maxoverheal);
	float healthp = ((float)health / (float)maxhealth);
	if (ent->m_flDistance > 1000) return 0;

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

void CC_HealOwner(const CCommand& args) {
	g_phFollowBot->m_hTargetHealing = g_phFollowBot->m_nOwnerID;
	g_phFollowBot->m_iForceHealTicks = 1;
}

void CC_BotCommand(const CCommand& args) {
	if (args.ArgC() < 2) return;
	int bot_id = strtol(args.Arg(1), 0, 0);
	char* buf = new char[256];
	for (int i = 2; i < args.ArgC(); i++) {
		strcat(buf, strfmt("%s ", args.Arg(i)));
	}
	if (!bot_id) {
		logging::Info("Executing command `%s` on EACH bot.", buf);
	} else {
		logging::Info("Executing command `%s` on bot %i.", buf, bot_id);
	}
	g_phFollowBot->m_pIPC->SetCommand(bot_id, (char*)buf);
	delete [] buf;
}

void CC_IPCList(const CCommand& args) {
	for (int i = 1; i < MAX_SEGS; i++) {
		ipc_client_seg seg = g_phFollowBot->m_pIPC->mem->segments[i];
		if (seg.initialized && (time(0) - seg.last_access_time < ACCESS_TIMEOUT)) {
			if (kill(seg.owner_pid, 0) == -1) {
				if (errno == ESRCH) continue;
			}
			logging::Info("[%i]: pid=%i name=`%s`", i, seg.owner_pid, seg.name);
		}
	}
}

FollowBot::FollowBot() {
	v_bEnabled = CreateConVar(CON_PREFIX "bot_enabled", "0", "Enables followbot");
	v_iBotPackage = CreateConVar(CON_PREFIX "bot_ai", "0", "AI Package (FOLLOW, MEDIC, SNIPER)");
	c_AddBotID = CreateConCommand(CON_PREFIX "bot_addbot", CC_AddBotID, "Adds another bot's ID");
	c_SetOwner = CreateConCommand(CON_PREFIX "bot_owner", CC_SetOwner, "Set followbot owner");
	c_ResetList = CreateConCommand(CON_PREFIX "bot_reset", CC_ResetList, "Resets bot list");
	c_BotCommand = CreateConCommand(CON_PREFIX "bot_command", CC_BotCommand, "Sends bot commands");
	c_IPCList = CreateConCommand(CON_PREFIX "bot_ipclist", CC_IPCList, "Lists IPC status");
	c_HealOwner = CreateConCommand(CON_PREFIX "bot_healowner", CC_HealOwner, "Heals owner");
	m_nOwnerID = 0;

	this->last_command_global = 0;
	this->last_command_local = 0;

	m_hTargetFollowing = 0;
	m_hTargetHealing = 0;

	cmd_Status = CreateConCommand(CON_PREFIX "bot_status", CC_BotStatus, "Status");
	g_pListener = new MedicCallListener();
	interfaces::eventManager->AddListener(g_pListener, "player_calledformedic", false);

	logging::Info("Creating shared memory for bot");
	m_pIPC = new ipcctl();
	if (m_pIPC->Init()) {
		logging::Info("Successfully allocated shared memory!");
		logging::Info("Bot ID: %i", m_pIPC->client_id);
	} else {
		logging::Info("Failed to allocate shared memory for bot.");
	}
}

FollowBot::~FollowBot() {
	interfaces::eventManager->RemoveListener(g_pListener);
	m_pIPC->Detach();
}

void FollowBot::PaintTraverse(void*, unsigned int, bool, bool) {}

void FollowBot::LevelInit(const char*) {}
void FollowBot::LevelShutdown() {}
*/
