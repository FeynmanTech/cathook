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
bool IsHooked(void* inst);

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

extern VMTHook* hkPanel;
extern VMTHook* hkClientMode;
extern VMTHook* hkClient;
extern VMTHook* hkNetChannel;

extern unsigned int offPaintTraverse;
extern unsigned int offCreateMove;
extern unsigned int offOverrideView;
extern unsigned int offFrameStageNotify;
extern unsigned int offCanPacket;
extern unsigned int offSendNetMsg;

}

#endif /* HOOKS_H_ */
