/*
 * Achievement.h
 *
 *  Created on: Jan 20, 2017
 *      Author: nullifiedcat
 */

#ifndef HACKS_ACHIEVEMENT_H_
#define HACKS_ACHIEVEMENT_H_

#include "IHack.h"

class AchievementHack : public IHack {
public:
	AchievementHack();

	void UnlockAll();
	void LockAll();

	ConCommand* c_Unlock;
	ConCommand* c_Lock;
};

void CC_Achievement_Unlock(const CCommand& args);
void CC_Achievement_Lock(const CCommand& args);

DECLARE_HACK_SINGLETON(AchievementHack);

#endif /* HACKS_ACHIEVEMENT_H_ */
