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

class HBunnyhop : public IHack {
public:
	DECLARE_HACK();
	ConVar* v_bEnabled;
};

extern HBunnyhop* g_phBunnyhop;

#endif /* HBUNNYHOP_H_ */
