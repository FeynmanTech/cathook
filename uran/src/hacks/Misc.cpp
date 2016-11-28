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
#include <iclient.h>
#include <inetchannel.h>
#include <cdll_int.h>
#include <iconvar.h>
#include <dt_common.h>
#include <inputsystem/iinputsystem.h>

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
		cmd->m_nFlags |= FCVAR_CHEAT;
	} else {
		cmd->m_nFlags &= ~FCVAR_CHEAT;
	}
}

void LockConCommands(bool lock) {
	LockConCommand("thirdperson", lock);
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
	char* buf = new char[256];

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

Misc::Misc() {
	v_bDbWeaponInfo = CreateConVar("u_misc_debug_weapon", "0", "Debug info: Weapon");
	v_bSemiAuto = CreateConVar("u_misc_semiauto", "0", "Force semi-auto");
	v_bNoZoom = CreateConVar("u_nozoom", "0", "No-Zoom");
	v_bNoFlinch = CreateConVar("u_noflinch", "0", "No-Flinch");
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
	if (v_bNoZoom->GetBool()) {
		if (g_pLocalPlayer->entity) {
			bool zoomed = (g_pLocalPlayer->cond_0 & cond::zoomed);
			if (zoomed) {
				SetEntityValue(g_pLocalPlayer->entity, eoffsets.iCond, g_pLocalPlayer->cond_0 &~ cond::zoomed);
			}
		}
	}
	return true;
}

void Misc::PaintTraverse(void*, unsigned int, bool, bool) {
	if (v_bNoZoom->GetBool()) {
		if (g_pLocalPlayer->entity) {
			bool zoomed = (g_pLocalPlayer->cond_0 & cond::zoomed);
			if (zoomed) {
				SetEntityValue(g_pLocalPlayer->entity, eoffsets.iCond, g_pLocalPlayer->cond_0 &~ cond::zoomed);
			}
		}
	}

	if (v_bNoFlinch->GetBool()) {
		static Vector oldPunchAngles = Vector();
		Vector punchAngles = GetEntityValue<Vector>(g_pLocalPlayer->entity, eoffsets.vecPunchAngle);
		QAngle viewAngles;
		interfaces::engineClient->GetViewAngles(viewAngles);
		viewAngles -= VectorToQAngle(punchAngles - oldPunchAngles);
		oldPunchAngles = punchAngles;
		interfaces::engineClient->SetViewAngles(viewAngles);
	}


	if (!v_bDbWeaponInfo->GetBool())return;
	/*if (!interfaces::input->IsButtonDown(ButtonCode_t::KEY_F)) {
		interfaces::baseClient->IN_ActivateMouse();
	} else {
		interfaces::baseClient->IN_DeactivateMouse();
	}*/

		if (g_pLocalPlayer->weapon) {
			IClientEntity* weapon = g_pLocalPlayer->weapon;
			AddSideString(draw::white, draw::black, "Weapon: %s [%i]", weapon->GetClientClass()->GetName(), weapon->GetClientClass()->m_ClassID);
			AddSideString(draw::white, draw::black, "flNextPrimaryAttack: %f", GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flNextPrimaryAttack) + 569);
			AddSideString(draw::white, draw::black, "nTickBase: %f", (float)(GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.nTickBase)) / 66);
			AddSideString(draw::white, draw::black, "CanShoot: %i", CanShoot(g_pLocalPlayer->entity));
			AddSideString(draw::white, draw::black, "Decaps: %i", GetEntityValue<int>(g_pLocalPlayer->entity, eoffsets.iDecapitations));
			AddSideString(draw::white, draw::black, "Damage: %f", GetEntityValue<float>(g_pLocalPlayer->weapon, eoffsets.flChargedDamage));
			AddSideString(draw::white, draw::black, "DefIndex: %i", GetEntityValue<int>(g_pLocalPlayer->weapon, eoffsets.iItemDefinitionIndex));

			//AddSideString(draw::white, draw::black, "VecPunchAngle: %f %f %f", pa.x, pa.y, pa.z);
			//draw::DrawString(10, y, draw::white, draw::black, false, "VecPunchAngleVel: %f %f %f", pav.x, pav.y, pav.z);
			//y += 14;
			//AddCenterString(draw::font_handle, interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_X), interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_Y), draw::white, L"S\u0FD5");
		}
}

Misc* g_phMisc = 0;
