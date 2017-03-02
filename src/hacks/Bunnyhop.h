/*
 * HBunnyhop.h
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#ifndef HBUNNYHOP_H_
#define HBUNNYHOP_H_

#include "IHack.h"

class ConVar;

class Bunnyhop : public IHack {
public:
	Bunnyhop();

	virtual void ProcessUserCmd(CUserCmd*) override;

	CatVar_DEPRECATED* v_bEnabled;
	CatVar_DEPRECATED* v_iPerfectJumpLimit;
	CatVar_DEPRECATED* v_bImperfect;
	CatVar_DEPRECATED* v_bAutoJump;
	CatVar_DEPRECATED* v_iAutoJumpSpeed;
	bool m_bFakeLagFix;
};

DECLARE_HACK_SINGLETON(Bunnyhop);

#endif /* HBUNNYHOP_H_ */
