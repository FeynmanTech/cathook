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

CatVar enabled(CV_SWITCH, "killsay", "0", "KillSay", NULL, "Enable KillSay");
CatVar filename(CV_STRING, "killsay_file", "killsays.txt", "Killsay file (~/.cathook/)", NULL, "Killsay file name. Should be located in ~/.cathook folder.");
CatVar compatability;
CatCommand reload("cat_killsay_reload", []() {
	killsays.Load(filename);
}, "Reload KillSay");

}}}

void KillSayEventListener::FireGameEvent(IGameEvent* event) {
	if (!g_phKillSay->v_bEnabled->GetBool()) return;
	SEGV_BEGIN;
	const char* msg = g_phKillSay->ComposeKillSay(event);
	if (msg) {
		g_pChatStack->Push(msg);
		delete msg;
	}
	SEGV_END;
}

const char* KillSay::ComposeKillSay(IGameEvent* event) {
	if (m_TextFile->GetLineCount() == 0) return 0;
	if (!event) return 0;
	int vid = event->GetInt("userid");
	int kid = event->GetInt("attacker");
	if (kid == vid) return 0;
	if (g_IEngine->GetPlayerForUserID(kid) != g_IEngine->GetLocalPlayer()) return 0;
	char* msg = strfmt("%s", m_TextFile->GetLine(rand() % m_TextFile->GetLineCount()));
	player_info_s info;
	g_IEngine->GetPlayerInfo(g_IEngine->GetPlayerForUserID(vid), &info);
	ReplaceString(msg, "%name%", info.name);
	CachedEntity* ent = ENTITY(g_IEngine->GetPlayerForUserID(vid));
	int clz = g_pPlayerResource->GetClass(ent);
	ReplaceString(msg, "%class%", (char*)tf_classes_killsay[clz]);
	player_info_s infok;
	g_IEngine->GetPlayerInfo(g_IEngine->GetPlayerForUserID(kid), &infok);
	ReplaceString(msg, "%killer%", (char*)infok.name);
	ReplaceString(msg, "%team%", (char*)tf_teams_killsay[ent->m_iTeam - 2]);
	ReplaceString(msg, "%myteam%", (char*)tf_teams_killsay[LOCAL_E->m_iTeam - 2]);
	ReplaceString(msg, "%myclass%", (char*)tf_classes_killsay[g_pPlayerResource->GetClass(LOCAL_E)]);
	ReplaceString(msg, "\\n", "\n");
	return msg;
}

KillSay::KillSay() {
	v_bEnabled = new CatVar;
	v_sFileName = new CatVar;
	CreateConCommand;
	m_TextFile = new TextFile(256, 1024);
	g_IEventManager->AddListener(&m_Listener, "player_death", false);
}

KillSay::~KillSay() {
	g_IEventManager->RemoveListener(&m_Listener);
}

void KillSay::Reload() {
	m_TextFile->LoadFile(v_sFileName->m_pConVar->GetString());
}

void CC_KillSay_ReloadFile(const CCommand& args) {
	SAFE_CALL(g_phKillSay->Reload());
}
