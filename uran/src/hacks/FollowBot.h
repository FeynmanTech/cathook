/*
 * HPyroBot.h
 *
 *  Created on: Oct 30, 2016
 *      Author: nullifiedcat
 */

#ifndef HPYROBOT_H_
#define HPYROBOT_H_

#include "IHack.h"

class IClientEntity;

void CC_ResetList(const CCommand& args);
void CC_AddBotID(const CCommand& args);
void CC_BotStatus(const CCommand& args);
void CC_SetOwner(const CCommand& args);

class FollowBot : public IHack {
public:
	void Create();
	bool CreateMove(void*, float, CUserCmd*);
	void Destroy();
	void PaintTraverse(void*, unsigned int, bool, bool);
	void ProcessEntity(IClientEntity* entity, bool enemy);
	void Tick(CUserCmd*);
	int ShouldNotTarget(IClientEntity* ent, bool notrace);

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
	ConVar* v_iForceFollow;
	ConVar* v_bForceFollowOnly;
	ConVar* v_iMaxDistance;
	ConVar* v_iShootDistance;
	ConVar* v_iMaxDeltaY;
	ConVar* v_bChat;

	int m_iShouldUbercharge;
	ConVar* v_bMediBot;
	ConCommand* c_AddBotID;
	ConCommand* c_SetOwner;
	ConCommand* c_ResetList;
	uint32 m_nOwnerID;
	uint32 m_OtherBots[32];
	uint32 m_nOtherBots;
};

extern FollowBot* g_phFollowBot;

#endif /* HPYROBOT_H_ */
