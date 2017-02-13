/*
 * Achievement.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: nullifiedcat
 */

#include "Achievement.h"

#include "../common.h"
#include "../helpers.h"

#include "../sdk.h"

DEFINE_HACK_SINGLETON(AchievementHack);

AchievementHack::AchievementHack() {
	c_Unlock = CreateConCommand(CON_PREFIX "achievement_unlock", &CC_Achievement_Unlock, "Unlocks all achievements.");
	c_Lock = CreateConCommand(CON_PREFIX "achievement_lock", &CC_Achievement_Lock, "Locks all achievements.");
}

void CC_Achievement_Unlock(const CCommand& args) {
	g_phAchievementHack->UnlockAll();
}

void AchievementHack::UnlockAll() {
	for (int i = 0; i < interfaces::achievements->GetAchievementCount(); i++) {
		interfaces::achievements->AwardAchievement(interfaces::achievements->GetAchievementByIndex(i)->GetAchievementID());
	}
}

void AchievementHack::LockAll() {
	interfaces::stats->RequestCurrentStats();
	//interfaces::stats->ResetAllStats(true);
	for (int i = 0; i < interfaces::achievements->GetAchievementCount(); i++) {
		interfaces::stats->ClearAchievement(interfaces::achievements->GetAchievementByIndex(i)->GetName());
	}
	interfaces::stats->StoreStats();
	interfaces::stats->RequestCurrentStats();
}

void CC_Achievement_Lock(const CCommand& args) {
	g_phAchievementHack->LockAll();
}
