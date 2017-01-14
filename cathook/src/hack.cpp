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

#include <steam/isteamuser.h>
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

#include "hooks/hookedmethods.h"

#include "sdk.h"
#include "vfunc.h"
#include "copypasted/CSignature.h"
#include "copypasted/Netvar.h"
#include "CDumper.h"
#include "ipc/ipcctl.h"

/*
 *  Credits to josh33901 aka F1ssi0N for butifel F1Public and Darkstorm 2015 Linux
 */

bool hack::shutdown = false;

ICvar* g_pCVar = 0;

void hack::InitHacks() {
	ADD_HACK(AutoStrafe);
	ADD_HACK(AntiAim);
	ADD_HACK(AntiDisguise);
	ADD_HACK(AutoReflect);
	//ADD_HACK(FollowBot);
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
#if _DEVELOPER
	interfaces::cvar->ConsoleColorPrintf(colors::red, "[DEVELOPER BUILD]\n");
#endif
	interfaces::cvar->ConsoleColorPrintf(colors::red, "Build for user " __DRM_STEAMID_S " (Early Access)\n");
#ifdef __DRM_NOTES
	interfaces::cvar->ConsoleColorPrintf(colors::red, "Build notes: " __DRM_NOTES "\n");
#endif
}

void hack::Initialize() {
	logging::Initialize();
	prctl(PR_SET_DUMPABLE,0,42,42,42);
	logging::Info("Build: " __DATE__ " " __TIME__);
	logging::Info("Loading shared objects...");
	sharedobj::LoadAllSharedObjects();
	g_pszTFPath = tf_path_from_maps();
	logging::Info("TF folder: %s", g_pszTFPath);
	logging::Info("Creating interfaces...");
	interfaces::CreateInterfaces();
	logging::Info("User: %llu", interfaces::user->GetSteamID().ConvertToUint64());
	DRM_ENFORCE;
	logging::Info("Interfaces created!");
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
	//hooks::hkPanel->HookMethod((void*)&hack::Hk_PaintTraverse, hooks::offPaintTraverse);
	hooks::hkPanel->HookMethod((void*)PaintTraverse_hook, hooks::offPaintTraverse);
	hooks::hkPanel->Apply();
	hooks::hkClientMode = new hooks::VMTHook();
	uintptr_t* clientMode = 0;
	while(!(clientMode = **(uintptr_t***)((uintptr_t)((*(void***)interfaces::baseClient)[10]) + 1))) {
		sleep(1);
	}
	hooks::hkClientMode->Init((void*)clientMode, 0);
	//hooks::hkClientMode->HookMethod((void*)&hack::Hk_CreateMove, hooks::offCreateMove);
	hooks::hkClientMode->HookMethod((void*)CreateMove_hook, hooks::offCreateMove);
	hooks::hkClientMode->HookMethod((void*)OverrideView_hook, hooks::offOverrideView);
	hooks::hkClientMode->Apply();
	hooks::hkClient = new hooks::VMTHook();
	hooks::hkClient->Init((void*)interfaces::baseClient, 0);
	hooks::hkClient->HookMethod((void*)FrameStageNotify_hook, hooks::offFrameStageNotify);
	hooks::hkClient->HookMethod((void*)DispatchUserMessage_hook, hooks::offFrameStageNotify + 1);
	hooks::hkClient->HookMethod((void*)IN_KeyEvent_hook, hooks::offKeyEvent);
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
	DELETE_HACK(AutoStrafe);
	DELETE_HACK(AntiAim);
	DELETE_HACK(AntiDisguise);
	DELETE_HACK(AutoReflect);
	DELETE_HACK(FollowBot);
	DELETE_HACK(Misc);
	DELETE_HACK(Aimbot);
	DELETE_HACK(Bunnyhop);
	DELETE_HACK(ESP);
	DELETE_HACK(Triggerbot);
	DELETE_HACK(AutoSticky);
	DELETE_HACK(Airstuck);
	DELETE_HACK(AutoHeal);
	DELETE_HACK(HuntsmanCompensation);
	DELETE_HACK(SpyAlert);
}
