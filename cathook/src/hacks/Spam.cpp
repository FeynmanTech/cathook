/*
 * Spam.cpp
 *
 *  Created on: Jan 21, 2017
 *      Author: nullifiedcat
 */

#include "Spam.h"
#include "../common.h"
#include "../sdk.h"
#include <pwd.h>

DEFINE_HACK_SINGLETON(Spam);

const char* Spam::GetName() { return "SPAM"; }

Spam::Spam() {
	v_bSpam = CREATE_CV(CV_SWITCH, "spam", "0", "Chat spam");
	v_bSpamNewlines = CREATE_CV(CV_SWITCH, "spam_newlines", "1", "Prepend newlines to messages");
	v_sSpamFile = CREATE_CV(CV_STRING, "spam_file", "spam.txt", "Spam file (~/.cathook/...)");
	c_Reload = CreateConCommand(CON_PREFIX "spam_reload", &CC_Spam_ReloadFile, "Reloads spam file");
	m_nSpam = 0;
	m_iCurrentIndex = 0;
	m_fLastSpam = 0;
}

bool Spam::CreateMove(void*, float, CUserCmd*) {
	if (!v_bSpam->GetBool()) return true;
	if (interfaces::gvars->curtime - m_fLastSpam < 0.8f) return true;
	if (m_nSpam == 0) return true;
	if (m_iCurrentIndex >= m_nSpam || m_iCurrentIndex >= SPAM_MAX_AMOUNT) m_iCurrentIndex = 0;
	char* spam = 0;
	if (v_bSpamNewlines->GetBool()) {
		spam = strfmt("say \"\n\n\n\n\n\n\n\n\n\n\n\n%s\"", m_Spam[m_iCurrentIndex]);
	} else {
		spam = strfmt("say \"%s\"", m_Spam[m_iCurrentIndex]);
	}
	interfaces::engineClient->ServerCmd(spam);
	m_fLastSpam = interfaces::gvars->curtime;
	delete [] spam;
	m_iCurrentIndex++;
	return true;
}

void Spam::LevelInit(const char*) {
	m_fLastSpam = 0.0f;
}

void Spam::PaintTraverse(void*, unsigned int, bool, bool) {}
void Spam::LevelShutdown() {}

void Spam::LoadFile() {
	uid_t uid = geteuid();
	passwd* pw = getpwuid(uid);
	if (!pw) {
		logging::Info("can't get the username!");
		return;
	}
	char* filename = strfmt("/home/%s/.cathook/%s", pw->pw_name, v_sSpamFile->m_pConVar->GetString());
	FILE* file = fopen(filename, "r");
	if (!file) {
		logging::Info("Could not open the file: %s", filename);
		delete filename;
		return;
	}
	delete filename;
	for (m_nSpam = 0; m_nSpam < SPAM_MAX_AMOUNT; m_nSpam++) {
		if(fgets(m_Spam[m_nSpam], SPAM_MAX_LENGTH, file)) {
			m_Spam[m_nSpam][strlen(m_Spam[m_nSpam]) - 1] = '\0';
		} else break;
	}
}

void CC_Spam_ReloadFile(const CCommand& args) {
	g_phSpam->LoadFile();
}
