/*
 * AutoSticky.h
 *
 *  Created on: Dec 2, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOSTICKY_H_
#define HACKS_AUTOSTICKY_H_

#include "IHack.h"

class IClientEntity;

class AutoSticky {
public:
	DECLARE_HACK_METHODS();
	AutoSticky();
	bool ShouldDetonate(IClientEntity* bomb);
	ConVar* v_bEnabled;
	ConVar* v_bBuildings;
	ConVar* v_bScottish;
	ConVar* v_flDetonateDistance;
};

DECLARE_HACK_SINGLETON(AutoSticky);

#endif /* HACKS_AUTOSTICKY_H_ */
