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
#include "../netmessage.h"

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

void DumpRecvTable(CachedEntity* ent, RecvTable* table, int depth, const char* ft) {
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
			logging::Info("%s [0x%04x] = %f", prop->GetName(), prop->GetOffset(), CE_FLOAT(ent, prop->GetOffset()));
		break;
		case SendPropType::DPT_Int:
			logging::Info("%s [0x%04x] = %i", prop->GetName(), prop->GetOffset(), CE_INT(ent, prop->GetOffset()));
		break;
		case SendPropType::DPT_String:
			logging::Info("%s [0x%04x] = %s", prop->GetName(), prop->GetOffset(), CE_VAR(ent, prop->GetOffset(), char*));
		break;
		case SendPropType::DPT_Vector:
			logging::Info("%s [0x%04x] = (%f, %f, %f)", prop->GetName(), prop->GetOffset(), CE_FLOAT(ent, prop->GetOffset()), CE_FLOAT(ent, prop->GetOffset() + 4), CE_FLOAT(ent, prop->GetOffset() + 8));
		break;
		case SendPropType::DPT_VectorXY:
			logging::Info("%s [0x%04x] = (%f, %f)", prop->GetName(), prop->GetOffset(), CE_FLOAT(ent, prop->GetOffset()), CE_FLOAT(ent, prop->GetOffset() + 4));
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
	CachedEntity* ent = ENTITY(idx);
	if (!ent) return;
	ClientClass* clz = RAW_ENT(ent)->GetClientClass();
	logging::Info("Entity %i: %s", ent->m_IDX, clz->GetName());
	const char* ft = (args.ArgC() > 1 ? args[2] : "");
	DumpRecvTable(ent, clz->m_pRecvTable, 0, ft);
}

void CC_ResetLists(const CCommand& args) {
	n_friends = 0;
	n_rage = 0;
}

void CC_DumpPlayers(const CCommand& args) {
	for (int i = 0; i < 64 && i < HIGHEST_ENTITY; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_BAD(ent) || !ent->m_pPlayerInfo) continue;
		logging::Info("[%i] FriendID: %lu ; Name: %s", i, ent->m_pPlayerInfo->friendsID, ent->m_pPlayerInfo->name);
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

void CC_SetName(const CCommand& args) {
	char* name = new char[32];
	snprintf(name, 32, "%s", args.Arg(1));
	if (args.ArgC() > 1 && atoi(args.Arg(2))) {
		for (int i = 0; i < strlen(name); i++) {
			if (name[i] == '^') name[i] = '\n';
		}
	}
	NET_SetConVar setname("name", (const char*)name);
	//logging::Info("Created!");
	INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
	setname.SetNetChannel(ch);
	setname.SetReliable(false);
	//logging::Info("Sending!");
	ch->SendNetMsg(setname, false);
}

void CC_Lockee(const CCommand& args) {
	if (args.ArgC() > 1) {
		LockConCommands(atoi(args.Arg(1)));
	}
	ConVar* name = interfaces::cvar->FindVar("name");
	/*name->m_fnChangeCallback = 0;
	logging::Info("callback: 0x%08x", name->m_fnChangeCallback);
	name->SetValue(g_phMisc->v_strName->GetString());
	name->InternalSetValue(g_phMisc->v_strName->GetString());
	name->m_StringLength = strlen(g_phMisc->v_strName->GetString());
	name->m_pszString = (char*)g_phMisc->v_strName->GetString();
	logging::Info("set %s, value: %s", g_phMisc->v_strName->GetString(), name->GetString());
*/}

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
			logging::Info("%i %f", CE_INT(g_pLocalPlayer->weapon, netvar.AttributeList + i * 12),
					CE_FLOAT(g_pLocalPlayer->weapon, netvar.AttributeList + i * 12 + 4));
		}
	}
}

void CC_SetValue(const CCommand& args) {
	if (args.ArgC() < 2) return;
	ConVar* var = interfaces::cvar->FindVar(args.Arg(1));
	if (!var) return;
	char* value = new char[256];
	snprintf(value, 256, "%s", args.Arg(2));
	if (args.ArgC() > 2 && atoi(args.Arg(3))) {
		for (size_t i = 0; i < strlen(value); i++) {
			if (value[i] == '^') value[i] = '\n';
		}
	}
	var->SetValue(value);
	logging::Info("Set '%s' to '%s'", args.Arg(1), value);
}

Misc::Misc() {
	v_bDebugInfo = CreateConVar(CON_PREFIX "misc_debug", "0", "Debug info");
	c_Name = CreateConCommand(CON_PREFIX "name", CC_SetName, "Sets custom name");
	c_DumpItemAttributes = CreateConCommand(CON_PREFIX "dump_item_attribs", CC_DumpAttribs, "Dump active weapon attributes");
	c_SayLine = CreateConCommand(CON_PREFIX "say_lines", CC_SayLines, "Uses ^ as a newline character");
	c_Shutdown = CreateConCommand(CON_PREFIX "shutdown", CC_Shutdown, "Stops the hack");
	c_AddFriend = CreateConCommand(CON_PREFIX "addfriend", CC_AddFriend, "Adds a friend");
	c_AddRage = CreateConCommand(CON_PREFIX "addrage", CC_AddRage, "Adds player to rage list");
	c_DumpVars = CreateConCommand(CON_PREFIX "dumpent", CC_DumpVars, "Dumps entity data");
	c_DumpPlayers = CreateConCommand(CON_PREFIX "dumpplayers", CC_DumpPlayers, "Dumps player data");
	c_Teamname = CreateConCommand(CON_PREFIX "teamname", CC_Teamname, "Team name");
	c_Lockee = CreateConCommand(CON_PREFIX "lockee", CC_Lockee, "Lock/Unlock commands");
	c_Reset = CreateConCommand(CON_PREFIX "reset_lists", CC_ResetLists, "Remove all friends and rage");
	c_Disconnect = CreateConCommand(CON_PREFIX "disconnect", CC_Disconnect, "Disconnect");
	c_DisconnectVAC = CreateConCommand(CON_PREFIX "disconnect_vac", CC_DisonnectVAC, "Disconnect (VAC)");
	v_bInfoSpam = CreateConVar(CON_PREFIX "info_spam", "0", "Info spam");
	v_bFastCrouch = CreateConVar(CON_PREFIX "fakecrouch", "0", "Fast crouch");
	CreateConCommand(CON_PREFIX "set", CC_SetValue, "Set ConVar value (if third argument is 1 the ^'s will be converted into newlines)");
}

int sa_switch = 0;


bool Misc::CreateMove(void*, float, CUserCmd* cmd) {
	//SetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.iCond, g_pLocalPlayer->cond_0 &~ cond::taunting);
	/*if (false && v_strName->GetString()[0] != '\0') {
		//logging::Info("Name: %s", v_strName->GetString());
		NET_SetConVar setname("name", v_strName->GetString());
		//logging::Info("Created!");
		INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
		setname.SetNetChannel(ch);
		setname.SetReliable(false);
		//logging::Info("Sending!");
		ch->SendNetMsg(setname, false);
		//logging::Info("Sent!");
		//ch->SendNetMsg(*(INetMessage*)&setname);
		//setname.WriteToBuffer(0);
	}*/
	if (v_bFastCrouch->GetBool()) {
		if (interfaces::gvars->tickcount % 4 == 0) {
			cmd->buttons &= ~IN_DUCK;
		}
	}
	static int curindex = 0;
	static int lastsay = 0;
	if (lastsay && lastsay < 200) {
		lastsay++;
	} else lastsay = 0;
	if (v_bInfoSpam->GetBool() && (lastsay == 0)) {
		/*IClientEntity* ent = ENTITY(curindex++);
		if (curindex >= 64) curindex = 0;
		//logging::Info("Making string for %i", curindex);
		if (!ent || ent->IsDormant()) goto breakif;
		//logging::Info("a");
		if (ent->GetClientClass()->m_ClassID != ClassID::CTFPlayer) goto breakif;
		//logging::Info("a");
		if (NET_INT(ent, netvar.iTeamNum) == g_pLocalPlayer->team) goto breakif;
		//logging::Info("Making string for %i", curindex);
		const char* str = MakeInfoString(ent);
		if (str) {
			interfaces::engineClient->ServerCmd(strfmt("say %s", str));
			lastsay = 1;
		}*/
	}
	breakif:
	return true;
}

void Misc::PaintTraverse(void*, unsigned int, bool, bool) {

	if (!v_bDebugInfo->GetBool())return;
	/*if (!interfaces::input->IsButtonDown(ButtonCode_t::KEY_F)) {
		interfaces::baseClient->IN_ActivateMouse();
	} else {
		interfaces::baseClient->IN_DeactivateMouse();
	}*/

		if (CE_GOOD(g_pLocalPlayer->weapon)) {
			AddSideString(colors::white, colors::black, "Weapon: %s [%i]", RAW_ENT(g_pLocalPlayer->weapon)->GetClientClass()->GetName(), g_pLocalPlayer->weapon->m_iClassID);
			AddSideString(colors::white, colors::black, "flNextPrimaryAttack: %f", CE_FLOAT(g_pLocalPlayer->weapon, netvar.flNextPrimaryAttack));
			AddSideString(colors::white, colors::black, "nTickBase: %f", (float)(CE_INT(g_pLocalPlayer->entity, netvar.nTickBase)) * interfaces::gvars->interval_per_tick);
			AddSideString(colors::white, colors::black, "CanShoot: %i", CanShoot());
			AddSideString(colors::white, colors::black, "Decaps: %i", CE_INT(g_pLocalPlayer->entity, netvar.iDecapitations));
			AddSideString(colors::white, colors::black, "Damage: %f", CE_FLOAT(g_pLocalPlayer->weapon, netvar.flChargedDamage));
			AddSideString(colors::white, colors::black, "DefIndex: %i", CE_INT(g_pLocalPlayer->weapon, netvar.iItemDefinitionIndex));
			AddSideString(colors::white, colors::black, "GlobalVars: 0x%08x", interfaces::gvars);
			AddSideString(colors::white, colors::black, "realtime: %f", interfaces::gvars->realtime);
			AddSideString(colors::white, colors::black, "interval_per_tick: %f", interfaces::gvars->interval_per_tick);
			AddSideString(colors::white, colors::black, "ambassador_can_headshot: %i", (interfaces::gvars->curtime - CE_FLOAT(g_pLocalPlayer->weapon, netvar.flLastFireTime)) > 0.95);
			AddSideString(colors::white, colors::black, "WeaponMode: %i", GetWeaponMode(g_pLocalPlayer->entity));
			AddSideString(colors::white, colors::black, "ToGround: %f", DistanceToGround(g_pLocalPlayer->v_Origin));
			AddSideString(colors::white, colors::black, "ServerTime: %f", CE_FLOAT(g_pLocalPlayer->entity, netvar.nTickBase) * interfaces::gvars->interval_per_tick);
			AddSideString(colors::white, colors::black, "CurTime: %f", interfaces::gvars->curtime);
			AddSideString(colors::white, colors::black, "FrameCount: %i", interfaces::gvars->framecount);
			float speed, gravity;
			GetProjectileData(g_pLocalPlayer->weapon, speed, gravity);
			AddSideString(colors::white, colors::black, "Speed: %f", speed);
			AddSideString(colors::white, colors::black, "Gravity: %f", gravity);
			AddSideString(colors::white, colors::black, "IsZoomed: %i", g_pLocalPlayer->bZoomed);
			AddSideString(colors::white, colors::black, "IsThirdPerson: %i", interfaces::iinput->CAM_IsThirdPerson());
			//AddSideString(draw::white, draw::black, "???: %f", NET_FLOAT(g_pLocalPlayer->entity, netvar.test));
			//AddSideString(draw::white, draw::black, "VecPunchAngle: %f %f %f", pa.x, pa.y, pa.z);
			//draw::DrawString(10, y, draw::white, draw::black, false, "VecPunchAngleVel: %f %f %f", pav.x, pav.y, pav.z);
			//y += 14;
			//AddCenterString(draw::font_handle, interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_X), interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_Y), draw::white, L"S\u0FD5");
		}
}
