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
	v_bStuck = CreateConVar(CON_PREFIX "airstuck", "0", "Airstuck (BindToggle that to a key!)");
}

void Airstuck::ProcessUserCmd(CUserCmd* cmd) {
	if (v_bStuck->GetBool()) {
		if (cmd->buttons & (IN_ATTACK | IN_ATTACK2)) {
			return;
		}
		if (interfaces::gvars->tickcount % 60 == 0) {
			INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
			NET_NOP packet;
			packet.SetNetChannel(ch);
			packet.SetReliable(false);
			ch->SendNetMsg(packet);
		}
	}
	return;
}

void Airstuck::OnLevelInit() {
	v_bStuck->SetValue(false);
}

void Airstuck::OnLevelShutdown() {
	v_bStuck->SetValue(false);
}
