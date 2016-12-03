/*
 * hack.h
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#ifndef HACK_H_
#define HACK_H_

#include <vector>

#define ADD_HACK(x) \
	hack::AddHack(g_ph##x = new x());

class IHack;
class CUserCmd;
class CViewSetup;

namespace hack {

extern std::vector<IHack*> hacks;
extern bool shutdown;
extern bool invalidated;

void Hk_OverrideView(void*, CViewSetup*);
void Hk_PaintTraverse(void*, unsigned int, bool, bool);
bool Hk_CreateMove(void*, float, CUserCmd*);

void AddHack(IHack* hack);

void Initialize();
void Think();
void Shutdown();

void InitHacks();

}

#endif /* HACK_H_ */
