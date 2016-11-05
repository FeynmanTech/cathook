/*
 * hack.h
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#ifndef HACK_H_
#define HACK_H_

#include <vector>

class IHack;
class CUserCmd;

namespace hack {

extern std::vector<IHack*> hacks;
extern bool shutdown;

void Hk_PaintTraverse(void*, unsigned int, bool, bool);
bool Hk_CreateMove(void*, float, CUserCmd*);

void AddHack(IHack* hack);

void Initialize();
void Think();
void Shutdown();

}

#endif /* HACK_H_ */
