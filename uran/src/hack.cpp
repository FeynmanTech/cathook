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
#include "hacks/HEsp.h"
//#include "hacks/HGlow.h"
#include "hacks/HPyroBot.h"
#include "hacks/HAimbot.h"
#include "hacks/AntiAim.h"
#include "hacks/Quickscope.h"
#include "hacks/Misc.h"
#include "usercmd.h"
#include "drawing.h"
#include "entity.h"
#include "localplayer.h"
#include "playerresource.h"

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
#include <icvar.h>
#include "copypasted/CSignature.h"
#include "copypasted/Netvar.h"
#include "CDumper.h"

/*
 *  Credits to josh33901 aka F1ssi0N for butifel F1Public and Darkstorm 2015 Linux
 */

typedef void(PaintTraverse_t)(void*, unsigned int, bool, bool);
typedef bool(CreateMove_t)(void*, float, CUserCmd*);

bool hack::invalidated = true;

void hack::Hk_PaintTraverse(void* p, unsigned int vp, bool fr, bool ar) {
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
		for (IHack* i_hack : hack::hacks) {
			i_hack->PaintTraverse(p, vp, fr, ar);
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
					int sw, sh;
					draw::DrawString(ce->m_ESPOrigin.x, ce->m_ESPOrigin.y, str.m_Color, false, str.m_String);
					ce->m_ESPOrigin.y += 14;
				} else {
					draw::DrawString(screen.x, screen.y, str.m_Color, true, str.m_String);
					screen.y += 14;
				}
			}
		}
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
		if (!i_hack->CreateMove(thisptr, inputSample, cmd)) {
			ret = false;
			//g_pLocalPlayer->bUseSilentAngles = true;
		}
	}
	hack::invalidated = false;
	//logging::Info("Inside CreateMove #2");
	/*if (g_pLocalPlayer->bUseSilentAngles) {
		Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
		Vector ang;
		VectorAngles(vsilent, ang);
		float yaw = deg2rad(ang.y - g_pLocalPlayer->v_OrigViewangles.y + cmd->viewangles.y);
		cmd->forwardmove = cos(yaw) * speed;
		cmd->sidemove = sin(yaw) * speed;
	}*/
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
	hack->Create();
	hack::hacks.push_back(hack);
}

// TODO globals
ICvar* g_pCVar = 0;


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
	logging::Info("Dumping NetVars...");
	CDumper dumper;
	dumper.SaveDump();
	logging::Info("Initializing surface...");
	draw::Initialize();
	logging::Info("Adding hacks...");
	SetCVarInterface(interfaces::cvar);
	hack::AddHack(new AntiAim());
	logging::Info("Quickscoping");
	hack::AddHack(new Quickscope());
	hack::AddHack(new HBunnyhop());
	hack::AddHack(new Misc());
	hack::AddHack(new HEsp());
	hack::AddHack(new HAimbot());
	hack::AddHack(new HTrigger());
	//hack::AddHack(new HGlow());
	hack::AddHack(new HPyroBot());
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
	hooks::hkCreateMove->Apply();
	logging::Info("Hooked!");

	logging::Info("Init done!");
}

void hack::Think() {
	// Main code goes here...
	usleep(1000);
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
		i_hack->Destroy();
	}
}
