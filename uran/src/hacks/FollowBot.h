/*
 * HPyroBot.h
 *
 *  Created on: Oct 30, 2016
 *      Author: nullifiedcat
 */

#ifndef HPYROBOT_H_
#define HPYROBOT_H_

#include "IHack.h"

class ipcctl;
class IClientEntity;

void CC_ResetList(const CCommand& args);
void CC_AddBotID(const CCommand& args);
void CC_BotStatus(const CCommand& args);
void CC_SetOwner(const CCommand& args);

enum botpackage {
	BOT_FOLLOW = 0,
	BOT_MEDIC,
	BOT_SNIPER,
	BOT_DUMMY
};

class FollowBot : public IHack {
public:
	DECLARE_HACK_METHODS();
	FollowBot();
	~FollowBot();
	void ProcessEntity(IClientEntity* entity, bool enemy);
	void Tick(CUserCmd*);
	int ShouldNotTarget(IClientEntity* ent, bool notrace);

	bool ShouldPopUber(bool force);
	void ActuallyCreateMove(CUserCmd*);
	IClientEntity* GetBestHealingTarget();
	int GetHealingPriority(IClientEntity* ent);
	bool IsFriendlyBot(IClientEntity* ent);
	bool IsOwner(IClientEntity* ent);
	void ResetBotList();
	void AddBotID(uint32 id);
	void SetOwner(uint32 id);
	void UpdateOwner();

	int m_hTargetFollowing;
	int m_hTargetHealing;

	ConCommand* cmd_Status;

	ConVar* v_bEnabled;
	int m_iForceHealTicks;
	int m_iShouldUbercharge;
	ConVar* v_iBotPackage;
	ConCommand* c_AddBotID;
	ConCommand* c_SetOwner;
	ConCommand* c_ResetList;
	ConCommand* c_HealOwner;
	ConCommand* c_BotCommand;
	ConCommand* c_IPCList;
	uint32 m_nOwnerID;
	uint32 m_OtherBots[32];
	uint32 m_nOtherBots;
	ipcctl* m_pIPC;
	unsigned last_command_global;
	unsigned last_command_local;
};

DECLARE_HACK_SINGLETON(FollowBot)

#endif /* HPYROBOT_H_ */
