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
	g_IEngine->ServerCmd(format("say \"", stack.top(), '"').c_str());
	stack.pop();
	m_fLastSay = g_pGlobals->curtime;
}

void ChatStack::Reset() {
	while (!stack.empty) stack.pop();
	m_fLastSay = 0.0f;
}

ChatStack* g_pChatStack = 0;
