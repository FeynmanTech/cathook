/*
 * HGlow.h
 *
 *  Created on: Oct 22, 2016
 *      Author: nullifiedcat
 */

#ifndef HGLOW_H_
#define HGLOW_H_

#include "IHack.h"

#include "../fixsdk.h"
#include "basehandle.h"

class ConVar;
class ConCommand;

class HGlow : public IHack {
public:
	void Create();
	bool CreateMove(void*, float, CUserCmd*);
	void Destroy();
	void PaintTraverse(void*, unsigned int, bool, bool);
	ConVar* v_bEnabled;
	ConVar* v_bEnemyOnly;
	ConVar* v_bHealthColor;
	ConCommand* cmd_scan;
};


#endif /* HGLOW_H_ */
