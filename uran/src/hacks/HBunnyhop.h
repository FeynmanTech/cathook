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
	bool CreateMove(void*, float, CUserCmd*);
	void PaintTraverse(void*, unsigned int, bool, bool);
	void Create();
	void Destroy();
	ConVar* v_bEnabled;
};

#endif /* HBUNNYHOP_H_ */
