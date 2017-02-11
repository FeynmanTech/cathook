/*
 * Misc.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: nullifiedcat
 */

#include "Misc.h"

#include <unistd.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>

#include "../hack.h"
#include "../common.h"
#include <checksum_md5.h>
#include "../sdk.h"
#include "../hooks/hookedmethods.h"
#include "../netmessage.h"
#include "../copypasted/CSignature.h"

DEFINE_HACK_SINGLETON(Misc);

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

void DumpRecvTable(CachedEntity* ent, RecvTable* table, int depth, const char* ft, unsigned acc_offset) {
	bool forcetable = ft && strlen(ft);
	if (!forcetable || !strcmp(ft, table->GetName()))
		logging::Info("==== TABLE: %s", table->GetName());
	for (int i = 0; i < table->GetNumProps(); i++) {
		RecvProp* prop = table->GetProp(i);
		if (!prop) continue;
		if (prop->GetDataTable()) {
			DumpRecvTable(ent, prop->GetDataTable(), depth + 1, ft, acc_offset + prop->GetOffset());
		}
		if (forcetable && strcmp(ft, table->GetName())) continue;
		switch (prop->GetType()) {
		case SendPropType::DPT_Float:
			logging::Info("%s [0x%04x] = %f", prop->GetName(), prop->GetOffset(), CE_FLOAT(ent, acc_offset + prop->GetOffset()));
		break;
		case SendPropType::DPT_Int:
			logging::Info("%s [0x%04x] = %i | %u | %hd | %hu", prop->GetName(), prop->GetOffset(), CE_INT(ent, acc_offset + prop->GetOffset()), CE_VAR(ent, acc_offset +  prop->GetOffset(), unsigned int), CE_VAR(ent, acc_offset + prop->GetOffset(), short), CE_VAR(ent, acc_offset + prop->GetOffset(), unsigned short));
		break;
		case SendPropType::DPT_String:
			logging::Info("%s [0x%04x] = %s", prop->GetName(), prop->GetOffset(), CE_VAR(ent, prop->GetOffset(), char*));
		break;
		case SendPropType::DPT_Vector:
			logging::Info("%s [0x%04x] = (%f, %f, %f)", prop->GetName(), prop->GetOffset(), CE_FLOAT(ent, acc_offset + prop->GetOffset()), CE_FLOAT(ent, acc_offset + prop->GetOffset() + 4), CE_FLOAT(ent, acc_offset + prop->GetOffset() + 8));
		break;
		case SendPropType::DPT_VectorXY:
			logging::Info("%s [0x%04x] = (%f, %f)", prop->GetName(), prop->GetOffset(), CE_FLOAT(ent, acc_offset + prop->GetOffset()), CE_FLOAT(ent,acc_offset +  prop->GetOffset() + 4));
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
	if (CE_BAD(ent)) return;
	ClientClass* clz = RAW_ENT(ent)->GetClientClass();
	logging::Info("Entity %i: %s", ent->m_IDX, clz->GetName());
	const char* ft = (args.ArgC() > 1 ? args[2] : 0);
	DumpRecvTable(ent, clz->m_pRecvTable, 0, ft, 0);
}

void CC_ResetLists(const CCommand& args) {
	n_friends = 0;
	n_rage = 0;
}

void CC_DumpPlayers(const CCommand& args) {
	for (int i = 0; i < 33 && i < HIGHEST_ENTITY; i++) {
		player_info_s* pi = new player_info_s;
		interfaces::engineClient->GetPlayerInfo(i, pi);
		logging::Info("[%i] FriendID: %lu ; Name: %s", i, pi->friendsID, pi->name);
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
	delete [] name;
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

void CC_Misc_Disconnect_VAC(const CCommand& args) {
	INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
	if (!ch) {
		logging::Info("No net channel!");
		return;
	}
	//logging::Info("version %i", ch->GetProtocolVersion());
	ch->Shutdown("VAC banned from secure server\n");
}

void CC_DumpAttribs(const CCommand& args) {
	if (CE_GOOD(g_pLocalPlayer->weapon())) {
		for (int i = 0; i < 15; i++) {
			logging::Info("%i %f", CE_INT(g_pLocalPlayer->weapon(), netvar.AttributeList + i * 12),
					CE_FLOAT(g_pLocalPlayer->weapon(), netvar.AttributeList + i * 12 + 4));
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
	delete [] value;
}

void CC_DumpConds(const CCommand& args) {
	if (args.ArgC() < 1) return;
	if (!atoi(args[1])) return;
	int idx = atoi(args[1]);
	CachedEntity* ent = ENTITY(idx);
	if (CE_BAD(ent)) return;
	if (TF2) {
		condition_data_s d1 = CE_VAR(ent, netvar._condition_bits, condition_data_s);
		logging::Info("0x%08x 0x%08x 0x%08x 0x%08x", d1.cond_0, d1.cond_1, d1.cond_2, d1.cond_3);
	}
	condition_data_s d2 = FromOldNetvars(ent);
	logging::Info("0x%08x 0x%08x 0x%08x 0x%08x", d2.cond_0, d2.cond_1, d2.cond_2, d2.cond_3);
}

void Schema_Reload() {
	static uintptr_t InitSchema_s = gSignatures.GetClientSignature("89 44 24 04 89 34 24 74 7B E8 FE DC FB FF 88 45 C7 80 7D C7 00 74 7B 8B 76 0C 39 F3 0F 85 BA 00 00 00 8B 5D D4 83 EB 01 8D 34 9D 00 00 00 00 EB 15 8D 76 00") - 0x54;
	typedef void(*InitSchema_t)(void*, void*, CUtlBuffer& buffer, bool byte, unsigned version);
	static InitSchema_t InitSchema = (InitSchema_t)InitSchema_s;
	static uintptr_t GetItemSchema_s = gSignatures.GetClientSignature("55 89 E5 83 EC 18 89 5D F8 8B 1D ? ? ? ? 89 7D FC 85 DB 74 12 89 D8 8B 7D FC 8B 5D F8 89 EC 5D C3 8D B6 00 00 00 00 C7 04 24 A8 06 00 00 E8 ? ? ? ? B9 AA 01 00 00 89 C3 31 C0 89 DF");
	typedef void*(*GetItemSchema_t)(void);
	static GetItemSchema_t GetItemSchema = (GetItemSchema_t)GetItemSchema_s;//(*(uintptr_t*)GetItemSchema_s + GetItemSchema_s + 4);
	void* itemschema = (GetItemSchema() + 4);
	void* data;
	passwd* pwd = getpwuid(getuid());
	char* user = pwd->pw_name;
	char* path = strfmt("/home/%s/.cathook/items_game.txt", user);
	FILE* file = fopen(path, "r");
	delete [] path;
	fseek(file, 0L, SEEK_END);
	char buffer[4 * 1000 * 1000];
	size_t len = ftell(file);
	rewind(file);
	buffer[len + 1] = 0;
	fread(&buffer, sizeof(char), len, file);
	fclose(file);
	CUtlBuffer buf(&buffer, 4 * 1000 * 1000, 9);
	InitSchema(0, itemschema, buf, false, 0xDEADCA7);
}

void CC_Misc_Schema(const CCommand& args) {
	Schema_Reload();
}

Misc::Misc() {
	if (TF2C) v_bMinigunJump = new CatVar(CV_SWITCH, "minigun_jump", "0", "Minigun Jump", NULL, "Allows you to jump while with minigun spun up");
	v_bDebugInfo = new CatVar(CV_SWITCH, "misc_debug", "0", "Debug info", NULL, "Log stuff to console, enable this if tf2 crashes");
	c_Name = CreateConCommand(CON_PREFIX "name", CC_SetName, "Sets custom name");
	if (TF2) c_DumpItemAttributes = CreateConCommand(CON_PREFIX "dump_item_attribs", CC_DumpAttribs, "Dump active weapon attributes");
	v_bAntiAFK = new CatVar(CV_SWITCH, "noafk", "0", "Anti AFK", NULL, "Sends random stuff to server to not be kicked for idling");
	c_SayLine = CreateConCommand(CON_PREFIX "say_lines", CC_SayLines, "Uses ^ as a newline character");
	c_Shutdown = CreateConCommand(CON_PREFIX "shutdown", CC_Shutdown, "Stops the hack");
	c_AddFriend = CreateConCommand(CON_PREFIX "addfriend", CC_AddFriend, "Adds a friend");
	c_AddRage = CreateConCommand(CON_PREFIX "addrage", CC_AddRage, "Adds player to rage list");
	c_DumpVars = CreateConCommand(CON_PREFIX "dumpent", CC_DumpVars, "Dumps entity data");
	c_DumpPlayers = CreateConCommand(CON_PREFIX "dumpplayers", CC_DumpPlayers, "Dumps player data");
	c_DumpConds = CreateConCommand(CON_PREFIX "dumpconds", CC_DumpConds, "Dumps conditions");
	c_Teamname = CreateConCommand(CON_PREFIX "teamname", CC_Teamname, "Team name");
	c_Lockee = CreateConCommand(CON_PREFIX "lockee", CC_Lockee, "Lock/Unlock commands");
	c_Reset = CreateConCommand(CON_PREFIX "reset_lists", CC_ResetLists, "Remove all friends and rage");
	c_Disconnect = CreateConCommand(CON_PREFIX "disconnect", CC_Disconnect, "Disconnect");
	c_DisconnectVAC = CreateConCommand(CON_PREFIX "disconnect_vac", CC_Misc_Disconnect_VAC, "Disconnect (VAC)");
	v_bInfoSpam = CreateConVar(CON_PREFIX "info_spam", "0", "Info spam");
	v_bFastCrouch = CreateConVar(CON_PREFIX "fakecrouch", "0", "Fast crouch");
	v_bFlashlightSpam = new CatVar(CV_SWITCH, "flashlight_spam", "0", "Flashlight Spam", NULL, "Quickly turns flashlight on and off");
	v_iFakeLag = new CatVar(CV_INT, "fakelag", "0", "Fakelag", NULL, "# of packets jammed", true, 25.0f);
	//v_bDumpEventInfo = CreateConVar(CON_PREFIX "debug_event_info", "0", "Show event info");
	CreateConCommand(CON_PREFIX "set", CC_SetValue, "Set ConVar value (if third argument is 1 the ^'s will be converted into newlines)");
	if (TF2C) {
		v_bTauntSlide = new CatVar(CV_SWITCH, "tauntslide", "0", "Taunt Slide", NULL, "Works only in TF2 Classic!");
		v_bCritHack = new CatVar(CV_SWITCH, "crits", "0", "Crit Hack", NULL, "Works only in TF2 Classic!");
	}
	//v_bDebugCrits = new CatVar(CV_SWITCH, "debug_crits", "0", "Debug Crits", NULL, "???");
	v_bCleanChat = new CatVar(CV_SWITCH, "clean_chat", "1", "Remove newlines from messages", NULL, "Removes newlines from messages, at least it should do that. Might be broken.");
	if (TF2) c_Schema = CreateConCommand(CON_PREFIX "schema", CC_Misc_Schema, "Load item schema");
	if (TF) v_bDebugCrits = new CatVar(CV_SWITCH, "debug_crits", "0", "???", NULL, "???");
	//if (TF2) v_bHookInspect = new CatVar(CV_SWITCH, "hook_inspect", "0", "Hook CanInspect", NULL, "Once enabled, can't be turned off. cathook can't be unloaded after enabling it");
	//interfaces::eventManager->AddListener(&listener, "player_death", false);
}

int sa_switch = 0;

bool s_bCrits;

float clampNC( float val, float minVal, float maxVal )
{
	val = fpmax(minVal, val);
	val = fpmin(maxVal, val);
	return val;
}

float RemapValClampedNC( float val, float A, float B, float C, float D)
{
	if ( A == B )
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	cVal = clampNC( cVal, 0.0f, 1.0f );

	return C + (D - C) * cVal;
}


int ciac_s = false;
float lastcheck = 0;

bool canmeleecrit(IClientEntity* weapon) {
	IClientEntity* owner = RAW_ENT(LOCAL_E);//vfunc<IClientEntity*(*)(IClientEntity*)>(weapon, 0, 0)(weapon);
	if (owner) {

	}
	return false;
}

void Misc::ProcessUserCmd(CUserCmd* cmd) {
	static bool flswitch = false;

	/*if (TF2 && v_bHookInspect->GetBool()) {
		if (CE_GOOD(LOCAL_W)) {
			//logging::Info("%i", vfunc<bool(*)(IClientEntity*)>(RAW_ENT(LOCAL_W), offCanInspect, 0)(RAW_ENT(LOCAL_W)));
			uintptr_t* vtable = *(uintptr_t**)(RAW_ENT(LOCAL_W));
			if (vtable[offCanInspect] != (uintptr_t)CanInspect_hook) {
				uintptr_t patch = (uintptr_t)CanInspect_hook;
				Patch((void*)((uintptr_t)vtable + offCanInspect * 4), &patch, 4);
				//vtable[offCanInspect] = (uintptr_t*)CanInspect_hook;
			}
			//logging::Info("%i", vfunc<bool(*)(IClientEntity*)>(RAW_ENT(LOCAL_W), offCanInspect, 0)(RAW_ENT(LOCAL_W)));
		}
	}
*/
	if (v_bTauntSlide->GetBool())
		RemoveCondition(LOCAL_E, TFCond_Taunting);
	if (TF2C && v_bCritHack->GetBool() && CE_GOOD(LOCAL_W)) {
		static uintptr_t CalcIsAttackCritical_s = gSignatures.GetClientSignature("55 89 E5 56 53 83 EC 10 8B 5D 08 89 1C 24 E8 ? ? ? ? 85 C0 89 C6 74 59 8B 00 89 34 24 FF 90 E0 02 00 00 84 C0 74 4A A1 ? ? ? ? 8B 40 04 3B 83 A8 09 00 00 74 3A");
		typedef void(*CalcIsAttackCritical_t)(IClientEntity*);
		CalcIsAttackCritical_t CIACFn = (CalcIsAttackCritical_t)(CalcIsAttackCritical_s);
		if (cmd->buttons & IN_ATTACK) {
			int tries = 0;
			RandomSeed(MD5_PseudoRandom(cmd->command_number) & 0x7fffffff);
			CIACFn(RAW_ENT(LOCAL_W));
			bool crit = *(bool*)((uintptr_t)RAW_ENT(LOCAL_W) + 2454ul);
			if (!crit) cmd->buttons &= ~IN_ATTACK;
			/*while (!crit && tries < 50) {
				tries++;
				//crit = (vfunc<bool(*)(IClientEntity*)>(RAW_ENT(LOCAL_W), 1764 / 4, 0))(RAW_ENT(LOCAL_W));
			}*/
		}
	}

	if (TF && v_bDebugCrits->GetBool() && CE_GOOD(LOCAL_W)) {
		//static uintptr_t helper = gSignatures.GetClientSignature("55 89 E5 81 EC 88 00 00 00 89 5D F4 8B 5D 08 89 75 F8 89 7D FC 31 FF 89 1C 24 E8 ? ? ? ? 85 C0 89 C6 74 0F 8B 00 89 34 24 FF 90 E0 02 00 00 84 C0 75 14 89 F8 8B 5D F4 8B 75 F8 8B 7D FC 89 EC 5D C3");
		/*if (interfaces::gvars->curtime - lastcheck >= 1.0f) {
			RandomSeed(cmd->random_seed);
			ciac_s = vfunc<int(*)(IClientEntity*)>(RAW_ENT(LOCAL_W), 458, 0)(RAW_ENT(LOCAL_W));
			if (ciac_s) cmd->buttons |= IN_ATTACK;
			lastcheck = interfaces::gvars->curtime;
		}*/
		if (TF2) {
			static uintptr_t critsig = gSignatures.GetClientSignature("55 89 E5 83 EC 28 89 5D F4 8B 5D 08 89 75 F8 89 7D FC 89 1C 24 E8 ? ? ? ? 85 C0 89 C6 74 60 8B 00 89 34 24 FF 90 E0 02 00 00 84 C0 74 51 A1 14 C8 F6 01 8B 40 04 3B 83 30 0B 00 00 74 41 89 83 30 0B 00 00 A1 ? ? ? ? C6 83 0F 0B 00 00 00 83 78 30 05 74 59");
			typedef void(*C_TFWeaponBase__CalcIsAttackCritical_t)(IClientEntity*);
			static C_TFWeaponBase__CalcIsAttackCritical_t ciac = (C_TFWeaponBase__CalcIsAttackCritical_t)critsig;
			if (interfaces::gvars->curtime - lastcheck >= 1.0f) {
				//RandomSeed(cmd->random_seed);
				ciac(RAW_ENT(LOCAL_W));
				//logging::Info("0x%08x", *(unsigned char*)(RAW_ENT(LOCAL_W) + 0x0B0E));
				//ciac_s = *(int*)((uintptr_t)RAW_ENT(LOCAL_W) + 0x0B0Eu) - 256;
				//RandomSeed(cmd->random_seed);
				ciac_s = vfunc<bool(*)(IClientEntity*)>(RAW_ENT(LOCAL_W), 1836 / 4, 0)(RAW_ENT(LOCAL_W));
				lastcheck = interfaces::gvars->curtime;
				if (ciac_s != 0) {
					//logging::Info("Attack!!!");
					cmd->buttons = cmd->buttons | IN_ATTACK;
					AddCenterString(colors::red, "Crit!");
				}
			}
		} else if (TF2C) {


		}
	}
	g_Settings.bSendPackets->SetValue(true);
	if (v_iFakeLag->GetInt()) {
		static int fakelag = 0;
		if (!g_phBunnyhop->m_bFakeLagFix && !(cmd->buttons & IN_ATTACK)) {
			g_Settings.bSendPackets->SetValue(false);
		}
		if (fakelag > v_iFakeLag->GetInt()) {
			fakelag = 0;
			g_Settings.bSendPackets->SetValue(true);
		}
		fakelag++;
	}
	if (v_bFlashlightSpam->GetBool()) {
		if (flswitch && !cmd->impulse) cmd->impulse = 100;
		flswitch = !flswitch;
	}
	if (v_bAntiAFK->GetBool()) {
		cmd->sidemove = RandFloatRange(-450.0, 450.0);
		cmd->forwardmove  = RandFloatRange(-450.0, 450.0);
		cmd->buttons = rand();
	}
	/*if (v_bDebugCrits->GetBool()) {
		static float last = 0.0f;
		if (interfaces::gvars->curtime - last >= 1.0f) {
			RandomSeed(cmd->random_seed);
			float chance = 0.02f * RemapValClampedNC( CE_INT(LOCAL_E, netvar.iCritMult), 0, 255, 1.0, 6 );
			s_bCrits = (RandomInt(0, 10000) < chance * 10000);
			last = interfaces::gvars->curtime;
			RandomSeed(cmd->random_seed);
		}
	}*/
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
		if (ent->GetClientClass()->m_ClassID != g_pClassID->C_Player) goto breakif;
		//logging::Info("a");
		if (NET_INT(ent, netvar.iTeamNum) == g_pLocalPlayer->team) goto breakif;
		//logging::Info("Making string for %i", curindex);
		const char* str = MakeInfoString(ent);
		if (str) {
			interfaces::engineClient->ServerCmd(strfmt("say %s", str));
			lastsay = 1;
		}*/
	}
}

void Misc::Draw() {

	if (!v_bDebugInfo->GetBool())return;
	/*if (!interfaces::input->IsButtonDown(ButtonCode_t::KEY_F)) {
		interfaces::baseClient->IN_ActivateMouse();
	} else {
		interfaces::baseClient->IN_DeactivateMouse();
	}*/

		if (CE_GOOD(g_pLocalPlayer->weapon())) {
			if (v_bDebugCrits->GetBool()) {
				if (TF2) {
					if (!vfunc<bool(*)(IClientEntity*)>(RAW_ENT(LOCAL_W), 465, 0)(RAW_ENT(LOCAL_W)))
						AddCenterString(colors::white, "Random crits are disabled");
					else {
						if (!vfunc<bool(*)(IClientEntity*)>(RAW_ENT(LOCAL_W), 465 + 21, 0)(RAW_ENT(LOCAL_W)))
							AddCenterString(colors::white, "Weapon can't randomly crit");
						else
							AddCenterString(colors::white, "Weapon can randomly crit");
					}
				}


			}
			AddSideString(colors::white, "Weapon: %s [%i]", RAW_ENT(g_pLocalPlayer->weapon())->GetClientClass()->GetName(), g_pLocalPlayer->weapon()->m_iClassID);
			//AddSideString(colors::white, "flNextPrimaryAttack: %f", CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flNextPrimaryAttack));
			//AddSideString(colors::white, "nTickBase: %f", (float)(CE_INT(g_pLocalPlayer->entity, netvar.nTickBase)) * interfaces::gvars->interval_per_tick);
			AddSideString(colors::white, "CanShoot: %i", CanShoot());
			//AddSideString(colors::white, "Damage: %f", CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flChargedDamage));
			if (TF2) AddSideString(colors::white, "DefIndex: %i", CE_INT(g_pLocalPlayer->weapon(), netvar.iItemDefinitionIndex));
			//AddSideString(colors::white, "GlobalVars: 0x%08x", interfaces::gvars);
			//AddSideString(colors::white, "realtime: %f", interfaces::gvars->realtime);
			//AddSideString(colors::white, "interval_per_tick: %f", interfaces::gvars->interval_per_tick);
			//if (TF2) AddSideString(colors::white, "ambassador_can_headshot: %i", (interfaces::gvars->curtime - CE_FLOAT(g_pLocalPlayer->weapon(), netvar.flLastFireTime)) > 0.95);
			AddSideString(colors::white, "WeaponMode: %i", GetWeaponMode(g_pLocalPlayer->entity));
			AddSideString(colors::white, "ToGround: %f", DistanceToGround(g_pLocalPlayer->v_Origin));
			AddSideString(colors::white, "ServerTime: %f", CE_FLOAT(g_pLocalPlayer->entity, netvar.nTickBase) * interfaces::gvars->interval_per_tick);
			AddSideString(colors::white, "CurTime: %f", interfaces::gvars->curtime);
			AddSideString(colors::white, "FrameCount: %i", interfaces::gvars->framecount);
			float speed, gravity;
			GetProjectileData(g_pLocalPlayer->weapon(), speed, gravity);
			AddSideString(colors::white, "Speed: %f", speed);
			AddSideString(colors::white, "Gravity: %f", gravity);
			AddSideString(colors::white, "CIAC: %i", ciac_s);
			if (TF2) AddSideString(colors::white, "Last CIAC: %.2f", lastcheck);
			if (TF2) AddSideString(colors::white, "Bucket: %.2f", *(float*)((uintptr_t)RAW_ENT(LOCAL_W) + 2612u));
			bool ciac = *(bool*)((uintptr_t)RAW_ENT(LOCAL_W) + 2454ul);
			static bool ciacl = false;
			if (ciac != ciacl && ciac) logging::Info("!!!");
			ciacl = ciac;
			if (TF2C) AddSideString(colors::white, "CAAC: %i", ciac);
			//AddSideString(colors::white, "IsZoomed: %i", g_pLocalPlayer->bZoomed);
			//AddSideString(colors::white, "CanHeadshot: %i", CanHeadshot());
			//AddSideString(colors::white, "IsThirdPerson: %i", interfaces::iinput->CAM_IsThirdPerson());
			//if (TF2C) AddSideString(colors::white, "Crits: %i", s_bCrits);
			//if (TF2C) AddSideString(colors::white, "CritMult: %i", RemapValClampedNC( CE_INT(LOCAL_E, netvar.iCritMult), 0, 255, 1.0, 6 ));
			for (int i = 0; i < HIGHEST_ENTITY; i++) {
				CachedEntity* e = ENTITY(i);
				if (CE_GOOD(e)) {
					if (e->m_Type == EntityType::ENTITY_PROJECTILE) {
						//logging::Info("Entity %i [%s]: V %.2f (X: %.2f, Y: %.2f, Z: %.2f) ACC %.2f (X: %.2f, Y: %.2f, Z: %.2f)", i, RAW_ENT(e)->GetClientClass()->GetName(), e->m_vecVelocity.Length(), e->m_vecVelocity.x, e->m_vecVelocity.y, e->m_vecVelocity.z, e->m_vecAcceleration.Length(), e->m_vecAcceleration.x, e->m_vecAcceleration.y, e->m_vecAcceleration.z);
						AddSideString(colors::white, "Entity %i [%s]: V %.2f (X: %.2f, Y: %.2f, Z: %.2f) ACC %.2f (X: %.2f, Y: %.2f, Z: %.2f)", i, RAW_ENT(e)->GetClientClass()->GetName(), e->m_vecVelocity.Length(), e->m_vecVelocity.x, e->m_vecVelocity.y, e->m_vecVelocity.z, e->m_vecAcceleration.Length(), e->m_vecAcceleration.x, e->m_vecAcceleration.y, e->m_vecAcceleration.z);
					}
				}
			}//AddSideString(draw::white, draw::black, "???: %f", NET_FLOAT(g_pLocalPlayer->entity, netvar.test));
			//AddSideString(draw::white, draw::black, "VecPunchAngle: %f %f %f", pa.x, pa.y, pa.z);
			//draw::DrawString(10, y, draw::white, draw::black, false, "VecPunchAngleVel: %f %f %f", pav.x, pav.y, pav.z);
			//y += 14;
			//AddCenterString(draw::font_handle, interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_X), interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_Y), draw::white, L"S\u0FD5");
		}
}
