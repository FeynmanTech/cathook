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
#include <csignal>

// All Hacks
#include "hacks/IHack.h"

#include "hacks/Aimbot.h"
#include "hacks/Airstuck.h"
#include "hacks/AntiAim.h"
#include "hacks/AntiDisguise.h"
#include "hacks/AutoHeal.h"
#include "hacks/AutoReflect.h"
#include "hacks/AutoSticky.h"
#include "hacks/AutoStrafe.h"
#include "hacks/Bunnyhop.h"
#include "hacks/ESP.h"
#include "hacks/FollowBot.h"
#include "hacks/HuntsmanCompensation.h"
#include "hacks/Misc.h"
#include "hacks/SpyAlert.h"
#include "hacks/Trigger.h"

#include "followbot/ipcctl.h"

#include "common.h"
#include "sharedobj.h"
#include "hooks.h"
#include "targeting/ITargetSystem.h"
#include "profiler.h"

#include "sdk.h"
#include "copypasted/CSignature.h"
#include "copypasted/Netvar.h"
#include "CDumper.h"

/*
 *  Credits to josh33901 aka F1ssi0N for butifel F1Public and Darkstorm 2015 Linux
 */

typedef void(PaintTraverse_t)(void*, unsigned int, bool, bool);
typedef bool(CreateMove_t)(void*, float, CUserCmd*);
typedef void(OverrideView_t)(void*, CViewSetup*);
typedef void(FrameStageNotify_t)(void*, int);
typedef bool(DispatchUserMessage_t)(void*, int, bf_read&);

bool hack::invalidated = true;

void hack::Hk_OverrideView(void* thisptr, CViewSetup* setup) {
	((OverrideView_t*)hooks::hkClientMode->GetMethod(hooks::offOverrideView))(thisptr, setup);
	if (g_Settings.flForceFOV && g_Settings.flForceFOV->GetBool()) {
		setup->fov = g_Settings.flForceFOV->GetFloat();
	}
}

void hack::Hk_PaintTraverse(void* p, unsigned int vp, bool fr, bool ar) {
	((PaintTraverse_t*)hooks::hkPanel->GetMethod(hooks::offPaintTraverse))(p, vp, fr, ar);
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
		if (g_Settings.bShowLogo->GetBool()) {
			AddSideString(colors::green, colors::black, "cathook by d4rkc4t");
			AddSideString(colors::green, colors::black, "build " __DATE__ " " __TIME__);
		}
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
					ce->m_ESPOrigin.y += 11;
				} else {
					draw::DrawString(screen.x, screen.y, str.m_Color, str.m_Background, true, str.m_String);
					screen.y += 11;
				}
			}
		}
		DrawStrings();
	}
}

typedef bool(CanPacket_t)(void* thisptr);
bool Hk_CanPacket(void* thisptr) {
	return false;//g_Settings.bSendPackets->GetBool() && ((CanPacket_t*)hooks::hkNetChannel->GetMethod(hooks::offCanPacket))(thisptr);
}

bool hack::Hk_CreateMove(void* thisptr, float inputSample, CUserCmd* cmd) {
	bool ret = ((CreateMove_t*)hooks::hkClientMode->GetMethod(hooks::offCreateMove))(thisptr, inputSample, cmd);
	if (!interfaces::engineClient->IsInGame()) {
		hack::invalidated = true;
		return true;
	}

	/*INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
	if (ch && !hooks::IsHooked(ch)) {
		logging::Info("NCI not hooked!");
		hooks::hkNetChannel = new hooks::VMTHook();
		hooks::hkNetChannel->Init(ch, 0);
		hooks::hkNetChannel->HookMethod((void*)Hk_CanPacket, hooks::offCanPacket);
		hooks::hkNetChannel->Apply();
		logging::Info("Applied!");
	}
	logging::Info("Can packet? %i", ch->CanPacket());*/
	if (!cmd) return ret;

	g_pPlayerResource->Update();

	g_pLocalPlayer->Update();
	g_pLocalPlayer->v_OrigViewangles = cmd->viewangles;
	gEntityCache.Update();

	CREATE_MOVE(Bunnyhop);
	//RunEnginePrediction(g_pLocalPlayer->entity, cmd);
	CREATE_MOVE(ESP);
	CREATE_MOVE(Aimbot);
	CREATE_MOVE(Airstuck);
	CREATE_MOVE(AntiAim);
	CREATE_MOVE(AntiDisguise);
	CREATE_MOVE(AutoHeal);
	CREATE_MOVE(AutoSticky);
	CREATE_MOVE(AutoReflect);
	CREATE_MOVE(AutoStrafe);
	CREATE_MOVE(FollowBot);
	CREATE_MOVE(Misc);
	CREATE_MOVE(Triggerbot);
	CREATE_MOVE(HuntsmanCompensation);

	/*for (IHack* i_hack : hack::hacks) {
		if (!i_hack->CreateMove(thisptr, inputSample, cmd)) {
			ret = false;
		}
	}*/
	hack::invalidated = false;
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
	return ret;
}

void hack::Hk_FrameStageNotify(void* thisptr, int stage) {
	//logging::Info("FrameStageNotify %i", stage);
	// Ambassador to festive ambassador changer. simple.
	if (stage == 5 && g_Settings.bNoFlinch->GetBool()) {
		static Vector oldPunchAngles = Vector();
		Vector punchAngles = GetEntityValue<Vector>(g_pLocalPlayer->entity, eoffsets.vecPunchAngle);
		QAngle viewAngles;
		interfaces::engineClient->GetViewAngles(viewAngles);
		viewAngles -= VectorToQAngle(punchAngles - oldPunchAngles);
		oldPunchAngles = punchAngles;
		interfaces::engineClient->SetViewAngles(viewAngles);
	}
	if (g_pLocalPlayer->weapon) {
		int defidx = GetEntityValue<int>(g_pLocalPlayer->weapon, eoffsets.iItemDefinitionIndex);
		if (defidx == 61) {
			SetEntityValue<int>(g_pLocalPlayer->weapon, eoffsets.iItemDefinitionIndex, 1006);
		}
	}
	if (g_Settings.bNoZoom->GetBool()) {
		if (g_pLocalPlayer->entity) {
			bool zoomed = (g_pLocalPlayer->cond_0 & cond::zoomed);
			if (zoomed) {
				SetEntityValue(g_pLocalPlayer->entity, eoffsets.iCond, g_pLocalPlayer->cond_0 &~ cond::zoomed);
			}
		}
	}
	((FrameStageNotify_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify))(thisptr, stage);
}

bool hack::Hk_DispatchUserMessage(void* thisptr, int type, bf_read& buf) {
	//logging::Info("message %i", type);
	return ((DispatchUserMessage_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify + 1))(thisptr, type, buf);
}

std::vector<IHack*> hack::hacks;
bool hack::shutdown = false;

void hack::AddHack(IHack* hack) {
	hack::hacks.push_back(hack);
}

ICvar* g_pCVar = 0;

void hack::InitHacks() {
	ADD_HACK(AutoStrafe);
	ADD_HACK(AntiAim);
	ADD_HACK(AntiDisguise);
	ADD_HACK(AutoReflect);
	ADD_HACK(FollowBot);
	ADD_HACK(Misc);
	ADD_HACK(Aimbot);
	ADD_HACK(Bunnyhop);
	ADD_HACK(ESP);
	ADD_HACK(Triggerbot);
	ADD_HACK(AutoSticky);
	ADD_HACK(Airstuck);
	ADD_HACK(AutoHeal);
	ADD_HACK(HuntsmanCompensation);
	ADD_HACK(SpyAlert);
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
	InitTargetingConVars();
	ConVar_Register();
	logging::Info("Initializing NetVar tree...");
	gNetvars.init();
	logging::Info("Initializing entity offsets...");
	InitEntityOffsets();

	g_pLocalPlayer = new LocalPlayer();
	g_pPlayerResource = new TFPlayerResource();

	logging::Info("Hooking methods...");
	hooks::hkPanel = new hooks::VMTHook();
	hooks::hkPanel->Init(interfaces::panel, 0);
	hooks::hkPanel->HookMethod((void*)&hack::Hk_PaintTraverse, hooks::offPaintTraverse);
	hooks::hkPanel->Apply();
	hooks::hkClientMode = new hooks::VMTHook();
	uintptr_t* clientMode = 0;
	while(!(clientMode = **(uintptr_t***)((uintptr_t)((*(void***)interfaces::baseClient)[10]) + 1))) {
		sleep(1);
	}
	hooks::hkClientMode->Init((void*)clientMode, 0);
	hooks::hkClientMode->HookMethod((void*)&hack::Hk_CreateMove, hooks::offCreateMove);
	hooks::hkClientMode->HookMethod((void*)&hack::Hk_OverrideView, hooks::offOverrideView);
	hooks::hkClientMode->Apply();
	hooks::hkClient = new hooks::VMTHook();
	hooks::hkClient->Init((void*)interfaces::baseClient, 0);
	hooks::hkClient->HookMethod((void*)&hack::Hk_FrameStageNotify, hooks::offFrameStageNotify);
	hooks::hkClient->HookMethod((void*)&hack::Hk_DispatchUserMessage, hooks::offFrameStageNotify + 1);
	hooks::hkClient->Apply();
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
	hooks::hkPanel->Kill();
	hooks::hkClientMode->Kill();
	hooks::hkClient->Kill();
	for (IHack* i_hack : hack::hacks) {
		delete i_hack;
	}
}
