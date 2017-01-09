/*
 * PaintTraverse.cpp
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#include "PaintTraverse.h"
#include "../common.h"
#include "../segvcatch/segvcatch.h"

void PaintTraverse_hook(void* thisptr, unsigned int panel, bool b1, bool b2) {
	if (!segvcatch::handler_fpe || !segvcatch::handler_segv) {
		segvcatch::init_segv();
		segvcatch::init_fpe();
		logging::Info("segvcatch init!");
	}

	SEGV_BEGIN;
	SAFE_CALL(((PaintTraverse_t*)hooks::hkPanel->GetMethod(hooks::offPaintTraverse))(thisptr, panel, b1, b2));
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
		const char* name = interfaces::panel->GetName(panel);
		if (strlen(name) > 4) {
			if (name[0] == 'M' && name[3] == 'S') {
				draw::panel_top = panel;
				logging::Info("Got top panel: %i", panel);
			}
		}
	}
	if (!interfaces::engineClient->IsInGame()) {
		g_Settings.bInvalid = true;
	}
	if (g_Settings.bInvalid) return;
	if (CE_BAD(g_pLocalPlayer->entity)) return;
	if (draw::panel_top == panel) {
		ResetStrings();
		if (g_Settings.bShowLogo->GetBool()) {
			AddSideString(colors::green, colors::black, "cathook by d4rkc4t");
			AddSideString(colors::red, colors::black, "DEVELOPER BUILD");
		}
		for (IHack* i_hack : hack::hacks) {
			//PROF_BEGIN();
			SEGV_BEGIN
				i_hack->PaintTraverse(thisptr, panel, b1, b2);
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

