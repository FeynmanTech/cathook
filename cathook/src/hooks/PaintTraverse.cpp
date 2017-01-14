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
#include "../segvcatch/segvcatch.h"

void PaintTraverse_hook(void* p, unsigned int vp, bool fr, bool ar) {
#if DEBUG_SEGV == true
	if (!segvcatch::handler_fpe || !segvcatch::handler_segv) {
		segvcatch::init_segv();
		segvcatch::init_fpe();
		logging::Info("segvcatch init!");
	}
#endif
	SEGV_BEGIN;
	SAFE_CALL(((PaintTraverse_t*)hooks::hkPanel->GetMethod(hooks::offPaintTraverse))(p, vp, fr, ar));
	if (!g_Settings.bHackEnabled->GetBool()) return;
	// Because of single-multi thread shit I'm gonna put this thing riiiight here.
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

	if (g_Settings.bNoVisuals->GetBool()) {
		return;
	}

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
	if (g_Settings.bInvalid) {
		return;
	}
	if (CE_BAD(g_pLocalPlayer->entity)) {
		return;
	}
	if (draw::panel_top == vp) {
		ResetStrings();
		if (g_Settings.bShowLogo->GetBool()) {
			AddSideString(colors::green, colors::black, "cathook by d4rkc4t");
#if _DEVELOPER
			AddSideString(colors::red, colors::black, "DEVELOPER BUILD");
#else
			AddSideString(colors::orange, colors::black, "Early Access: " __DRM_NAME);
#endif
			AddSideString(colors::green, colors::black, "version: " CATHOOK_VERSION_MAJOR "." CATHOOK_VERSION_MINOR "." CATHOOK_VERSION_PATCH);
		}

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

