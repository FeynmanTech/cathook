/*
 * chatstack.h
 *
 *  Created on: Jan 24, 2017
 *      Author: nullifiedcat
 */

#ifndef CHATSTACK_H_
#define CHATSTACK_H_

#define CHATSTACK_SIZE 32
#define CHATSTACK_INTERVAL 0.8f

#include "beforecheaders.h"
#include <string>
#include <stack>
#include <functional>
#include "aftercheaders.h"

class ChatStack {
public:
	typedef std::function<std::string(void)> MessageProviderFn_t;

	void OnCreateMove();
	void Reset();

	std::vector<MessageProviderFn_t> bottomProviders;
	std::stack<std::string> stack;

	float m_fLastSay;
};

extern ChatStack* g_pChatStack;

#endif /* CHATSTACK_H_ */
