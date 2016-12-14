/*
 * AutoReflect.h
 *
 *  Created on: Nov 18, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOREFLECT_H_
#define HACKS_AUTOREFLECT_H_

#include "IHack.h"

class IClientEntity;

class AutoReflect : public IHack {
public:
	DECLARE_HACK_METHODS();
	AutoReflect();
	bool ShouldReflect(IClientEntity* ent);
	ConVar* v_bEnabled;
	ConVar* v_bDisableWhenAttacking;
	ConVar* v_bReflectStickies;
	ConVar* v_iReflectDistance;
};

DECLARE_HACK_SINGLETON(AutoReflect);

#endif /* HACKS_AUTOREFLECT_H_ */
