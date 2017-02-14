/*
 * chatstack.cpp
 *
 *  Created on: Jan 24, 2017
 *      Author: nullifiedcat
 */

#include "chatstack.h"
#include "common.h"
#include "sdk.h"

void ChatStack::AddProvider(MessageProviderFn_t provider) {
	bottomProviders.push_back(provider);
}

void ChatStack::OnCreateMove() {
	if (m_fLastSay > g_pGlobals->curtime) m_fLastSay = 0;
	if (g_pGlobals->curtime - CHATSTACK_INTERVAL <= m_fLastSay) return;
	std::string message;
	if (stack.size() == 0) {
		if (bottomProviders.size()) {
			message = bottomProviders[provider_index]();
			provider_index++;
			if (provider_index >= bottomProviders.size()) provider_index = 0;
		}
	} else {
		message = stack.top();
		stack.pop();
	}
	if (message.size())
		g_IEngine->ServerCmd(format("say \"", message, '"').c_str());
	m_fLastSay = g_pGlobals->curtime;
}

void ChatStack::Reset() {
	while (!stack.empty) stack.pop();
	m_fLastSay = 0.0f;
}

ChatStack* g_pChatStack = 0;
