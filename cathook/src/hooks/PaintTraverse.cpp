/*
 * PaintTraverse.cpp
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#include "PaintTraverse.h"
#include "../common.h"
#include "../hack.h"
#include "hookedmethods.h"
#include "../gui/GUI.h"
#include "../segvcatch/segvcatch.h"

void PaintTraverse_hook(void* p, unsigned int vp, bool fr, bool ar) {
#if DEBUG_SEGV == true
	if (!segvcatch::handler_fpe || !segvcatch::handler_segv) {
		segvcatch::init_segv();
		segvcatch::init_fpe();
	}
#endif
	SEGV_BEGIN;
	static unsigned long panel_focus = 0;
	static unsigned long panel_scope = 0;
	static unsigned long panel_top = 0;
	static bool draw_flag = false;
	bool call_default = true;
	if (g_Settings.bHackEnabled->GetBool() && panel_scope && g_Settings.bNoZoom->GetBool() && vp == panel_scope) call_default = false;
	if (g_Settings.bHackEnabled->GetBool()) {
		bool vis = g_pGUI->v_bGUIVisible->GetBool();
		interfaces::surface->SetCursorAlwaysVisible(vis);
	}


	if (call_default) SAFE_CALL(((PaintTraverse_t*)hooks::hkPanel->GetMethod(hooks::offPaintTraverse))(p, vp, fr, ar));
	if (vp == panel_top) draw_flag = true;
	if (!g_Settings.bHackEnabled->GetBool()) return;
	// Because of single-multi thread shit I'm gonna put this thing riiiight here.
	static bool autoexec_done = false;
	if (!autoexec_done) {
		interfaces::engineClient->ExecuteClientCmd("exec cat_autoexec");
		interfaces::engineClient->ExecuteClientCmd("cat_killsay_reload");
		interfaces::engineClient->ExecuteClientCmd("cat_spam_reload");
		autoexec_done = true;
	}
#if NO_IPC != true
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
#endif


	if (!draw::width || !draw::height) {
		interfaces::engineClient->GetScreenSize(draw::width, draw::height);
	}
	if (!panel_top) {
		const char* name = interfaces::panel->GetName(vp);
		if (strlen(name) > 4) {
			if (name[0] == 'M' && name[3] == 'S') {
				panel_top = vp;
			}
			if (name[0] == 'F' && name[5] == 'O') {
				panel_focus = vp;
			}
		}
	}
	if (!panel_scope) {
		if (!strcmp(interfaces::panel->GetName(vp), "HudScope")) {
			panel_scope = vp;
		}
	}
	if (!interfaces::engineClient->IsInGame()) {
		g_Settings.bInvalid = true;
	}

	if (g_Settings.bNoVisuals->GetBool()) {
		return;
	}
	if (g_Settings.bCleanScreenshots->GetBool() && interfaces::engineClient->IsTakingScreenshot()) return;

	ResetStrings();

	if (vp != panel_focus) return;
	if (!draw_flag) return;
	draw_flag = false;

#if GUI_ENABLED == true
		/*g_pGUI->UpdateKeys();
		g_pGUI->UpdateMouse();
		g_pGUI->Draw();*/
		g_pGUI->Update();
#endif


	if (g_Settings.bShowLogo->GetBool()) {
		AddSideString(colors::green, "cathook by d4rkc4t");
#if _DEVELOPER
		AddSideString(colors::red, "[developer build]");
#else
		AddSideString(colors::green, "built for " __DRM_NAME);
#endif
		AddSideString(colors::green, "alpha build " CATHOOK_BUILD_NUMBER " \"" CATHOOK_BUILD_NAME "\"");
	}
	if (CE_GOOD(g_pLocalPlayer->entity) && !g_Settings.bInvalid) {
		//SAFE_CALL(PAINT_TRAVERSE(AutoStrafe));
		//SAFE_CALL(PAINT_TRAVERSE(AntiAim));
		SAFE_CALL(PAINT_TRAVERSE(AntiDisguise));
		//SAFE_CALL(PAINT_TRAVERSE(AutoReflect));
		//SAFE_CALL(PAINT_TRAVERSE(FollowBot));
		SAFE_CALL(PAINT_TRAVERSE(Misc));
		//SAFE_CALL(PAINT_TRAVERSE(Aimbot));
		//SAFE_CALL(PAINT_TRAVERSE(Bunnyhop));
		SAFE_CALL(PAINT_TRAVERSE(ESP));
		//SAFE_CALL(PAINT_TRAVERSE(Triggerbot));
		//SAFE_CALL(PAINT_TRAVERSE(AutoSticky));
		//SAFE_CALL(PAINT_TRAVERSE(Airstuck));
		//SAFE_CALL(PAINT_TRAVERSE(AutoHeal));
		//SAFE_CALL(PAINT_TRAVERSE(HuntsmanCompensation));
		SAFE_CALL(PAINT_TRAVERSE(SpyAlert));
		Vector screen;
		for (int i = 0; i < HIGHEST_ENTITY; i++) {
			CachedEntity* ce = gEntityCache.GetEntity(i);
			if (CE_BAD(ce)) continue;
			if (ce->m_ESPOrigin.IsZero(1.0f))
				if (!draw::EntityCenterToScreen(ce, screen)) continue;
			for (int j = 0; j < ce->m_nESPStrings; j++) {
				ESPStringCompound str = ce->GetESPString(j);
				int color = str.m_bColored ? str.m_nColor : ce->m_ESPColorFG;
				//logging::Info("drawing [idx=%i][ns=%i] %s", i, ce->m_nESPStrings, str.m_String);
				if (!ce->m_ESPOrigin.IsZero(1.0)) {
					draw::String(fonts::ESP, ce->m_ESPOrigin.x, ce->m_ESPOrigin.y, color, 2, str.m_String);
					ce->m_ESPOrigin.y += 12;
				} else {
					draw::String(fonts::ESP, screen.x, screen.y, color, 2, str.m_String);
					screen.y += 11;
				}
			}
		}
		if (g_Settings.bNoZoom->GetBool()) {
			if (CE_GOOD(g_pLocalPlayer->entity)) {
				RemoveCondition(g_pLocalPlayer->entity, condition::TFCond_Zoomed);
			}
		}
	}

	DrawStrings();
	SEGV_END;
}

