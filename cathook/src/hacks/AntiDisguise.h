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
	DECLARE_HACK_METHODS();
	AntiDisguise();
	CatVar* v_bEnabled;
};

DECLARE_HACK_SINGLETON(AntiDisguise);

#endif /* HACKS_ANTIDISGUISE_H_ */
