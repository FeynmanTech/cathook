/*
 * AutoSticky.h
 *
 *  Created on: Dec 2, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOSTICKY_H_
#define HACKS_AUTOSTICKY_H_

#include "IHack.h"

class CachedEntity;

class AutoSticky : public IHack {
public:
	AutoSticky();

	virtual void ProcessUserCmd(CUserCmd*) override;

	bool ShouldDetonate(CachedEntity* bomb);
	CatVar* v_bEnabled;
	CatVar* v_bBuildings;
	CatVar* v_bScottish;
	CatVar* v_flDetonateDistance;
};

DECLARE_HACK_SINGLETON(AutoSticky);

#endif /* HACKS_AUTOSTICKY_H_ */
