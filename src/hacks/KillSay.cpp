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

DEFINE_HACK_SINGLETON(KillSay);

const char* tf_classes_killsay[] = {
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

const char* tf_teams_killsay[] = {
	"RED",
	"BLU"
};

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
	if (interfaces::engineClient->GetPlayerForUserID(kid) != interfaces::engineClient->GetLocalPlayer()) return 0;
	char* msg = strfmt("%s", m_TextFile->GetLine(rand() % m_TextFile->GetLineCount()));
	player_info_s info;
	interfaces::engineClient->GetPlayerInfo(interfaces::engineClient->GetPlayerForUserID(vid), &info);
	ReplaceString(msg, "%name%", info.name);
	CachedEntity* ent = ENTITY(interfaces::engineClient->GetPlayerForUserID(vid));
	int clz = g_pPlayerResource->GetClass(ent);
	ReplaceString(msg, "%class%", (char*)tf_classes_killsay[clz]);
	player_info_s infok;
	interfaces::engineClient->GetPlayerInfo(interfaces::engineClient->GetPlayerForUserID(kid), &infok);
	ReplaceString(msg, "%killer%", (char*)infok.name);
	ReplaceString(msg, "%team%", (char*)tf_teams_killsay[ent->m_iTeam - 2]);
	ReplaceString(msg, "%myteam%", (char*)tf_teams_killsay[LOCAL_E->m_iTeam - 2]);
	ReplaceString(msg, "%myclass%", (char*)tf_classes_killsay[g_pPlayerResource->GetClass(LOCAL_E)]);
	ReplaceString(msg, "\\n", "\n");
	return msg;
}

KillSay::KillSay() {
	v_bEnabled = new CatVar_DEPRECATED(CV_SWITCH, "killsay", "0", "KillSay", NULL, "Enable KillSay");
	v_sFileName = new CatVar_DEPRECATED(CV_STRING, "killsay_file", "killsays.txt", "Killsay file (~/.cathook/)", NULL, "Killsay file name. Should be located in ~/.cathook folder.");
	CreateConCommand("cat_killsay_reload", CC_KillSay_ReloadFile, "Reload KillSay");
	m_TextFile = new TextFile(256, 1024);
	interfaces::eventManager->AddListener(&m_Listener, "player_death", false);
}

KillSay::~KillSay() {
	interfaces::eventManager->RemoveListener(&m_Listener);
}

void KillSay::Reload() {
	m_TextFile->LoadFile(v_sFileName->m_pConVar->GetString());
}

void CC_KillSay_ReloadFile(const CCommand& args) {
	SAFE_CALL(g_phKillSay->Reload());
}
