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

class TextFile;

class KillSayEventListener : public IGameEventListener2 {
	virtual void FireGameEvent(IGameEvent* event);
};

class KillSay : public IHack {
public:
	DECLARE_HACK_METHODS();
	KillSay();
	~KillSay();
	const char* ComposeKillSay(IGameEvent* evt);

	CatVar* v_bEnabled;
	CatVar* v_sFileName;

	TextFile* m_TextFile;
	KillSayEventListener m_Listener;
};

//void CC_PushKillsayDBG(const CCommand& args);
void CC_KillSay_ReloadFile(const CCommand& args);

DECLARE_HACK_SINGLETON(KillSay);

#endif /* HACKS_KILLSAY_H_ */
