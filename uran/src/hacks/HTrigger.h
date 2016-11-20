/*
 * HTrigger.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HTRIGGER_H_
#define HTRIGGER_H_

#include "IHack.h"

class ConVar;

class HTrigger : public IHack {
public:
	bool CreateMove(void*, float, CUserCmd*);
	void PaintTraverse(void*, unsigned int, bool, bool);
	void Create();
	void Destroy();
	ConVar* v_bEnabled;
	ConVar* v_bIgnoreCloak;
	ConVar* v_bZoomedOnly;
	ConVar* v_iHitbox;
	ConVar* v_bBodyshot;
	ConVar* v_bFinishingHit;
	ConVar* v_iMinRange;
	ConVar* v_bBuildings;
};

extern HTrigger* g_phTrigger;

#endif /* HTRIGGER_H_ */
