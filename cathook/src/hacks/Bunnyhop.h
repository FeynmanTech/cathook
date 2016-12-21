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
	DECLARE_HACK_METHODS();
	Bunnyhop();
	CatVar* v_bEnabled;
	CatVar* v_bAutoJump;
	CatVar* v_iAutoJumpSpeed;
};

DECLARE_HACK_SINGLETON(Bunnyhop);

#endif /* HBUNNYHOP_H_ */
