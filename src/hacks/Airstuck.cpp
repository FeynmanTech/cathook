/*
 * Airstuck.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#include "Airstuck.h"

#include "../common.h"
#include "../sdk.h"

#include "../netmessage.h"

namespace hacks { namespace shared { namespace airstuck {

bool is_stuck = false;

CatVar stuck(CV_SWITCH, "airstuck", "0", "AirStuck enabled");
CatVar stuck_key(CV_KEY, "airstuck_key", "0", "AirStuck key");

void SendNOP() {
	INetChannel* channel = (INetChannel*)g_IEngine->GetNetChannelInfo();
	if (channel) {
		NET_NOP nop;
		nop.SetNetChannel(channel);
		nop.SetReliable(false);
		channel->SendNetMsg(nop);
	}
}

void ProcessUserCmd(CUserCmd* cmd) {
	is_stuck = stuck && g_IInputSystem->IsButtonDown((ButtonCode_t)stuck_key); // TODO this might fail? cast stuff.
	if (cmd->buttons & (IN_ATTACK | IN_ATTACK2)) is_stuck = false;
	if (is_stuck) {
		if (g_pGlobals->tickcount % 60 == 0) {
			SendNOP();
		}
	}
}

void Reset() {
	is_stuck = false;
}

}}}
