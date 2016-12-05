/*
 * Misc.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: nullifiedcat
 */

#include "Misc.h"

#include <unistd.h>

#include "../hack.h"
#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(Misc);

const char* Misc::GetName() {
	return "MISC";
}

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
	//bool forcetable = strlen(ft);
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

void CC_ResetLists(const CCommand& args) {
	n_friends = 0;
	n_rage = 0;
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

void LockConCommand(const char* name, bool lock) {
	ConCommandBase* cmd = interfaces::cvar->FindCommandBase(name);
	if (lock) {
		// TODO LockConCommand FIX!
		cmd->m_nFlags |= FCVAR_CHEAT;
		cmd->m_nFlags |= FCVAR_REPLICATED;
	} else {
		cmd->m_nFlags &= ~FCVAR_CHEAT;
		cmd->m_nFlags &= ~FCVAR_REPLICATED;
	}
}

void LockConCommands(bool lock) {
	LockConCommand("sv_cheats", lock);
}

ConCommandBase* teamname = 0;

void CC_Lockee(const CCommand& args) {
	if (args.ArgC() > 1) {
		LockConCommands(atoi(args.Arg(1)));
	}
}

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

void CC_SayInfo(const CCommand& args) {
	int id = atoi(args.Arg(1));
	player_info_t info;
	if (!interfaces::engineClient->GetPlayerInfo(id, &info)) return;
	//char* buf = new char[256];

}

void CC_Disconnect(const CCommand& args) {
	INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
	//logging::Info("userid %i rate %i", interfaces::client->GetUserID(), interfaces::client->GetRate());
	//INetChannel* ch = interfaces::client->GetNetChannel();
	if (!ch) {
		logging::Info("No net channel!");
		return;
	}
	logging::Info("version %i", ch->GetProtocolVersion());
	ch->Shutdown(args.ArgS());
}

void CC_DisonnectVAC(const CCommand& args) {
	INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
	if (!ch) {
		logging::Info("No net channel!");
		return;
	}
	//logging::Info("version %i", ch->GetProtocolVersion());
	ch->Shutdown("VAC banned from secure server\n");
}

void CC_DumpAttribs(const CCommand& args) {
	if (g_pLocalPlayer->weapon) {
		for (int i = 0; i < 15; i++) {
			logging::Info("%i %f", GetEntityValue<int>(g_pLocalPlayer->weapon, eoffsets.AttributeList + i * 12),
					GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.AttributeList + i * 12 + 4));
		}
	}
}

Misc::Misc() {
	v_bDbWeaponInfo = CreateConVar("u_misc_debug_weapon", "0", "Debug info: Weapon");
	v_strName = CreateConVar("u_name", "", "Crash the game if changed");
	c_DumpItemAttributes = CreateConCommand("u_dump_item_attribs", CC_DumpAttribs, "Dump active weapon attributes");
	c_SayLine = CreateConCommand("u_say_lines", CC_SayLines, "Uses ^ as a newline character");
	c_Shutdown = CreateConCommand("u_shutdown", CC_Shutdown, "Stops the hack");
	c_AddFriend = CreateConCommand("u_addfriend", CC_AddFriend, "Adds a friend");
	c_AddRage = CreateConCommand("u_addrage", CC_AddRage, "Adds player to rage list");
	c_DumpVars = CreateConCommand("u_dumpent", CC_DumpVars, "Dumps entity data");
	c_DumpPlayers = CreateConCommand("u_dumpplayers", CC_DumpPlayers, "Dumps player data");
	c_Teamname = CreateConCommand("u_teamname", CC_Teamname, "Team name");
	c_Lockee = CreateConCommand("u_lockee", CC_Lockee, "Lock/Unlock commands");
	c_Reset = CreateConCommand("u_reset_lists", CC_ResetLists, "Remove all friends and rage");
	c_Disconnect = CreateConCommand("u_disconnect", CC_Disconnect, "Disconnect");
	c_DisconnectVAC = CreateConCommand("u_disconnect_vac", CC_DisonnectVAC, "Disconnect (VAC)");
	v_bInfoSpam = CreateConVar("u_info_spam", "0", "Info spam");
}

int sa_switch = 0;


bool Misc::CreateMove(void*, float, CUserCmd* cmd) {
	/*if (v_strName->m_StringLength) {
		logging::Info("Creating NetMsg!");
		NET_SetConVar setname("name", v_strName->GetString());
		INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
		setname.SetNetChannel(ch);
		logging::Info("Sending!");
		ch->SendNetMsg(*(INetMessage*)&setname);
	}*/
	static int curindex = 0;
	static int lastsay = 0;
	if (lastsay && lastsay < 200) {
		lastsay++;
	} else lastsay = 0;
	if (v_bInfoSpam->GetBool() && (lastsay == 0)) {
		IClientEntity* ent = interfaces::entityList->GetClientEntity(curindex++);
		if (curindex >= 64) curindex = 0;
		//logging::Info("Making string for %i", curindex);
		if (!ent || ent->IsDormant()) goto breakif;
		//logging::Info("a");
		if (ent->GetClientClass()->m_ClassID != ClassID::CTFPlayer) goto breakif;
		//logging::Info("a");
		if (GetEntityValue<int>(ent, eoffsets.iTeamNum) == g_pLocalPlayer->team) goto breakif;
		//logging::Info("Making string for %i", curindex);
		const char* str = MakeInfoString(ent);
		if (str) {
			interfaces::engineClient->ServerCmd(strfmt("say %s", str));
			lastsay = 1;
		}
	}
	breakif:
	return true;
}

void Misc::PaintTraverse(void*, unsigned int, bool, bool) {

	if (!v_bDbWeaponInfo->GetBool())return;
	/*if (!interfaces::input->IsButtonDown(ButtonCode_t::KEY_F)) {
		interfaces::baseClient->IN_ActivateMouse();
	} else {
		interfaces::baseClient->IN_DeactivateMouse();
	}*/

		if (g_pLocalPlayer->weapon) {
			IClientEntity* weapon = g_pLocalPlayer->weapon;
			AddSideString(draw::white, draw::black, "Weapon: %s [%i]", weapon->GetClientClass()->GetName(), weapon->GetClientClass()->m_ClassID);
			AddSideString(draw::white, draw::black, "flNextPrimaryAttack: %f", GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flNextPrimaryAttack));
			AddSideString(draw::white, draw::black, "nTickBase: %f", (float)(GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.nTickBase)) * interfaces::gvars->interval_per_tick);
			AddSideString(draw::white, draw::black, "CanShoot: %i", BulletTime());
			AddSideString(draw::white, draw::black, "Decaps: %i", GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.iDecapitations));
			AddSideString(draw::white, draw::black, "Damage: %f", GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flChargedDamage));
			AddSideString(draw::white, draw::black, "DefIndex: %i", GetEntityValue<int>(g_pLocalPlayer->weapon, eoffsets.iItemDefinitionIndex));
			AddSideString(draw::white, draw::black, "GlobalVars: 0x%08x", interfaces::gvars);
			AddSideString(draw::white, draw::black, "realtime: %f", interfaces::gvars->realtime);
			AddSideString(draw::white, draw::black, "interval_per_tick: %f", interfaces::gvars->interval_per_tick);
			AddSideString(draw::white, draw::black, "ambassador_can_headshot: %i", (interfaces::gvars->curtime - GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flLastFireTime)) > 0.95);
			AddSideString(draw::white, draw::black, "WeaponMode: %i", GetWeaponMode(g_pLocalPlayer->entity));
			//AddSideString(draw::white, draw::black, "VecPunchAngle: %f %f %f", pa.x, pa.y, pa.z);
			//draw::DrawString(10, y, draw::white, draw::black, false, "VecPunchAngleVel: %f %f %f", pav.x, pav.y, pav.z);
			//y += 14;
			//AddCenterString(draw::font_handle, interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_X), interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_Y), draw::white, L"S\u0FD5");
		}
}
