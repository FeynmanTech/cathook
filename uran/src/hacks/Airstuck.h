/*
 * Airstuck.h
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AIRSTUCK_H_
#define HACKS_AIRSTUCK_H_

#include "IHack.h"

class Airstuck : public IHack {
public:
	DECLARE_HACK_METHODS();
	Airstuck();

	ConVar* v_bStuck;
};

extern Airstuck* g_phAirstuck;

#endif /* HACKS_AIRSTUCK_H_ */
