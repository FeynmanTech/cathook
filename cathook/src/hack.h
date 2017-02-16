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

#define HACK_PROCESS_USERCMD(x, y) \
	{ PROF_SECTION(x, "CreateMove@" #x); \
	g_ph##x->ProcessUserCmd(y); }

#define HACK_DRAW(x) \
	{ PROF_SECTION(x, "PaintTraverse@" #x); \
	g_ph##x->Draw(); }

#define LEVEL_INIT(x) \
	g_ph##x->OnLevelInit()

#define LEVEL_SHUTDOWN(x) \
	g_ph##x->OnLevelShutdown()

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

void CC_Cat(const CCommand& args);
extern ConCommand* c_Cat;

}

#endif /* HACK_H_ */
