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

const char* KillSay::GetName() { return "KILLSAY"; }

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
		g_phKillSay->PushStack(msg);
		delete msg;
	}
	SEGV_END;
}

const char* KillSay::ComposeKillSay(IGameEvent* event) {
	if (m_nKillSays == 0) return 0;
	if (!event) return 0;
	int vid = event->GetInt("userid");
	int kid = event->GetInt("attacker");
	if (interfaces::engineClient->GetPlayerForUserID(kid) != interfaces::engineClient->GetLocalPlayer()) return 0;
	char* msg = strfmt("%s", m_KillSays[rand() % m_nKillSays]);
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
	v_bEnabled = CREATE_CV(CV_SWITCH, "killsay", "0", "KillSay");
	v_sFileName = CREATE_CV(CV_STRING, "killsay_file", "killsays.txt", "Killsay file (~/.cathook/)");
	m_Stack = new char256[KILLSAY_STACK_SIZE];
	CreateConCommand("cat_killsay_reload", CC_KillSay_ReloadFile, "Reload KillSay");
	m_nStackTop = -1;
	m_fLastSay = 0.0f;
	m_nKillSays = 0;
	interfaces::eventManager->AddListener(&m_Listener, "player_death", false);
}

void CC_KillSay_ReloadFile(const CCommand& args) {
	SAFE_CALL(g_phKillSay->LoadFile());
}

void KillSay::LoadFile() {
	uid_t uid = geteuid();
	passwd* pw = getpwuid(uid);
	if (!pw) {
		logging::Info("can't get the username!");
		return;
	}
	char* filename = strfmt("/home/%s/.cathook/%s", pw->pw_name, v_sFileName->m_pConVar->GetString());
	FILE* file = fopen(filename, "r");
	if (!file) {
		logging::Info("Could not open the file: %s", filename);
		delete filename;
		return;
	}
	delete filename;
	for (m_nKillSays = 0; m_nKillSays < KILLSAY_ARRAY_MAX_SIZE; m_nKillSays++) {
		if(fgets(m_KillSays[m_nKillSays], KILLSAY_MAX_LENGTH, file)) {
			m_KillSays[m_nKillSays][strlen(m_KillSays[m_nKillSays]) - 1] = '\0';
		} else break;
	}
}

void KillSay::PushStack(const char* text) {
	if (m_nStackTop == KILLSAY_STACK_SIZE - 1) return;
	strncpy(m_Stack[++m_nStackTop].data, text, 255);
	//logging::Info("Pushed, new stacktop: %i", m_nStackTop);
}

char256* KillSay::PopStack() {
	if (m_nStackTop < 0) return 0;
	return &m_Stack[m_nStackTop--];
}

bool KillSay::CreateMove(void*, float, CUserCmd*) {
	if (m_nStackTop <= 0) return true;
	if (interfaces::gvars->realtime - m_fLastSay < 1.0f) return true;
	char256* killsay = PopStack();
	if (!killsay) return true;
	char* cmd = strfmt("say \"%s\"", killsay->data);
	interfaces::engineClient->ServerCmd(cmd, true);
	delete cmd;
	m_fLastSay = interfaces::gvars->realtime;
	return true;
}
void KillSay::PaintTraverse(void*, unsigned int, bool, bool) {}
void KillSay::LevelInit(const char*) {
	m_fLastSay = 0.0f;
	m_nStackTop = -1;
}
void KillSay::LevelShutdown() {}
