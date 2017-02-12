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
	virtual void OnLevelInit() override;
	virtual void OnLevelShutdown() override;

	ConVar* v_bStuck;
};

DECLARE_HACK_SINGLETON(Airstuck);

#endif /* HACKS_AIRSTUCK_H_ */
