/*
 * hack.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "hack.h"

#include <fstream>
#include <iostream>
//#include <cstring>
#include <unistd.h>
#include <link.h>
#include <unordered_map>
#include <cstring>
#include <memory>

#include "interfaces.h"
#include "sharedobj.h"
#include "entitycache.h"
#include "sdk/in_buttons.h"
#include "logging.h"
#include "hooks.h"
#include "hacks/IHack.h"
#include "helpers.h"
#include "hacks/HBunnyhop.h"
#include "hacks/HTrigger.h"
#include "followbot/ipcctl.h"
#include "hacks/AutoReflect.h"
#include "hacks/HEsp.h"
//#include "hacks/HGlow.h"
#include "hacks/HAimbot.h"
#include "hacks/AntiAim.h"
#include "hacks/Misc.h"
#include "hacks/AntiDisguise.h"
#include "usercmd.h"
#include "drawing.h"
#include "hacks/Airstuck.h"
#include "hacks/AutoStrafe.h"
#include "entity.h"
#include "localplayer.h"
#include "playerresource.h"

#include "profiler.h"

#include <csignal>

#include "fixsdk.h"
#include <tier1/convar.h>
#include <igameevents.h>
#include <icliententitylist.h>
#include <cdll_int.h>
#include <engine/IEngineTrace.h>
#include <icliententity.h>
#include <cmodel.h>
#include <client_class.h>
#include <vgui/ISurface.h>
#include <vgui/IPanel.h>
#include <convar.h>
#include <Color.h>
#include <view_shared.h>
#include <icvar.h>
#include "copypasted/CSignature.h"
#include "copypasted/Netvar.h"
#include "CDumper.h"
#include "hacks/FollowBot.h"
#include "globals.h"

/*
 *  Credits to josh33901 aka F1ssi0N for butifel F1Public and Darkstorm 2015 Linux
 */

typedef void(PaintTraverse_t)(void*, unsigned int, bool, bool);
typedef bool(CreateMove_t)(void*, float, CUserCmd*);
typedef void(OverrideView_t)(void*, CViewSetup*);

bool hack::invalidated = true;

void hack::Hk_OverrideView(void* thisptr, CViewSetup* setup) {
	((OverrideView_t*)hooks::hkCreateMove->GetMethod(hooks::offOverrideView))(thisptr, setup);
	if (g_Settings.flForceFOV && g_Settings.flForceFOV->GetBool()) {
		setup->fov = g_Settings.flForceFOV->GetFloat();
	}
}

void hack::Hk_PaintTraverse(void* p, unsigned int vp, bool fr, bool ar) {
	// TODO move it into PrePT hook

	//if (v_bNoZoom->GetBool()) {
	//}

	((PaintTraverse_t*)hooks::hkPaintTraverse->GetMethod(hooks::offPaintTraverse))(p, vp, fr, ar);
	if (!draw::width || !draw::height) {
		interfaces::engineClient->GetScreenSize(draw::width, draw::height);
	}
	if (!draw::panel_top) {
		const char* name = interfaces::panel->GetName(vp);
		if (strlen(name) > 4) {
			if (name[0] == 'M' && name[3] == 'S') {
				draw::panel_top = vp;
				logging::Info("Got top panel: %i", vp);
			}
		}
	}
	if (!interfaces::engineClient->IsInGame()) {
		hack::invalidated = true;
	}
	if (hack::invalidated) return;
	if (draw::panel_top == vp) {
		ResetStrings();
		for (IHack* i_hack : hack::hacks) {
			//PROF_BEGIN();
			i_hack->PaintTraverse(p, vp, fr, ar);
			//PROF_END(strfmt("%s PaintTraverse", i_hack->GetName()));
		}
		Vector screen;
		for (int i = 0; i < gEntityCache.m_nMax && i < interfaces::entityList->GetHighestEntityIndex(); i++) {
			CachedEntity* ce = gEntityCache.GetEntity(i);
			if (!CheckCE(ce)) continue;
			if (ce->m_ESPOrigin.IsZero(1.0f))
				if (!draw::EntityCenterToScreen(ce->m_pEntity, screen)) continue;
			for (int j = 0; j < ce->m_nESPStrings; j++) {
				ESPStringCompound str = ce->GetESPString(j);
				//logging::Info("drawing [idx=%i][ns=%i] %s", i, ce->m_nESPStrings, str.m_String);
				if (!ce->m_ESPOrigin.IsZero(1.0)) {
					draw::DrawString(ce->m_ESPOrigin.x, ce->m_ESPOrigin.y, str.m_Color, str.m_Background, false, str.m_String);
					ce->m_ESPOrigin.y += 12;
				} else {
					draw::DrawString(screen.x, screen.y, str.m_Color, str.m_Background, true, str.m_String);
					screen.y += 12;
				}
			}
		}
		DrawStrings();
	}
}

bool hack::Hk_CreateMove(void* thisptr, float inputSample, CUserCmd* cmd) {
	bool ret = ((CreateMove_t*)hooks::hkCreateMove->GetMethod(hooks::offCreateMove))(thisptr, inputSample, cmd);
	if (!interfaces::engineClient->IsInGame()) {
		hack::invalidated = true;
		return true;
	}
	if (!cmd) return ret;

	g_pPlayerResource->Update();

	g_pLocalPlayer->Update();
	g_pLocalPlayer->v_OrigViewangles = cmd->viewangles;
	gEntityCache.Update();

	//logging::Info("Inside CreateMove");
	//g_pLocalPlayer->bUseSilentAngles = false;
	//logging::Info("Inside CreateMove #1");
	for (IHack* i_hack : hack::hacks) {
		//PROF_BEGIN();
		if (!i_hack->CreateMove(thisptr, inputSample, cmd)) {
			ret = false;
			//g_pLocalPlayer->bUseSilentAngles = true;
		}
		//PROF_END(strfmt("%s CreateMove", i_hack->GetName()));
	}
	hack::invalidated = false;
	//logging::Info("Inside CreateMove #2");
	if (g_pLocalPlayer->bUseSilentAngles) {
		Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
		Vector ang;
		VectorAngles(vsilent, ang);
		float yaw = deg2rad(ang.y - g_pLocalPlayer->v_OrigViewangles.y + cmd->viewangles.y);
		cmd->forwardmove = cos(yaw) * speed;
		cmd->sidemove = sin(yaw) * speed;
		return false;
	}
	//logging::Info("Inside CreateMove #3");
	//logging::Info("viewangles: %f, %f, %f", cmd->viewangles.x, cmd->viewangles.y, cmd->viewangles.z);
	//QAngle a;
	//a.x = g_pLocalPlayer->v_OrigViewangles.x;
	//a.y = g_pLocalPlayer->v_OrigViewangles.y;
	//a.z = g_pLocalPlayer->v_OrigViewangles.z;
	//interfaces::engineClient->SetViewAngles(a);
	//g_pLocalPlayer->bAttackLastTick = (cmd->buttons & (IN_ATTACK | IN_ATTACK2 | IN_USE));
	//logging::Info("Inside CreateMove #4");
	return ret;
}

// TODO globals
std::vector<IHack*> hack::hacks;
bool hack::shutdown = false;

void hack::AddHack(IHack* hack) {
	hack::hacks.push_back(hack);
}

// TODO globals
ICvar* g_pCVar = 0;


void hack::InitHacks() {
	hack::AddHack(g_phAutoStrafe = new AutoStrafe());
	hack::AddHack(g_phAntiAim = new AntiAim());
	hack::AddHack(g_phAntiDisguise = new AntiDisguise());
	hack::AddHack(g_phAutoReflect = new AutoReflect());
	hack::AddHack(g_phFollowBot = new FollowBot());
	hack::AddHack(g_phMisc = new Misc());
	hack::AddHack(g_phAimbot = new HAimbot());
	hack::AddHack(g_phBunnyhop = new HBunnyhop());
	hack::AddHack(g_phEsp = new HEsp());
	hack::AddHack(g_phTrigger = new HTrigger());
	hack::AddHack(g_phAirstuck = new Airstuck());
}

void hack::Initialize() {
	logging::Initialize();
	//std::string test = "";
	logging::Info("Build: " __DATE__ " " __TIME__);
	logging::Info("Loading shared objects...");
	sharedobj::LoadAllSharedObjects();
	/* TODO */
	//logging::Info("TRYIN' SHIT");
	//CGlowObject* gom = (CGlowObject*)((uintptr_t)sharedobj::client->lmap->l_addr + 0x01FC6260);
	//logging::Info("MANAGER?? 0x%08f", gom);
	logging::Info("Creating interfaces...");
	interfaces::CreateInterfaces();
	logging::Info("Interfaces created!");
	logging::Info("APPID: %i", interfaces::engineClient->GetAppID());
	//logging::Info("Dumping NetVars...");
	//CDumper dumper;
	//dumper.SaveDump();
	logging::Info("Initializing surface...");
	draw::Initialize();
	logging::Info("Adding hacks...");
	SetCVarInterface(interfaces::cvar);
	hack::InitHacks();
	logging::Info("Init global settings");
	g_Settings.Init();
	ConVar_Register();
	logging::Info("Initializing NetVar tree...");
	gNetvars.init();
	logging::Info("Initializing entity offsets...");
	InitEntityOffsets();

	g_pLocalPlayer = new LocalPlayer();
	g_pPlayerResource = new TFPlayerResource();

	logging::Info("Hooking PaintTraverse...");
	hooks::hkPaintTraverse = new hooks::VMTHook();
	hooks::hkPaintTraverse->Init(interfaces::panel, 0);
	hooks::hkPaintTraverse->HookMethod((void*)&hack::Hk_PaintTraverse, hooks::offPaintTraverse);
	hooks::hkPaintTraverse->Apply();
	logging::Info("Hooking CreateMove...");
	hooks::hkCreateMove = new hooks::VMTHook();
	uintptr_t* clientMode = 0;
	while(!(clientMode = **(uintptr_t***)((uintptr_t)((*(void***)interfaces::baseClient)[10]) + 1))) {
		sleep(1);
	}
	hooks::hkCreateMove->Init((void*)clientMode, 0);
	hooks::hkCreateMove->HookMethod((void*)&hack::Hk_CreateMove, hooks::offCreateMove);
	logging::Info("Hooking OverrideView...");
	hooks::hkCreateMove->HookMethod((void*)&hack::Hk_OverrideView, hooks::offOverrideView);
	hooks::hkCreateMove->Apply();
	logging::Info("Hooked!");
	InitStrings();
	logging::Info("Init done!");
}

void hack::Think() {
	//logging::Info("Hack::Think");
	// Fucking TODo
	if (g_phFollowBot->v_bEnabled->GetBool()) {
		ipc_bot_seg* seg_g = g_phFollowBot->m_pIPC->GetBotCommand(0);
		ipc_bot_seg* seg_l = g_phFollowBot->m_pIPC->GetBotCommand(g_phFollowBot->m_pIPC->bot_id);

		if (seg_g == 0) {
			logging::Info("!!! seg_g == 0 !!!");
		}
		if (seg_l == 0) {
			logging::Info("!!! seg_l == 0 !!!");
		}

		if (seg_g && seg_g->command_number > g_phFollowBot->last_command_global) {
			logging::Info("Executing `%s`", seg_g->command_buffer);
			if (g_phFollowBot->last_command_global) interfaces::engineClient->ExecuteClientCmd(seg_g->command_buffer);
			g_phFollowBot->last_command_global = seg_g->command_number;
		}
		if (seg_l && seg_l->command_number > g_phFollowBot->last_command_local) {
			logging::Info("Executing `%s`", seg_l->command_buffer);
			if (g_phFollowBot->last_command_local) interfaces::engineClient->ExecuteClientCmd(seg_l->command_buffer);
			g_phFollowBot->last_command_local = seg_l->command_number;
		}
	}
	usleep(250000);
}

void hack::Shutdown() {
	if (hack::shutdown) return;
	hack::shutdown = true;
	logging::Info("Shutting down...");
	logging::Shutdown();
	ConVar_Unregister();
	hooks::hkPaintTraverse->Kill();
	hooks::hkCreateMove->Kill();
	for (IHack* i_hack : hack::hacks) {
		delete i_hack;
	}
}
