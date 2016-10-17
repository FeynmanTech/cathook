/*
 * HPowerupList.h
 *
 *  Created on: Oct 10, 2016
 *      Author: nullifiedcat
 */

#ifndef HPOWERUPLIST_H_
#define HPOWERUPLIST_H_

#include "IHack.h"

class HPowerupList : public IHack {
public:
	void Create();
	void Destroy();
	void CreateMove(void*, float, CUserCmd*);
	void PaintTraverse(void*, unsigned int, bool, bool);

	ConVar* v_bEnabled;
	ConVar* v_iPosX;
	ConVar* v_iPosY;
	ConVar* v_bShowNames;
	ConVar* v_bShowClass;
	ConVar* v_bShowDistance;

	// TODO
};


#endif /* HPOWERUPLIST_H_ */
