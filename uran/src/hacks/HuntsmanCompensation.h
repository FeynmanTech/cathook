/*
 * HuntsmanCompensation.h
 *
 *  Created on: Dec 4, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_HUNTSMANCOMPENSATION_H_
#define HACKS_HUNTSMANCOMPENSATION_H_

#include "IHack.h"

class HuntsmanCompensation : public IHack {
public:
	DECLARE_HACK_METHODS();
	HuntsmanCompensation();
	ConVar* v_bEnabled;
};

DECLARE_HACK_SINGLETON(HuntsmanCompensation);

#endif /* HACKS_HUNTSMANCOMPENSATION_H_ */
