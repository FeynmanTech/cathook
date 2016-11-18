/*
 * Misc.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: nullifiedcat
 */

#include "Misc.h"

#include "../hack.h"
#include "../helpers.h"
#include "../drawing.h"
#include "../localplayer.h"
#include "../logging.h"
#include "../interfaces.h"
#include "../entity.h"
#include "../usercmd.h"
#include "../sdk/in_buttons.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <client_class.h>
#include <icliententitylist.h>
#include <Color.h>
#include <cdll_int.h>
#include <dt_common.h>

void CC_SayLines(const CCommand& args) {
	char cmd[256];
	sprintf(cmd, "say %s", args.ArgS());
	for (int i = 0; i < strlen(cmd); i++) {
		if (cmd[i] == '^') cmd[i] = '\n';
	}
	interfaces::engineClient->ServerCmd(cmd);
}

void CC_Shutdown(const CCommand& args) {
	hack::Shutdown();
}

void CC_AddFriend(const CCommand& args) {
	if (args.ArgC() < 1) return;
	if (!atoi(args[1])) return;
	logging::Info("Adding friend %s", args.Arg(1));
	friends[n_friends++] = atoi(args[1]);
}

void CC_AddRage(const CCommand& args) {
	if (args.ArgC() < 1) return;
	if (!atoi(args[1])) return;
	logging::Info("Adding rage %s", args.Arg(1));
	rage[n_rage++] = atoi(args[1]);
}

void DumpRecvTable(IClientEntity* ent, RecvTable* table, int depth, const char* ft) {
	bool forcetable = strlen(ft);
	if (!ft || !strcmp(ft, table->GetName()))
		logging::Info("==== TABLE: %s", table->GetName());
	for (int i = 0; i < table->GetNumProps(); i++) {
		RecvProp* prop = table->GetProp(i);
		if (!prop) continue;
		if (prop->GetDataTable()) {
			DumpRecvTable(ent, prop->GetDataTable(), depth + 1, ft);
		}
		if (ft && strcmp(ft, table->GetName())) continue;
		switch (prop->GetType()) {
		case SendPropType::DPT_Float:
			logging::Info("%s [0x%04x] = %f", prop->GetName(), prop->GetOffset(), GetEntityValue<float>(ent, prop->GetOffset()));
		break;
		case SendPropType::DPT_Int:
			logging::Info("%s [0x%04x] = %i", prop->GetName(), prop->GetOffset(), GetEntityValue<int>(ent, prop->GetOffset()));
		break;
		case SendPropType::DPT_String:
			logging::Info("%s [0x%04x] = %s", prop->GetName(), prop->GetOffset(), GetEntityValue<char*>(ent, prop->GetOffset()));
		break;
		case SendPropType::DPT_Vector:
			logging::Info("%s [0x%04x] = (%f, %f, %f)", prop->GetName(), prop->GetOffset(), GetEntityValue<float>(ent, prop->GetOffset()), GetEntityValue<float>(ent, prop->GetOffset() + 4), GetEntityValue<float>(ent, prop->GetOffset() + 8));
		break;
		case SendPropType::DPT_VectorXY:
			logging::Info("%s [0x%04x] = (%f, %f)", prop->GetName(), prop->GetOffset(), GetEntityValue<float>(ent, prop->GetOffset()), GetEntityValue<float>(ent, prop->GetOffset() + 4));
		break;
		}

	}
	if (!ft || !strcmp(ft, table->GetName()))
		logging::Info("==== END OF TABLE: %s", table->GetName());
}

void CC_DumpVars(const CCommand& args) {
	if (args.ArgC() < 1) return;
	if (!atoi(args[1])) return;
	int idx = atoi(args[1]);
	IClientEntity* ent = interfaces::entityList->GetClientEntity(idx);
	if (!ent) return;
	ClientClass* clz = ent->GetClientClass();
	logging::Info("Entity %i: %s", ent->entindex(), clz->GetName());
	const char* ft = (args.ArgC() > 1 ? args[2] : "");
	DumpRecvTable(ent, clz->m_pRecvTable, 0, ft);
}

void CC_DumpPlayers(const CCommand& args) {
	for (int i = 0; i < 64 && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(i);
		if (!ent || ent->GetClientClass()->m_ClassID != ClassID::CTFPlayer) continue;
		player_info_t info;
		if (!interfaces::engineClient->GetPlayerInfo(i, &info)) continue;
		logging::Info("[%i] FriendID: %lu ; Name: %s", i, info.friendsID, info.name);
	}
}

ConCommandBase* teamname = 0;

void CC_Teamname(const CCommand& args) {
	if (!teamname) {
		logging::Info("searching");
		teamname = interfaces::cvar->FindCommandBase("tournament_teamname");
	}
	logging::Info("Teamname 0x%08x", teamname);
	if (!teamname) return;

	/*CCommand cmd;
	char* buf = new char[256];
	sprintf(buf, "tournament_teamname %s", args.Arg(1));
	logging::Info("Z");
	cmd.Tokenize(buf);
	logging::Info("A");
	teamname->Dispatch(cmd);
	logging::Info("B");*/
}

void Misc::Create() {
	v_bDbWeaponInfo = CreateConVar("u_misc_debug_weapon", "0", "Debug info: Weapon");
	v_bSemiAuto = CreateConVar("u_misc_semiauto", "0", "Force semi-auto");
	c_SayLine = CreateConCommand("u_say_lines", CC_SayLines, "Uses ^ as a newline character");
	c_Shutdown = CreateConCommand("u_shutdown", CC_Shutdown, "Stops the hack");
	c_AddFriend = CreateConCommand("u_addfriend", CC_AddFriend, "Adds a friend");
	c_AddRage = CreateConCommand("u_addrage", CC_AddRage, "Adds player to rage list");
	c_DumpVars = CreateConCommand("u_dumpent", CC_DumpVars, "Dumps entity data");
	c_DumpPlayers = CreateConCommand("u_dumpplayers", CC_DumpPlayers, "Dumps player data");
	c_Teamname = CreateConCommand("u_teamname", CC_Teamname, "Team name");
}

int sa_switch = 0;

bool Misc::CreateMove(void*, float, CUserCmd* cmd) {
	if (v_bSemiAuto->GetBool()) {
		if (cmd->buttons & IN_ATTACK) {
			if (sa_switch > 5) {
				cmd->buttons = cmd->buttons &~ IN_ATTACK;
			}
			sa_switch++;
		} else {
			sa_switch = 0;
		}
	}
	return true;
}

void Misc::Destroy() {

}

void Misc::PaintTraverse(void*, unsigned int, bool, bool) {
	int y = 10;
	if (v_bDbWeaponInfo->GetBool()) {
		if (g_pLocalPlayer->weapon) {
			IClientEntity* weapon = g_pLocalPlayer->weapon;
			draw::DrawString(10, y, draw::white, draw::black, false, "Weapon: %s [%i]", weapon->GetClientClass()->GetName(), weapon->GetClientClass()->m_ClassID);
			y += 14;
			draw::DrawString(10, y, draw::white, draw::black, false, "flNextPrimaryAttack: %f", GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flNextPrimaryAttack) + 569);
			y += 14;
			draw::DrawString(10, y, draw::white, draw::black, false, "nTickBase: %f", (float)(GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.nTickBase)) / 66);
			y += 14;
			draw::DrawString(10, y, draw::white, draw::black, false, "CanShoot: %i", CanShoot(g_pLocalPlayer->entity));
			y += 14;
			draw::DrawString(10, y, draw::white, draw::black, false, "Decaps: %i", GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.iDecapitations));
			y += 14;
			draw::DrawString(10, y, draw::white, draw::black, false, "Damage: %f", GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flChargedDamage));
	 }
	}
}
