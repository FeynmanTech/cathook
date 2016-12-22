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

DEFINE_HACK_SINGLETON(Airstuck);

Airstuck::Airstuck() {
	v_bStuck = CreateConVar(CON_PREFIX "airstuck", "0", "Airstuck");
}

const char* Airstuck::GetName() {
	return "AIRSTUCK";
}

bool Airstuck::CreateMove(void*, float, CUserCmd* cmd) {
	if (v_bStuck->GetBool()) {
		if (cmd->buttons & (IN_ATTACK | IN_ATTACK2)) {
			return true;
		}
		if (interfaces::gvars->tickcount % 60 == 0) {
			INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
			NET_NOP packet;
			packet.SetNetChannel(ch);
			packet.SetReliable(false);
			ch->SendNetMsg(packet);
		}
	}
	return true;
}

void Airstuck::PaintTraverse(void*, unsigned int, bool, bool) {};
