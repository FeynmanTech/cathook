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

#define CREATE_MOVE(x) \
	g_ph##x->CreateMove(thisptr, inputSample, cmd)

#define DEBUG_SEGV false

#if DEBUG_SEGV == true

#define SEGV_BEGIN \
	try {

#define SEGV_END \
	} catch (...) { \
		logging::Info("SEGV/FPE occured! %s in %s:%d", __func__, __FILE__, __LINE__); \
	}

#define SEGV_END_INFO(x) \
	} catch (...) { \
		logging::Info("SEGV/FPE occured! (%s)", x); \
	}

#define SAFE_CALL(x) \
	SEGV_BEGIN \
	x; \
	SEGV_END_INFO(#x)

#else

#define SEGV_BEGIN
#define SEGV_END
#define SEGV_END_INFO(x)
#define SAFE_CALL(x) x

#endif

class IHack;
class CUserCmd;
class CViewSetup;
class bf_read;
class ConCommand;
class CCommand;

namespace hack {

extern std::vector<IHack*> hacks;
extern bool shutdown;
extern bool invalidated;

void Hk_OverrideView(void*, CViewSetup*);
void Hk_PaintTraverse(void*, unsigned int, bool, bool);
bool Hk_CreateMove(void*, float, CUserCmd*);
void Hk_FrameStageNotify(void*, int stage);
bool Hk_DispatchUserMessage(void*, int, bf_read&);

void AddHack(IHack* hack);

void Initialize();
void Think();
void Shutdown();

void InitHacks();

void CC_Cat(const CCommand& args);
extern ConCommand* c_Cat;

}

#endif /* HACK_H_ */
