/*
 * chatstack.cpp
 *
 *  Created on: Jan 24, 2017
 *      Author: nullifiedcat
 */

#include "chatstack.h"
#include "common.h"
#include "sdk.h"

namespace chat_stack {

void AddProvider(MessageProviderFn_t provider) {
	bottom_providers.push_back(provider);
}

void OnCreateMove() {
	if (last_say > g_pGlobals->curtime) last_say = 0;
	if (g_pGlobals->curtime - CHATSTACK_INTERVAL <= last_say) return;
	std::string message;
	if (stack.size() == 0) {
		if (bottom_providers.size()) {
			message = bottom_providers[provider_index]();
			provider_index++;
			if (provider_index >= bottom_providers.size()) provider_index = 0;
		}
	} else {
		message = stack.top();
		stack.pop();
	}
	if (message.size())
		g_IEngine->ServerCmd(format("say \"", message, '"').c_str());
	last_say = g_pGlobals->curtime;
}

void Reset() {
	while (!stack.empty) stack.pop();
	last_say = 0.0f;
}

size_t provider_index = 0;
std::vector<MessageProviderFn_t> bottom_providers;
std::stack<std::string> stack;
float last_say = 0.0f;

}
