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
	DECLARE_HACK_METHODS();
	Triggerbot();
	~Triggerbot();
	CatVar* v_bEnabled;
	CatVar* v_bIgnoreCloak;
	CatVar* v_bZoomedOnly;
	CatVar* v_iHitbox;
	CatVar* v_bBodyshot;
	CatVar* v_bFinishingHit;
	CatVar* v_iMinRange;
	CatVar* v_bBuildings;
	CatVar* v_bIgnoreVaccinator;
};

DECLARE_HACK_SINGLETON(Triggerbot);

#endif /* HTRIGGER_H_ */
