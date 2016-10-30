/*
 * AntiAim.h
 *
 *  Created on: Oct 26, 2016
 *      Author: nullifiedcat
 */

#ifndef ANTIAIM_H_
#define ANTIAIM_H_

#include "IHack.h"

class ConVar;

class AntiAim : public IHack {
public:
	void Create();
	bool CreateMove(void*, float, CUserCmd*);
	void Destroy();
	void PaintTraverse(void*, unsigned int, bool, bool);
	ConVar* v_bEnabled;
};

#endif /* ANTIAIM_H_ */
