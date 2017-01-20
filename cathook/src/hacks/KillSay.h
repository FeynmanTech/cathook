/*
 * KillSay.h
 *
 *  Created on: Jan 19, 2017
 *      Author: nullifiedcat
 */

#ifndef HACKS_KILLSAY_H_
#define HACKS_KILLSAY_H_

#include "IHack.h"

#include "../fixsdk.h"
#include <igameevents.h>

#define KILLSAY_STACK_SIZE 64
#define KILLSAY_ARRAY_MAX_SIZE 256
#define KILLSAY_MAX_LENGTH 256

struct char256 { char data[256]; };

class KillSayEventListener : public IGameEventListener2 {
	virtual void FireGameEvent(IGameEvent* event);
};

class KillSay : public IHack {
public:
	DECLARE_HACK_METHODS();
	KillSay();
	~KillSay();
	void PushStack(const char* text);
	char256* PopStack();
	const char* ComposeKillSay(IGameEvent* evt);
	void LoadFile();

	CatVar* v_bEnabled;
	CatVar* v_sFileName;

	char m_KillSays[KILLSAY_ARRAY_MAX_SIZE][KILLSAY_MAX_LENGTH];
	int m_nKillSays;
	KillSayEventListener m_Listener;
	float m_fLastSay;
	char256* m_Stack;
	int m_nStackTop;
};

//void CC_PushKillsayDBG(const CCommand& args);
void CC_KillSay_ReloadFile(const CCommand& args);

DECLARE_HACK_SINGLETON(KillSay);

#endif /* HACKS_KILLSAY_H_ */
