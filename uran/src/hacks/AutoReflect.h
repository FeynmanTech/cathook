/*
 * AutoReflect.h
 *
 *  Created on: Nov 18, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOREFLECT_H_
#define HACKS_AUTOREFLECT_H_

#include "IHack.h"

class AutoReflect : IHack {
public:
	DECL_HACK
	ConVar* v_bEnabled;
	ConVar* v_iReflectDistance;
};

#endif /* HACKS_AUTOREFLECT_H_ */
