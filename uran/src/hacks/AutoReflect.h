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
	DECLARE_HACK()
	ConVar* v_bEnabled;
	ConVar* v_iReflectDistance;
};

extern AutoReflect* g_phAutoReflect;

#endif /* HACKS_AUTOREFLECT_H_ */
