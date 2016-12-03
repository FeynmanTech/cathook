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

bool IsReflectableProjectile(IClientEntity* ent);

class AutoReflect : public IHack {
public:
	DECLARE_HACK_METHODS();
	AutoReflect();
	ConVar* v_bEnabled;
	ConVar* v_iReflectDistance;
};

DECLARE_HACK_SINGLETON(AutoReflect);

#endif /* HACKS_AUTOREFLECT_H_ */
