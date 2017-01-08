/*
 * hack.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "hack.h"

#include <fstream>
#include <iostream>
#include <sys/prctl.h>
//#include <cstring>
#include <unistd.h>
#include <link.h>
#include <unordered_map>
#include <cstring>
#include <memory>
#include "segvcatch/segvcatch.h"
#include <csignal>
#include <sys/sysinfo.h>

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

#include "common.h"
#include "sharedobj.h"
#include "hooks.h"
#include "netmessage.h"
#include "targeting/ITargetSystem.h"
#include "profiler.h"
#include "gui/gui.h"
#include "gui/controls.h"
#include "cvwrapper.h"

#include "sdk.h"
#include "vfunc.h"
#include "copypasted/CSignature.h"
#include "copypasted/Netvar.h"
#include "CDumper.h"
#include "ipc/ipcctl.h"

/*
 *  Credits to josh33901 aka F1ssi0N for butifel F1Public and Darkstorm 2015 Linux
 */

typedef void(PaintTraverse_t)(void*, unsigned int, bool, bool);
typedef bool(CreateMove_t)(void*, float, CUserCmd*);
typedef void(OverrideView_t)(void*, CViewSetup*);
typedef void(FrameStageNotify_t)(void*, int);
typedef bool(DispatchUserMessage_t)(void*, int, bf_read&);

Vector last_angles(0.0f, 0.0f, 0.0f);

void hack::Hk_OverrideView(void* thisptr, CViewSetup* setup) {
	SEGV_BEGIN;
	((OverrideView_t*)hooks::hkClientMode->GetMethod(hooks::offOverrideView))(thisptr, setup);
	if (!g_Settings.bHackEnabled->GetBool()) return;
	if (g_Settings.flForceFOV && g_Settings.flForceFOV->GetBool()) {
		setup->fov = g_Settings.flForceFOV->GetFloat();
	}
	SEGV_END;
}

void hack::Hk_PaintTraverse(void* p, unsigned int vp, bool fr, bool ar) {
	if (!segvcatch::handler_fpe || !segvcatch::handler_segv) {
		segvcatch::init_segv();
		segvcatch::init_fpe();
		logging::Info("segvcatch init!");
	}

	SEGV_BEGIN;
	SAFE_CALL(((PaintTraverse_t*)hooks::hkPanel->GetMethod(hooks::offPaintTraverse))(p, vp, fr, ar));
	if (!g_Settings.bHackEnabled->GetBool()) return;
	// Because of single-multi thread shit I'm gonna put this thing riiiight here.
	if (g_phFollowBot->v_bEnabled->GetBool()) {
		ipc_client_seg* seg_g = g_phFollowBot->m_pIPC->GetClientSegment(0);
		ipc_client_seg* seg_l = g_phFollowBot->m_pIPC->GetClientSegment(g_phFollowBot->m_pIPC->client_id);

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

	if (g_Settings.bNoVisuals->GetBool()) return;

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
		g_Settings.bInvalid = true;
	}
	if (g_Settings.bInvalid) return;
	if (CE_BAD(g_pLocalPlayer->entity)) return;
	if (draw::panel_top == vp) {
		ResetStrings();
		if (g_Settings.bShowLogo->GetBool()) {
			AddSideString(colors::green, colors::black, "cathook by d4rkc4t");
			AddSideString(colors::red, colors::black, "DEVELOPER BUILD");
		}
		for (IHack* i_hack : hack::hacks) {
			//PROF_BEGIN();
			SEGV_BEGIN
				i_hack->PaintTraverse(p, vp, fr, ar);
			SEGV_END_INFO("Hack PaintTraverse")
			//PROF_END(strfmt("%s PaintTraverse", i_hack->GetName()));
		}
		Vector screen;
		for (int i = 0; i < HIGHEST_ENTITY; i++) {
			CachedEntity* ce = gEntityCache.GetEntity(i);
			if (CE_BAD(ce)) continue;
			if (ce->m_ESPOrigin.IsZero(1.0f))
				if (!draw::EntityCenterToScreen(ce, screen)) continue;
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
#if GUI_ENABLED == true
		g_pGUI->UpdateKeys();
		g_pGUI->Draw();
#endif
		DrawStrings();
	}
	SEGV_END;
}

typedef bool(CanPacket_t)(void* thisptr);
bool Hk_CanPacket(void* thisptr) {
	SEGV_BEGIN;
	return g_Settings.bSendPackets->GetBool() && ((CanPacket_t*)hooks::hkNetChannel->GetMethod(hooks::offCanPacket))(thisptr);
	SEGV_END;
	return false;
}

typedef int(IN_KeyEvent_t)(void* thisptr, int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding);
int Hk_IN_KeyEvent(void* thisptr, int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding) {
	SEGV_BEGIN;
	if (eventcode == 1) {
		if (g_pGUI->KeyEvent(keynum)) return 1;
	}
	return ((IN_KeyEvent_t*)hooks::hkClient->GetMethod(hooks::offKeyEvent))(thisptr, eventcode, keynum, pszCurrentBinding);
	SEGV_END;
	return 0;
}

typedef bool(SendNetMsg_t)(void* thisptr, INetMessage& msg, bool forcereliable, bool voice);
bool Hk_SendNetMsg(void* thisptr, INetMessage& msg, bool bForceReliable = false, bool bVoice = false) {
	SEGV_BEGIN;

	//logging::Info("Sending NetMsg! %i", msg.GetType());
	if (g_phAirstuck->v_bStuck->GetBool() && g_Settings.bHackEnabled->GetBool()) {
		switch (msg.GetType()) {
		case net_NOP:
		case net_SignonState:
		case net_StringCmd:
			break;
		default:
			return false;
		}
	}
	return ((SendNetMsg_t*)hooks::hkNetChannel->GetMethod(hooks::offSendNetMsg))(thisptr, msg, bForceReliable, bVoice);
	SEGV_END;
	return false;
}

typedef void(Shutdown_t)(void*, const char*);
void Hk_Shutdown(void* thisptr, const char* reason) {
	SEGV_BEGIN;
	if (g_Settings.bHackEnabled->GetBool()) {
		const char* new_reason = reason;
		if (g_Settings.sDisconnectMsg->m_StringLength > 3) {
			new_reason = g_Settings.sDisconnectMsg->GetString();
		}
		((Shutdown_t*)hooks::hkNetChannel->GetMethod(hooks::offShutdown))(thisptr, new_reason);
	} else {
		((Shutdown_t*)hooks::hkNetChannel->GetMethod(hooks::offShutdown))(thisptr, reason);
	}
	SEGV_END;
}

bool hack::Hk_CreateMove(void* thisptr, float inputSample, CUserCmd* cmd) {
	SEGV_BEGIN;


	bool ret = ((CreateMove_t*)hooks::hkClientMode->GetMethod(hooks::offCreateMove))(thisptr, inputSample, cmd);


	if (!g_Settings.bHackEnabled->GetBool()) return ret;

	if (!interfaces::engineClient->IsInGame()) {
		g_Settings.bInvalid = true;
		return true;
	}

	PROF_BEGIN();

	INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
	if (ch && !hooks::IsHooked((void*)((uintptr_t)ch))) {
		logging::Info("Hooking INetChannel!");
		hooks::hkNetChannel = new hooks::VMTHook();
		hooks::hkNetChannel->Init(ch, 0);
		hooks::hkNetChannel->HookMethod((void*)Hk_CanPacket, hooks::offCanPacket);
		hooks::hkNetChannel->HookMethod((void*)Hk_SendNetMsg, hooks::offSendNetMsg);
		hooks::hkNetChannel->HookMethod((void*)Hk_Shutdown, hooks::offShutdown);
		hooks::hkNetChannel->Apply();
		logging::Info("NetChannel Hooked!");
	}
	//logging::Info("canpacket: %i", ch->CanPacket());
	//if (!cmd) return ret;

	bool time_replaced = false;
	float curtime_old;
	if (CE_GOOD(g_pLocalPlayer->entity)) {
		float servertime = (float)CE_INT(g_pLocalPlayer->entity, netvar.nTickBase) * interfaces::gvars->interval_per_tick;
		curtime_old = interfaces::gvars->curtime;
		interfaces::gvars->curtime = servertime;
		time_replaced = true;
	}
	if (g_Settings.bInvalid) {
		gEntityCache.Invalidate();
	}
	PROF_BEGIN();
	SAFE_CALL(gEntityCache.Update());
	PROF_END("Entity Cache updating");
	SAFE_CALL(g_pPlayerResource->Update());
	SAFE_CALL(g_pLocalPlayer->Update());
	if (CE_GOOD(g_pLocalPlayer->entity)) {
			g_pLocalPlayer->v_OrigViewangles = cmd->viewangles;
		PROF_BEGIN();
		SAFE_CALL(CREATE_MOVE(Bunnyhop));
		//RunEnginePrediction(g_pLocalPlayer->entity, cmd);
		SAFE_CALL(CREATE_MOVE(ESP));
		SAFE_CALL(CREATE_MOVE(Aimbot));
		SAFE_CALL(CREATE_MOVE(Airstuck));
		SAFE_CALL(CREATE_MOVE(AntiAim));
		SAFE_CALL(CREATE_MOVE(AntiDisguise));
		SAFE_CALL(CREATE_MOVE(AutoHeal));
		SAFE_CALL(CREATE_MOVE(AutoSticky));
		SAFE_CALL(CREATE_MOVE(AutoReflect));
		SAFE_CALL(CREATE_MOVE(AutoStrafe));
		SAFE_CALL(CREATE_MOVE(FollowBot));
		SAFE_CALL(CREATE_MOVE(Misc));
		SAFE_CALL(CREATE_MOVE(Triggerbot));
		SAFE_CALL(CREATE_MOVE(HuntsmanCompensation));
		PROF_END("Hacks processing");
		if (time_replaced) interfaces::gvars->curtime = curtime_old;
	}
	/*for (IHack* i_hack : hack::hacks) {
		if (!i_hack->CreateMove(thisptr, inputSample, cmd)) {
			ret = false;
		}
	}*/
	g_Settings.bInvalid = false;
	if (CE_GOOD(g_pLocalPlayer->entity)) {
		if (g_pLocalPlayer->bUseSilentAngles) {
			Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
			float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
			Vector ang;
			VectorAngles(vsilent, ang);
			float yaw = DEG2RAD(ang.y - g_pLocalPlayer->v_OrigViewangles.y + cmd->viewangles.y);
			cmd->forwardmove = cos(yaw) * speed;
			cmd->sidemove = sin(yaw) * speed;
			ret = false;
		}
	}
	if (cmd)
		last_angles = cmd->viewangles;

	PROF_END("CreateMove");

	return ret;

	SEGV_END;
	return true;
}

void hack::Hk_FrameStageNotify(void* thisptr, int stage) {
	SEGV_BEGIN;
	//logging::Info("FrameStageNotify %i", stage);
	// Ambassador to festive ambassador changer. simple.
	if (g_Settings.bHackEnabled->GetBool()) {
		if (CE_GOOD(g_pLocalPlayer->weapon)) {
			int defidx = CE_INT(g_pLocalPlayer->weapon, netvar.iItemDefinitionIndex);
			if (defidx == 61) {
				CE_INT(g_pLocalPlayer->weapon, netvar.iItemDefinitionIndex) = 1006;
			}
		}
		if (g_Settings.bThirdperson->GetBool() && g_pLocalPlayer->entity) {
			CE_INT(g_pLocalPlayer->entity, netvar.nForceTauntCam) = 1;
		}
		if (stage == 5 && g_Settings.bShowAntiAim->GetBool() && interfaces::iinput->CAM_IsThirdPerson()) {
			if (g_pLocalPlayer->entity) {
				CE_FLOAT(g_pLocalPlayer->entity, netvar.deadflag + 4) = last_angles.x;
				CE_FLOAT(g_pLocalPlayer->entity, netvar.deadflag + 8) = last_angles.y;
			}
		}
	}
	((FrameStageNotify_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify))(thisptr, stage);
	if (g_Settings.bHackEnabled->GetBool()) {
		if (stage == 5 && g_Settings.bNoFlinch->GetBool()) {
			static Vector oldPunchAngles = Vector();
			Vector punchAngles = CE_VECTOR(g_pLocalPlayer->entity, netvar.vecPunchAngle);
			QAngle viewAngles;
			interfaces::engineClient->GetViewAngles(viewAngles);
			viewAngles -= VectorToQAngle(punchAngles - oldPunchAngles);
			oldPunchAngles = punchAngles;
			interfaces::engineClient->SetViewAngles(viewAngles);
		}

		if (g_Settings.bNoZoom->GetBool()) {
			if (g_pLocalPlayer->entity) {
				//g_pLocalPlayer->bWasZoomed = NET_INT(g_pLocalPlayer->entity, netvar.iCond) & cond::zoomed;
				CE_INT(g_pLocalPlayer->entity, netvar.iCond) = CE_INT(g_pLocalPlayer->entity, netvar.iCond) &~ cond::zoomed;
			}
		}
	}
	SEGV_END;
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

ConCommand* hack::c_Cat = 0;

void hack::CC_Cat(const CCommand& args) {
	interfaces::cvar->ConsoleColorPrintf(colors::blu, "CatHook");
	interfaces::cvar->ConsoleColorPrintf(colors::white, " by ");
	interfaces::cvar->ConsoleColorPrintf(colors::blu, "d4rkc4t\n");
	interfaces::cvar->ConsoleColorPrintf(colors::white, "Build: " __DATE__ " " __TIME__"\n");
	interfaces::cvar->ConsoleColorPrintf(colors::red, "[DEVELOPER BUILD]\n");
}

void hack::Initialize() {
	logging::Initialize();
	prctl(PR_SET_DUMPABLE,0,42,42,42);
	logging::Info("Build: " __DATE__ " " __TIME__);
	logging::Info("Loading shared objects...");
	sharedobj::LoadAllSharedObjects();
	logging::Info("Creating interfaces...");
	interfaces::CreateInterfaces();
	logging::Info("Interfaces created!");
	logging::Info("APPID: %i", interfaces::engineClient->GetAppID());
	logging::Info("Dumping NetVars...");
	CDumper dumper;
	dumper.SaveDump();
	logging::Info("Initializing surface...");
	draw::Initialize();
	logging::Info("Colorizing...");
	colors::Init();
	logging::Info("Boosting luck...");
	uintptr_t mmmf = (gSignatures.GetClientSignature("C7 44 24 04 09 00 00 00 BB ? ? ? ? C7 04 24 00 00 00 00 E8 ? ? ? ? BA ? ? ? ? 85 C0 B8 ? ? ? ? 0F 44 DA") + 37);
	if (mmmf) {
		unsigned char patch1[] = { 0x89, 0xD3, 0x90 };
		unsigned char patch2[] = { 0x89, 0xC2, 0x90 };
		Patch((void*)mmmf, (void*)patch1, 3);
		Patch((void*)(mmmf + 8), (void*)patch2, 3);
	} else logging::Info("You are already filled with luck.");
	logging::Info("Adding hacks...");

	BeginConVars();
	hack::c_Cat = CreateConCommand(CON_NAME, &hack::CC_Cat, "Info");
	hack::InitHacks();
	logging::Info("Init global settings");
	g_Settings.Init();
#if ENTITY_CACHE_PROFILER == true
	if (!g_vEntityCacheProfiling) {
		g_vEntityCacheProfiling = CREATE_CV(CV_SWITCH, "entity_cache_profiling", "0", "Entity cache profiling");
	}
#endif
	g_pGUI = new GUI();
	g_pGUI->Setup();
	EndConVars();
	logging::Info("Initializing NetVar tree...");
	gNetvars.init();
	logging::Info("Initializing entity offsets...");
	InitNetVars();

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
	hooks::hkClient->HookMethod((void*)&Hk_IN_KeyEvent, hooks::offKeyEvent);
	hooks::hkClient->Apply();
	/*hooks::hkMatSurface = new hooks::VMTHook();
	hooks::hkMatSurface->Init((void*)interfaces::matsurface, 0);
	hooks::hkMatSurface->HookMethod((void*)hk_HandleInputEvent, hooks::offHandleInputEvent);
	hooks::hkMatSurface->Apply();
	logging::Info("MatSurface Hooked? %f", interfaces::matsurface->DrawGetAlphaMultiplier());*/
	logging::Info("Hooked!");
	InitStrings();
	logging::Info("Init done!");
}

void hack::Think() {
	usleep(250000);
}

void hack::Shutdown() {
	if (hack::shutdown) return;
	hack::shutdown = true;
	logging::Info("Shutting down...");
	logging::Shutdown();
	ConVar_Unregister();
	if (hooks::hkPanel) hooks::hkPanel->Kill();
	if (hooks::hkClientMode) hooks::hkClientMode->Kill();
	if (hooks::hkClient) hooks::hkClient->Kill();
	if (hooks::hkMatSurface) hooks::hkMatSurface->Kill();
	if (hooks::hkNetChannel) hooks::hkNetChannel->Kill();
	for (IHack* i_hack : hack::hacks) {
		delete i_hack;
	}
}
