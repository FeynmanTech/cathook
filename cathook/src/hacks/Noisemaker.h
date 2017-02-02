/*
 * Noisemaker.h
 *
 *  Created on: Feb 2, 2017
 *      Author: nullifiedcat
 */

#ifndef HACKS_NOISEMAKER_H_
#define HACKS_NOISEMAKER_H_

#include "IHack.h"

class Noisemaker : public IHack {
public:
	DECLARE_HACK_METHODS();

	Noisemaker();

	CatVar* v_bEnabled;
};

DECLARE_HACK_SINGLETON(Noisemaker);

#endif /* HACKS_NOISEMAKER_H_ */
