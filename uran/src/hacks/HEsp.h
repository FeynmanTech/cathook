/*
 * HEsp.h
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#ifndef HESP_H_
#define HESP_H_

#include "IHack.h"

class IClientEntity;
class ConVar;

class HEsp : public IHack {
public:
	bool CreateMove(void*, float, CUserCmd*);
	void PaintTraverse(void*, unsigned int, bool, bool);
	void Create();
	void Destroy();
	void ProcessEntity(IClientEntity* ent, int idx);
	ConVar* v_bEnabled;
	ConVar* v_bBoxESP;
	ConVar* v_bEntityESP;
	ConVar* v_bTeammates;
	ConVar* v_bItemESP;
	ConVar* v_bTeammatePowerup;
	ConVar* v_bShowHead;
	ConVar* v_bShowTargetScore;
	ConVar* v_bShowEntityID;
	//ConVar* v_bModelInfo;
};

#endif /* HESP_H_ */
