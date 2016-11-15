/*
 * Quickscope.h
 *
 *  Created on: Nov 12, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_QUICKSCOPE_H_
#define HACKS_QUICKSCOPE_H_

#include "IHack.h"

class Quickscope : public IHack {
public:
	DECL_HACK
	ConVar* v_bEnabled;
};

#endif /* HACKS_QUICKSCOPE_H_ */
