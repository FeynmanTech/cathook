/*
 * hack.h
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#ifndef HACK_H_
#define HACK_H_

#define ADD_HACK(x) \
	g_ph##x = new x()

#define CREATE_MOVE(x) \
	g_ph##x->CreateMove(thisptr, inputSample, cmd)

#define PAINT_TRAVERSE(x) \
	g_ph##x->PaintTraverse(p, vp, fr, ar)

#define LEVEL_INIT(x, a) \
	g_ph##x->LevelInit(a)

#define LEVEL_INIT(x) \
	g_ph##x->LevelShutdown()

#define DELETE_HACK(x) \
	delete g_ph##x

class IHack;
class CUserCmd;
class CViewSetup;
class bf_read;
class ConCommand;
class CCommand;

namespace hack {

extern bool shutdown;

void Initialize();
void Think();
void Shutdown();

void InitHacks();

void CC_Cat(const CCommand& args);
extern ConCommand* c_Cat;

}

#endif /* HACK_H_ */
