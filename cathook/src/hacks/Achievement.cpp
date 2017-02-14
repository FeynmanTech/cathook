/*
 * Achievement.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: nullifiedcat
 */

#include "Achievement.h"

namespace hacks { namespace tf2 { namespace achievement {

void LockEverything() {
	g_ISteamStats->RequestCurrentStats();
	for (int i = 0; i < g_IAchievement->GetAchievementCount(); i++) {
		g_ISteamStats->ClearAchievement(g_IAchievement->GetAchievementByIndex(i)->GetName());
	}
	g_ISteamStats->StoreStats();
	g_ISteamStats->RequestCurrentStats();
}

void UnlockEverything() {
	for (int i = 0; i < g_IAchievement->GetAchievementCount(); i++) {
		g_IAchievement->AwardAchievement(g_IAchievement->GetAchievementByIndex(i)->GetAchievementID());
	}
}

ConCommand lock("achievement_lock", "Locks all achievements", LockEverything);
ConCommand unlock("achievement_unlock", "Unlocks all achievements", UnlockEverything);

}}}
