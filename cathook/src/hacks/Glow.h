/*
 * Glow.h
 *
 *  Created on: Jan 16, 2017
 *      Author: nullifiedcat
 */

#ifndef HACKS_GLOW_H_
#define HACKS_GLOW_H_

#include "IHack.h"

class Glow : public IHack {
public:
	DECLARE_HACK_METHODS();
	Glow();
	CatVar* v_bEnabled;
	bool m_bEnabledOnce;
};

DECLARE_HACK_SINGLETON(Glow);

#endif /* HACKS_GLOW_H_ */
