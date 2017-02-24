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
	Glow();

	virtual void ProcessUserCmd(CUserCmd*) override;
	virtual void OnLevelInit() override;
	virtual void OnLevelShutdown() override;

	CatVar* v_bEnabled;
	bool m_bEnabledOnce;
};

DECLARE_HACK_SINGLETON(Glow);

#endif /* HACKS_GLOW_H_ */
