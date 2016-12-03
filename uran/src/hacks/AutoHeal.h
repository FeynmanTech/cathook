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
	ConVar* v_bEnabled;
};

DECLARE_HACK_SINGLETON(AutoHeal);

#endif /* HACKS_AUTOHEAL_H_ */
