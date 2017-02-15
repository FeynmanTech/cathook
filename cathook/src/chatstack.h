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

namespace chat_stack {

typedef std::function<std::string(void)> MessageProviderFn_t;

void AddProvider(MessageProviderFn_t provider);
void OnCreateMove();
void Reset();

extern size_t provider_index;
extern std::vector<MessageProviderFn_t> bottom_providers;
extern std::stack<std::string> stack;
extern float last_say;

};

#endif /* CHATSTACK_H_ */
