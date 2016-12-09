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
	v_bStuck = CreateConVar("u_airstuck", "0", "Toggle airstuck");
}

const char* Airstuck::GetName() {
	return "AIRSTUCK";
}

bool Airstuck::CreateMove(void*, float, CUserCmd* cmd) {
	// TODO Airstuck.
	interfaces::cvar->FindVar("net_fakeloss")->SetValue(0);
	interfaces::cvar->FindVar("host_timescale")->SetValue(1.0f);
	if (v_bStuck->GetBool()) {
		if (cmd->buttons & (IN_ATTACK | IN_ATTACK2)) {
			return true;
		}
		//cmd->tick_count = 0xFFFFF;
		//INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
		//NET_Tick tick;
		//ch->SendNetMsg((INetMessage*)&tick);
		//cmd->tick_count = 0xFFFF;
		//interfaces::cvar->FindVar("net_fakeloss")->SetValue(99);
		//interfaces::cvar->FindVar("net_fakeloss")->SetValue(100.0f);
		if (interfaces::gvars->tickcount % 60 == 0) {
			INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
			//IClient* client = (IClient*)ch->GetMsgHandler();
			//IServer* server = client->GetServer();
			//logging::Info("Client: 0x%08x Server: 0x%08x");
			NET_NOP packet;
			//logging::Info("NOP sent!");
			//NET_SignonState packet(6, 1);
			//logging::Info("Created!");
			packet.SetNetChannel(ch);
			packet.SetReliable(false);
			//logging::Info("Sending!");
			ch->SendNetMsg(packet);
			//packet.WriteToBuffer(buffer);
		}
	}
	return true;
}

void Airstuck::PaintTraverse(void*, unsigned int, bool, bool) {};
