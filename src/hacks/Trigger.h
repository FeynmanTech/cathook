/*
 * HTrigger.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HTRIGGER_H_
#define HTRIGGER_H_

#include "IHack.h"

class CatVar_DEPRECATED;

class Triggerbot : public IHack {
public:
	Triggerbot();
	~Triggerbot();

	virtual void ProcessUserCmd(CUserCmd*) override;
	virtual void Draw() override;

	CatVar_DEPRECATED* v_bEnabled;
	CatVar_DEPRECATED* v_bRespectCloak;
	CatVar_DEPRECATED* v_bZoomedOnly;
	CatVar_DEPRECATED* v_iHitbox;
	CatVar_DEPRECATED* v_bBodyshot;
	CatVar_DEPRECATED* v_bFinishingHit;
	CatVar_DEPRECATED* v_iMaxRange;
	CatVar_DEPRECATED* v_bBuildings;
	CatVar_DEPRECATED* v_bIgnoreVaccinator;
	CatVar_DEPRECATED* v_bAmbassadorCharge;
	CatVar_DEPRECATED* v_bImproveAccuracy;
};

DECLARE_HACK_SINGLETON(Triggerbot);

#endif /* HTRIGGER_H_ */
