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

class Triggerbot : public IHack {
public:
	DECLARE_HACK_METHODS();
	Triggerbot();
	~Triggerbot();
	ConVar* v_bEnabled;
	ConVar* v_bIgnoreCloak;
	ConVar* v_bZoomedOnly;
	ConVar* v_iHitbox;
	ConVar* v_bBodyshot;
	ConVar* v_bFinishingHit;
	ConVar* v_iMinRange;
	ConVar* v_bBuildings;
	ConVar* v_bIgnoreVaccinator;
};

DECLARE_HACK_SINGLETON(Triggerbot);

#endif /* HTRIGGER_H_ */
