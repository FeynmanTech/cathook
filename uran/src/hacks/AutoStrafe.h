/*
 * AutoStrafe.h
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOSTRAFE_H_
#define HACKS_AUTOSTRAFE_H_

#include "IHack.h"

class AutoStrafe : public IHack {
public:
	DECLARE_HACK_METHODS();
	AutoStrafe();
	ConVar* v_bEnabled;
};

extern AutoStrafe* g_phAutoStrafe;

#endif /* HACKS_AUTOSTRAFE_H_ */
