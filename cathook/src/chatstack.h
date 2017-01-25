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

class ChatStack {
public:
	ChatStack();
	~ChatStack();
	void OnCreateMove();
	void Reset();
	void Push(const char* message);
	const char* Pop();
	int m_nStackDepth;
	char* m_Buffer;
	float m_fLastSay;
};

extern ChatStack* g_pChatStack;

#endif /* CHATSTACK_H_ */
