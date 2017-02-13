/*
 * chatstack.cpp
 *
 *  Created on: Jan 24, 2017
 *      Author: nullifiedcat
 */

#include "chatstack.h"
#include "common.h"
#include "sdk.h"

void ChatStack::OnCreateMove() {
	if (stack.size() == 0) return;
	if (m_fLastSay > g_pGlobals->curtime) m_fLastSay = 0;
	if (g_pGlobals->curtime - CHATSTACK_INTERVAL <= m_fLastSay) return;
	const char* msg = Pop();
	const char* cmd = strfmt("say \"%s\"", msg);
	g_IEngine->ServerCmd(cmd);
	m_fLastSay = g_pGlobals->curtime;
	delete [] cmd;
}

void ChatStack::Push(const char* message) {
	if (m_nStackDepth >= CHATSTACK_SIZE) return;
	strncpy(&m_Buffer[m_nStackDepth++ * 256], message, 255);
}

const char* ChatStack::Pop() {
	if (m_nStackDepth <= 0) return 0;
	return (const char*)&m_Buffer[(--m_nStackDepth) * 256];
}


void ChatStack::Reset() {
	m_nStackDepth = 0;
}

ChatStack* g_pChatStack = 0;
