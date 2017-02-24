/*
 * hack.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "hack.h"

#include "beforecheaders.h"
#include <vector>
#include <map>
#include <cstring>
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
#include "aftercheaders.h"

#include <steam/isteamuser.h>
// All Hacks
#include "hacks/hacklist.h"

#include "common.h"
#include "sharedobj.h"
#include "hooks.h"
#include "netmessage.h"
#include "targeting/ITargetSystem.h"
#include "profiler.h"
#include "gui/GUI.h"
//#include "gui/controls.h"
#include "cvwrapper.h"

#include "hooks/hookedmethods.h"

#include "sdk.h"
#include "vfunc.h"
#include "copypasted/CSignature.h"
#include "copypasted/Netvar.h"
#include "CDumper.h"
#include "ipc/ipcctl.h"

#include <KeyValues.h>

/*
 *  Credits to josh33901 aka F1ssi0N for butifel F1Public and Darkstorm 2015 Linux
 */

bool hack::shutdown = false;

void hack::InitHacks() {
	ADD_HACK(AutoStrafe);
	ADD_HACK(AntiAim);
	if (TF) ADD_HACK(AntiDisguise);
	if (TF) ADD_HACK(AutoReflect);
	//ADD_HACK(FollowBot);
	ADD_HACK(Misc);
	ADD_HACK(Aimbot);
	ADD_HACK(Bunnyhop);
	ADD_HACK(ESP);
	ADD_HACK(Triggerbot);
	if (TF) ADD_HACK(AutoSticky);
	ADD_HACK(Airstuck);
	if (TF) ADD_HACK(AutoHeal);
	if (TF) ADD_HACK(SpyAlert);
	if (TF2) ADD_HACK(Glow);
	ADD_HACK(KillSay);
	ADD_HACK(Spam);
	if (TF) ADD_HACK(AchievementHack);
	if (TF2) ADD_HACK(Noisemaker);
}

ConCommand* hack::c_Cat = 0;

void hack::CC_Cat(const CCommand& args) {
	interfaces::cvar->ConsoleColorPrintf(*reinterpret_cast<Color*>(&colors::blu), "cathook");
	interfaces::cvar->ConsoleColorPrintf(*reinterpret_cast<Color*>(&colors::white), " by ");
	interfaces::cvar->ConsoleColorPrintf(*reinterpret_cast<Color*>(&colors::blu), "d4rkc4t\n");
	interfaces::cvar->ConsoleColorPrintf(*reinterpret_cast<Color*>(&colors::white), "build: " CATHOOK_BUILD_NUMBER " \"" CATHOOK_BUILD_NAME "\"\n");
#if _DEVELOPER
	interfaces::cvar->ConsoleColorPrintf(*reinterpret_cast<Color*>(&colors::red), "[DEVELOPER BUILD]\n");
#else
	interfaces::cvar->ConsoleColorPrintf(*reinterpret_cast<Color*>(&colors::red), "Build for user " __DRM_NAME " (Early Access)\n");
#endif
#ifdef __DRM_NOTES
	interfaces::cvar->ConsoleColorPrintf(*reinterpret_cast<Color*>(&colors::red), "Build notes: " __DRM_NOTES "\n");
#endif
}

typedef bool(handlevent_t)(IMatSystemSurface* thisptr, const InputEvent_t& event);
bool test_handleevent(IMatSystemSurface* thisptr, const InputEvent_t& event) {

}

void hack::Initialize() {
	logging::Initialize();
	srand(time(0));
	prctl(PR_SET_DUMPABLE,0,42,42,42);
	sharedobj::LoadAllSharedObjects();
	g_pszTFPath = tf_path_from_maps();
	interfaces::CreateInterfaces();
	CDumper dumper;
	dumper.SaveDump();
	ClientClass* cc = interfaces::baseClient->GetAllClasses();
	FILE* cd = fopen("/tmp/cathook-classdump.txt", "w");
	while (cc) {
		fprintf(cd, "[%d] %s\n", cc->m_ClassID, cc->GetName());
		cc = cc->m_pNext;
	}
	fclose(cd);
	if (TF2) g_pClassID = new ClassIDTF2();
	else if (TF2C) g_pClassID = new ClassIDTF2C();
	else if (HL2DM) g_pClassID = new ClassIDHL2DM();
	g_pClassID->Init();

	draw::Initialize();
	colors::Init();
	if (TF2) {
		uintptr_t mmmf = (gSignatures.GetClientSignature("C7 44 24 04 09 00 00 00 BB ? ? ? ? C7 04 24 00 00 00 00 E8 ? ? ? ? BA ? ? ? ? 85 C0 B8 ? ? ? ? 0F 44 DA") + 37);
		if (mmmf) {
			unsigned char patch1[] = { 0x89, 0xD3, 0x90 };
			unsigned char patch2[] = { 0x89, 0xC2, 0x90 };
			Patch((void*)mmmf, (void*)patch1, 3);
			Patch((void*)(mmmf + 8), (void*)patch2, 3);
		}
		uintptr_t canInspectSig = (gSignatures.GetClientSignature("55 0F 57 C0 89 E5 83 EC 48 8B 45 08 F3 0F 11 04 24 F3 0F 11 45 E8 C7 44 24 10 01 00 00 00 C7 44 24 0C 00 00 00 00 89 44 24 08 C7 44 24 ? ? ? ? ? E8 ? ? ? ? F3 0F 10 45 E8 D9 5D E4 F3 0F 10 4D E4 C9 0F 2F C8 0F 95 C0 C3") + 72);
		if (canInspectSig) {
			unsigned char patch[] = { 0xB0, 0x01, 0x90 };
			Patch((void*)canInspectSig, (void*)patch, 3);
		}
	}
	BeginConVars();
	hack::c_Cat = CreateConCommand(CON_NAME, &hack::CC_Cat, "Info");
	hack::InitHacks();
	g_Settings.Init();
#if ENTITY_CACHE_PROFILER == true
	if (!g_vEntityCacheProfiling) {
		g_vEntityCacheProfiling = CREATE_CV(CV_SWITCH, "entity_cache_profiling", "0", "Entity cache profiling");
	}
#endif
	g_pGUI = new CatGUI();
	g_pGUI->Setup();
	EndConVars();
	gNetvars.init();
	InitNetVars();

	g_pLocalPlayer = new LocalPlayer();
	g_pPlayerResource = new TFPlayerResource();

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
	//hooks::hkMatSurface = new hooks::VMTHook();
	//hooks::hkMatSurface->Init((void*)interfaces::matsurface, 0);
	//hooks::hkMatSurface->HookMethod((void*)test_handleevent, 1);
	hooks::hkClientMode->Init((void*)clientMode, 0);
	//hooks::hkClientMode->HookMethod((void*)&hack::Hk_CreateMove, hooks::offCreateMove);
	hooks::hkClientMode->HookMethod((void*)CreateMove_hook, hooks::offCreateMove);
	hooks::hkClientMode->HookMethod((void*)OverrideView_hook, hooks::offOverrideView);
	hooks::hkClientMode->HookMethod((void*)LevelInit_hook, hooks::offLevelInit);
	hooks::hkClientMode->HookMethod((void*)LevelShutdown_hook, hooks::offLevelShutdown);
	hooks::hkClientMode->Apply();
	hooks::hkStudioRender = new hooks::VMTHook();
	hooks::hkStudioRender->Init((void*)interfaces::render, 0);
	hooks::hkStudioRender->HookMethod((void*)BeginFrame_hook, hooks::offBeginFrame);
	hooks::hkStudioRender->Apply();

	hooks::hkClient = new hooks::VMTHook();
	hooks::hkClient->Init((void*)interfaces::baseClient, 0);
	hooks::hkClient->HookMethod((void*)FrameStageNotify_hook, hooks::offFrameStageNotify);
	hooks::hkClient->HookMethod((void*)DispatchUserMessage_hook, hooks::offFrameStageNotify + 1);
	hooks::hkClient->HookMethod((void*)IN_KeyEvent_hook, hooks::offKeyEvent);
	hooks::hkClient->Apply();
	if (TF2) g_GlowObjectManager = *reinterpret_cast<CGlowObjectManager**>(gSignatures.GetClientSignature("C1 E0 05 03 05") + 5);
	InitStrings();
	g_pChatStack = new ChatStack();
}

void hack::Think() {
	usleep(250000);
}

void hack::Shutdown() {
	if (hack::shutdown) return;
	hack::shutdown = true;
	logging::Shutdown();
	if (hooks::hkPanel) hooks::hkPanel->Kill();
	if (hooks::hkClientMode) hooks::hkClientMode->Kill();
	if (hooks::hkClient) hooks::hkClient->Kill();
	if (hooks::hkMatSurface) hooks::hkMatSurface->Kill();
	if (hooks::hkNetChannel) hooks::hkNetChannel->Kill();
	if (hooks::hkStudioRender) hooks::hkStudioRender->Kill();
	ConVar_Unregister();
	DELETE_HACK(AutoStrafe);
	DELETE_HACK(AntiAim);
	if (TF) DELETE_HACK(AntiDisguise);
	if (TF) DELETE_HACK(AutoReflect);
	//DELETE_HACK(FollowBot);
	DELETE_HACK(Misc);
	DELETE_HACK(Aimbot);
	DELETE_HACK(Bunnyhop);
	DELETE_HACK(ESP);
	DELETE_HACK(Triggerbot);
	if (TF) DELETE_HACK(AutoSticky);
	DELETE_HACK(Airstuck);
	if (TF) DELETE_HACK(AutoHeal);
	DELETE_HACK(SpyAlert);
	if (TF) DELETE_HACK(Glow);
	DELETE_HACK(KillSay);
	if (TF) DELETE_HACK(AchievementHack);
	if (TF2) DELETE_HACK(Noisemaker);
	DELETE_HACK(Spam);
}
