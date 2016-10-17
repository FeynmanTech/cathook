/*
 * hooks.h
 *
 *  Created on: Oct 4, 2016
 *      Author: nullifiedcat
 */

#ifndef HOOKS_H_
#define HOOKS_H_

namespace hooks {

// Parts of copypasted code
// Credits: Casual_Hacker

unsigned int CountMethods(void** vmt);
void**& GetVMT(void* inst, unsigned int offset);

class VMTHook {
public:
	enum { GUARD = 0xD34DC477 };
	void Init(void* inst, unsigned int offset);
	void Kill();
	void HookMethod(void* func, unsigned int idx);
	void* GetMethod(unsigned int idx) const;
	void Apply();
protected:
	void ***vmt;
	void **oldvmt;
	void **array;
};

extern VMTHook* hkPaintTraverse;
extern VMTHook* hkCreateMove;

extern unsigned int offPaintTraverse;
extern unsigned int offCreateMove;

}

#endif /* HOOKS_H_ */
