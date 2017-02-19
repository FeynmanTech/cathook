/*
 * KillSay.h
 *
 *  Created on: Jan 19, 2017
 *      Author: nullifiedcat
 */

#ifndef HACKS_KILLSAY_H_
#define HACKS_KILLSAY_H_

#include <igameevents.h>

namespace hacks { namespace shared { namespace killsay {

class DeathListener : public IGameEventListener2 {
public:
	virtual void FireGameEvent(IGameEvent* event);
	~DeathListener();
};

std::string ComposeMessage(IGameEvent* event);

extern CatVar enabled;
extern CatVar filename;
extern CatVar compatability;
extern CatCommand reload;

extern DeathListener listener;
extern TextFile killsays;

}}}


#endif /* HACKS_KILLSAY_H_ */
