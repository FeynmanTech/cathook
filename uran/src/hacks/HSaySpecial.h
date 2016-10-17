/*
 * HTesting.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HSAYSPECIAL_H_
#define HSAYSPECIAL_H_

#include "IHack.h"

class CUserCmd;
class ConVar;
class ConCommand;

class HSaySpecial : public IHack {
public:
	bool CreateMove(void*, float, CUserCmd*);
	void PaintTraverse(void*, unsigned int, bool, bool);
	void Create();
	void Destroy();
	ConCommand* c_SayLine;
};

#endif /* HSAYSPECIAL_H_ */
