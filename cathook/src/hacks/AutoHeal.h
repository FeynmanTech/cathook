/*
 * AutoHeal.h
 *
 *  Created on: Dec 3, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOHEAL_H_
#define HACKS_AUTOHEAL_H_

#include "IHack.h"

class AutoHeal : public IHack {
public:
	DECLARE_HACK_METHODS();
	AutoHeal();
	int GetBestHealingTarget();
	int GetHealingPriority(int idx);
	bool CanHeal(int idx);
	int m_iCurrentHealingTarget;
	int m_iNewTarget;
	CatVar* v_bEnabled;
	CatVar* v_bSilent;
};

DECLARE_HACK_SINGLETON(AutoHeal);

#endif /* HACKS_AUTOHEAL_H_ */
