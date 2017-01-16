/*
 * IHack.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef IHACK_H_
#define IHACK_H_

class CUserCmd;
class ConVar;
class CatVar;
class ConCommand;
class CCommand;
class CachedEntity;

typedef unsigned int uint32;

#define DECLARE_HACK_METHODS() \
	const char* GetName(); \
	bool CreateMove(void*, float, CUserCmd*); \
	void PaintTraverse(void*, unsigned int, bool, bool); \
	void LevelInit(const char*); \
	void LevelShutdown();

#define DECLARE_HACK_SINGLETON(x) \
extern x* g_ph##x;

#define DEFINE_HACK_SINGLETON(x) \
x* g_ph##x = 0;

class IHack {
public:
	inline virtual ~IHack() {};
	virtual const char* GetName() = 0;
	inline virtual void PaintTraverse(void*, unsigned int, bool, bool) {};
	inline virtual bool CreateMove(void*, float, CUserCmd*) { return true; };
	inline virtual void LevelInit(const char*) {};
	inline virtual void LevelShutdown() {};
	// Create() and Destroy() are deprecated: use ctors and destructors instead.
	//inline virtual void Create() {};
	//inline virtual void Destroy() {};
	//virtual void BeforePaintTraverse(void*, unsigned int, bool, bool);
	//virtual void BeforeCreateMove(void*, unsigned int, bool, bool);
};

#endif /* IHACK_H_ */
