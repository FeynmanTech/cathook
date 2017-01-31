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
	v_bSpam = new CatVar(CV_SWITCH, "spam", "0", "Chat spam", NULL, "Enable Spam");
	v_bSpamNewlines = new CatVar(CV_SWITCH, "spam_newlines", "1", "Prepend newlines to messages", NULL, "If enabled, several newlines will be added before each message");
	v_sSpamFile = new CatVar(CV_STRING, "spam_file", "spam.txt", "Spam file (~/.cathook/...)", NULL, "Spam file name. Each line should be no longer than 100 characters, file must be located in ~/.cathook folder");
	c_Reload = CreateConCommand(CON_PREFIX "spam_reload", &CC_Spam_ReloadFile, "Reloads spam file");
	m_iCurrentIndex = 0;
	m_fLastSpam = 0;
	m_TextFile = new TextFile(256, 256);
}

bool Spam::CreateMove(void*, float, CUserCmd*) {
	if (!v_bSpam->GetBool()) return true;
	if (interfaces::gvars->curtime - m_fLastSpam < 0.8f) return true;
	if (interfaces::gvars->curtime < m_fLastSpam) m_fLastSpam = 0.0f;
	if (m_TextFile->GetLineCount() == 0) return true;
	if (m_iCurrentIndex >= m_TextFile->GetLineCount() || m_iCurrentIndex < 0) m_iCurrentIndex = 0;
	char* spam = 0;
	if (v_bSpamNewlines->GetBool()) {
		spam = strfmt("\n\n\n\n\n\n\n\n\n\n\n\n\n%s", m_TextFile->GetLine(m_iCurrentIndex));
	} else {
		spam = strfmt("%s", m_TextFile->GetLine(m_iCurrentIndex));
	}
	ReplaceString(spam, "\\n", "\n");
	g_pChatStack->Push(spam);
	delete [] spam;
	m_iCurrentIndex++;
	m_fLastSpam = interfaces::gvars->curtime;
	return true;
}

void Spam::LevelInit(const char*) {
	m_fLastSpam = 0;
}
void Spam::PaintTraverse(void*, unsigned int, bool, bool) {}
void Spam::LevelShutdown() {
	m_fLastSpam = 0;
}

void CC_Spam_ReloadFile(const CCommand& args) {
	g_phSpam->m_TextFile->LoadFile(g_phSpam->v_sSpamFile->GetString());
}
