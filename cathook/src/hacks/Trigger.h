/*
 * HTrigger.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HTRIGGER_H_
#define HTRIGGER_H_

#include "IHack.h"

class CatVar;

class Triggerbot : public IHack {
public:
	Triggerbot();
	~Triggerbot();

	HACK_OVERRIDES_PROCESSUSERCMD;

	CatVar* v_bEnabled;
	CatVar* v_bRespectCloak;
	CatVar* v_bZoomedOnly;
	CatVar* v_iHitbox;
	CatVar* v_bBodyshot;
	CatVar* v_bFinishingHit;
	CatVar* v_iMaxRange;
	CatVar* v_bBuildings;
	CatVar* v_bIgnoreVaccinator;
	CatVar* v_bAmbassadorCharge;
	CatVar* v_bImproveAccuracy;
};

DECLARE_HACK_SINGLETON(Triggerbot);

#endif /* HTRIGGER_H_ */
