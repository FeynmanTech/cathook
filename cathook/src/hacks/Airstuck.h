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
	Airstuck();

	virtual void ProcessUserCmd(CUserCmd*) override;

	ConVar* v_bStuck;
};

DECLARE_HACK_SINGLETON(Airstuck);

#endif /* HACKS_AIRSTUCK_H_ */
