/*
 * KillSay.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: nullifiedcat
 */

#include "KillSay.h"
#include "../common.h"
#include "../sdk.h"
#include <pwd.h>

#include "beforecheaders.h"
#include <cstring>
#include "aftercheaders.h"

// TODO deathmatch stuff..

namespace hacks { namespace shared { namespace killsay {

const std::string classes[] = {
	"class",
	"scout",
	"sniper",
	"soldier",
	"demoman",
	"medic",
	"heavy",
	"pyro",
	"spy",
	"engineer"
};

const std::string teams[] = {
	"RED",
	"BLU"
};

DeathListener listener;
TextFile killsays;

void DeathListener::FireGameEvent(IGameEvent* event) {
	if (!enabled) return;
	std::string message = ComposeMessage(event);
	if (message.size()) chat_stack::stack.push(message);
}

DeathListener::~DeathListener() {
	g_IEventManager->RemoveListener(this);
}

std::string ComposeMessage(IGameEvent* event) {
	if (killsays.LineCount() == 0) return 0;
	if (!event) return 0;
	int vid = event->GetInt("userid");
	int kid = event->GetInt("attacker");
	if (kid == vid) return 0;
	if (g_IEngine->GetPlayerForUserID(kid) != g_IEngine->GetLocalPlayer()) return 0;
	std::string message = killsays.Line((rand() % killsays.LineCount()));
	player_info_s info;
	g_IEngine->GetPlayerInfo(g_IEngine->GetPlayerForUserID(vid), &info);
	ReplaceString(message, "%name%", info.name);
	CachedEntity& ent = gEntityCache.Entity(g_IEngine->GetPlayerForUserID(vid));
	int clz = ent.Class();
	ReplaceString(message, "%class%", (char*)classes[clz]);
	player_info_s infok;
	g_IEngine->GetPlayerInfo(g_IEngine->GetPlayerForUserID(kid), &infok);
	ReplaceString(message, "%killer%", (char*)infok.name);
	ReplaceString(message, "%team%", (char*)teams[ent.Team() - 2]);
	ReplaceString(message, "%myteam%", (char*)teams[g_LocalPlayer.entity->Team() - 2]);
	ReplaceString(message, "%myclass%", (char*)classes[g_LocalPlayer.entity->Class()]);
	ReplaceString(message, "\\n", "\n");
	return message;
}

CatVar enabled(CV_SWITCH, "killsay", "0", "KillSay", NULL, "Enable KillSay");
CatVar filename(CV_STRING, "killsay_file", "killsays.txt", "Killsay file (~/.cathook/)", NULL, "Killsay file name. Should be located in ~/.cathook folder.");
CatVar compatability;
CatCommand reload("cat_killsay_reload", []() {
	killsays.Load((std::string)filename);
}, "Reload KillSay");

}}}
