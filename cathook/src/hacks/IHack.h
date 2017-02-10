/*
 * IHack.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef IHACK_H_
#define IHACK_H_

#include "../beforecheaders.h"
#include <string>
#include "../aftercheaders.h"

class CUserCmd;
class ConVar;
class CatVar;
class ConCommand;
class CCommand;
class CachedEntity;

#define DECLARE_HACK_SINGLETON(x) \
extern x* g_ph##x;

#define DEFINE_HACK_SINGLETON(x) \
x* g_ph##x = 0;

class IHack {
public:
	inline virtual ~IHack() {};
	// Deprecated
	/*
	inline virtual void PaintTraverse(void*, unsigned int, bool, bool) {};
	inline virtual bool CreateMove(void*, float, CUserCmd*) { return true; };
	inline virtual void LevelInit(const char*) {};
	inline virtual void LevelShutdown() {};
	*/
	inline virtual void Draw() {};
	inline virtual void ProcessUserCmd(CUserCmd*) {};
	inline virtual void OnLevelInit() {};
	inline virtual void OnLevelShutdown() {};
};

#endif /* IHACK_H_ */
