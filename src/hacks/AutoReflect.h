/*
 * AutoReflect.h
 *
 *  Created on: Nov 18, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOREFLECT_H_
#define HACKS_AUTOREFLECT_H_

#include "IHack.h"

class AutoReflect : public IHack {
public:
	AutoReflect();

	virtual void ProcessUserCmd(CUserCmd*) override;

	bool ShouldReflect(CachedEntity* ent);
	CatVar_DEPRECATED* v_bEnabled;
	CatVar_DEPRECATED* v_bDisableWhenAttacking;
	CatVar_DEPRECATED* v_bReflectStickies;
	CatVar_DEPRECATED* v_iReflectDistance;
};

DECLARE_HACK_SINGLETON(AutoReflect);

#endif /* HACKS_AUTOREFLECT_H_ */
