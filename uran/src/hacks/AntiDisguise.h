/*
 * AntiDisguise.h
 *
 *  Created on: Nov 16, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_ANTIDISGUISE_H_
#define HACKS_ANTIDISGUISE_H_

#include "IHack.h"

class AntiDisguise : public IHack {
public:
	DECL_HACK
	ConVar* v_bEnabled;
};

#endif /* HACKS_ANTIDISGUISE_H_ */
