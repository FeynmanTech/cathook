/*
 * Achievement.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: nullifiedcat
 */

#include "Achievement.h"
#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(AchievementHack);

const char* AchievementHack::GetName() { return "ACHIEVEMENT"; }

AchievementHack::AchievementHack() {
	c_Unlock = CreateConCommand(CON_PREFIX "achievement_unlock", &CC_Achievement_Unlock, "Unlocks all achievements.");
	c_Lock = CreateConCommand(CON_PREFIX "achievement_lock", &CC_Achievement_Unlock, "Locks all achievements.");
}

void CC_Achievement_Unlock(const CCommand& args) {
	for (int i = 0; i < interfaces::achievements->GetAchievementCount(); i++) {
		interfaces::achievements->AwardAchievement(interfaces::achievements->GetAchievementByIndex(i)->GetAchievementID());
	}
}

void CC_Achievement_Lock(const CCommand& args) {
	for (int i = 0; i < interfaces::achievements->GetAchievementCount(); i++) {
		interfaces::stats->ClearAchievement(interfaces::achievements->GetAchievementByIndex(i)->GetName());
	}
	interfaces::stats->StoreStats();
	interfaces::stats->RequestCurrentStats();
}

bool AchievementHack::CreateMove(void*, float, CUserCmd*) { return true; }
void AchievementHack::PaintTraverse(void*, unsigned int, bool, bool) {}
void AchievementHack::LevelInit(const char*) {}
void AchievementHack::LevelShutdown() {}
