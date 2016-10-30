/*
 * HPyroBot.h
 *
 *  Created on: Oct 30, 2016
 *      Author: nullifiedcat
 */

#ifndef HPYROBOT_H_
#define HPYROBOT_H_

#include "IHack.h"

class ConVar;
class IClientEntity;

class HPyroBot : public IHack {
public:
	void Create();
	bool CreateMove(void*, float, CUserCmd*);
	void Destroy();
	void PaintTraverse(void*, unsigned int, bool, bool);
	void ProcessEntity(IClientEntity* entity, bool enemy);
	void Tick(CUserCmd*);
	bool ShouldTarget(IClientEntity* ent);
	ConVar* v_bEnabled;
	ConVar* v_iForceFollow;
	ConVar* v_bForceFollowOnly;
	ConVar* v_iMaxDistance;
	ConVar* v_iShootDistance;
	ConVar* v_iMaxDeltaY;
	ConVar* v_bChat;
};

#endif /* HPYROBOT_H_ */
