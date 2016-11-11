/*
 * Tracers.h
 *
 *  Created on: Nov 11, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_TRACERS_H_
#define HACKS_TRACERS_H_

#include "IHack.h"

class Tracers : public IHack {
public:
	DECL_HACK
	ConVar* v_bEnabled;
	ConVar* v_bTraceVisible;
};

#endif /* HACKS_TRACERS_H_ */
