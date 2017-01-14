/*
 * gui.cpp
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#include "gui.h"
#include "controls.h"
#include "../common.h"
#include "../sdk.h"
#include "../hacks/hacklist.h"

GUI::GUI() {
	m_bActive = false;
	m_nListCount = 0;
	m_nStackSize = 0;
	m_ListStack = new GUI_List*[LISTS_MAX]();
	m_Lists = new GUI_List*[LISTS_MAX]();
}

void GUI::AddList(GUI_List* list) {
	m_Lists[m_nListCount] = list;
	m_nListCount++;
}

void GUI::PushList(const char* id) {
	if (m_nStackSize == LISTS_MAX - 1) return;
	for (int i = 0; i < m_nListCount; i++) {
		GUI_List* list = m_Lists[i];
		if (strcmp(list->m_pszListID, id) == 0) {
			m_ListStack[m_nStackSize] = list;
			m_nStackSize++;
			return;
		}
	}
}

void GUI::PopList() {
	if (m_nStackSize == 1) return;
	m_nStackSize--;
}

void GUI::Draw() {
	if (!m_bActive) return;
	for (int i = 0; i < m_nStackSize; i++) {
		GUI_List* list = m_ListStack[i];
		if (list)
			list->Draw();
	}
}

void GUI::UpdateKeys() {
	for (int i = 0; i < ButtonCode_t::KEY_COUNT; i++) {
		bool down = interfaces::input->IsButtonDown((ButtonCode_t)(KEY_FIRST + i));
		bool changed = m_bPressedState[i] != down;
		if (changed && down) m_iPressedFrame[i] = interfaces::gvars->framecount;
		m_bPressedState[i] = down;
		if (m_bKeysInit) {
			if (changed) {
				KeyEvent((ButtonCode_t)i);
			}
		}
	}
	if (!m_bKeysInit) m_bKeysInit = 1;
}

bool GUI::KeyEvent(ButtonCode_t key) {
	if (m_bPressedState[key]) {
		if (key == KEY_INSERT)
			m_bActive = !m_bActive;
		if (!m_bActive) return false;
		if (key == KEY_BACKSPACE) {
			PopList();
			return false;
		}

		if (m_nStackSize) {
			if (m_ListStack[m_nStackSize - 1])
				m_ListStack[m_nStackSize - 1]->KeyEvent(key);
		}
	}
	return false;
}

#define CREATE_LIST(id, name) \
	GUI_List* list_##id = new GUI_List(#id, name); \
	AddList(list_##id);

#define ADD_SUBLIST(list, sublist) \
	list_##list->AddElement(new GUIListElement_SubList(list_##sublist));

#define ADD_VAR(list, var) \
	list_##list->AddElement(new GUIListElement_Var(var));


void GUI::Setup() {
	CREATE_LIST(main, "MAIN");
	CREATE_LIST(aimbot, "Aimbot");
	CREATE_LIST(antiaim, "Anti-Aim");
	CREATE_LIST(autoheal, "Auto Heal");
	CREATE_LIST(autoreflect, "Auto Reflect");
	CREATE_LIST(autosticky, "Auto Sticky");
	CREATE_LIST(esp, "ESP");
	CREATE_LIST(bhop, "Bunnyhop");
	CREATE_LIST(trigger, "Triggerbot");
	CREATE_LIST(spy, "Spy Alert");

	list_main->Move(100, 100);

	ADD_SUBLIST(main, aimbot);
	ADD_SUBLIST(main, antiaim);
	ADD_SUBLIST(main, autoreflect);
	ADD_SUBLIST(main, autosticky);
	ADD_SUBLIST(main, esp);
	ADD_SUBLIST(main, autoheal);
	ADD_SUBLIST(main, bhop);
	ADD_SUBLIST(main, trigger);
	ADD_SUBLIST(main, spy);

	ADD_VAR(aimbot, g_phAimbot->v_bEnabled);
	// TODO enums
	ADD_VAR(aimbot, g_phAimbot->v_eAimKeyMode);
	ADD_VAR(aimbot, g_phAimbot->v_eAimKey);
	ADD_VAR(aimbot, g_phAimbot->v_ePriorityMode);
	ADD_VAR(aimbot, g_phAimbot->v_eHitbox);
	ADD_VAR(aimbot, g_phAimbot->v_bAutoHitbox);
	ADD_VAR(aimbot, g_phAimbot->v_bPrediction);
	ADD_VAR(aimbot, g_phAimbot->v_bAutoShoot);
	ADD_VAR(aimbot, g_phAimbot->v_bSilent);
	ADD_VAR(aimbot, g_phAimbot->v_bZoomedOnly);
	ADD_VAR(aimbot, g_phAimbot->v_bRespectCloak);
	ADD_VAR(aimbot, g_phAimbot->v_bAimBuildings);
	ADD_VAR(aimbot, g_phAimbot->v_fFOV);
	ADD_VAR(aimbot, g_phAimbot->v_bMachinaPenetration);
	ADD_VAR(aimbot, g_phAimbot->v_fAutoShootHuntsmanCharge);
	ADD_VAR(aimbot, g_phAimbot->v_iAutoShootCharge);
	ADD_VAR(aimbot, g_phAimbot->v_iMaxRange);
	ADD_VAR(aimbot, g_phAimbot->v_bCharge);
	ADD_VAR(aimbot, g_phAimbot->v_bEnabledAttacking);
	ADD_VAR(aimbot, g_phAimbot->v_bTriggerMode);
	ADD_VAR(aimbot, g_phAimbot->v_bProjectileAimbot);
	ADD_VAR(aimbot, g_phAimbot->v_fOverrideProjSpeed);
	/*CREATE_LIST(aimbot_smooth, "Smooth")
	ADD_SUBLIST(aimbot, aimbot_smooth);
	ADD_VAR(aimbot_smooth, g_phAimbot->v_bSmooth);
	// TODO inverse step multipilers
	ADD_VAR(aimbot_smooth, g_phAimbot->v_fSmoothAutoshootTreshold);
	ADD_VAR(aimbot_smooth, g_phAimbot->v_fSmoothRandomness);
	ADD_VAR(aimbot_smooth, g_phAimbot->v_fSmoothValue);*/
	ADD_VAR(aimbot, g_phAimbot->v_iSeenDelay);

	ADD_VAR(antiaim, g_phAntiAim->v_bEnabled);
	ADD_VAR(antiaim, g_phAntiAim->v_PitchMode);
	ADD_VAR(antiaim, g_phAntiAim->v_flPitch);
	ADD_VAR(antiaim, g_phAntiAim->v_YawMode);
	ADD_VAR(antiaim, g_phAntiAim->v_flYaw);
	ADD_VAR(antiaim, g_phAntiAim->v_flSpinSpeed);

	ADD_VAR(autoheal, g_phAutoHeal->v_bEnabled);
	ADD_VAR(autoheal, g_phAutoHeal->v_bSilent);

	ADD_VAR(autoreflect, g_phAutoReflect->v_bEnabled);
	ADD_VAR(autoreflect, g_phAutoReflect->v_bDisableWhenAttacking);
	ADD_VAR(autoreflect, g_phAutoReflect->v_bReflectStickies);
	ADD_VAR(autoreflect, g_phAutoReflect->v_iReflectDistance);

	ADD_VAR(autosticky, g_phAutoSticky->v_bEnabled);
	ADD_VAR(autosticky, g_phAutoSticky->v_bBuildings);
	ADD_VAR(autosticky, g_phAutoSticky->v_bScottish);
	ADD_VAR(autosticky, g_phAutoSticky->v_flDetonateDistance);

	ADD_VAR(esp, g_phESP->v_bEnabled);

	CREATE_LIST(esp_item, "Item ESP");
		ADD_VAR(esp_item, g_phESP->v_bItemESP);
		ADD_VAR(esp_item, g_phESP->v_bShowAmmoPacks);
		ADD_VAR(esp_item, g_phESP->v_bShowHealthPacks);
		ADD_VAR(esp_item, g_phESP->v_bShowDroppedWeapons);
		ADD_VAR(esp_item, g_phESP->v_bShowPowerups);
	ADD_SUBLIST(esp, esp_item);
	CREATE_LIST(esp_proj, "Projectile ESP");
		ADD_VAR(esp_proj, g_phESP->v_bProjectileESP);
		ADD_VAR(esp_proj, g_phESP->v_bOnlyEnemyProjectiles);
		ADD_VAR(esp_proj, g_phESP->v_iShowRockets);
		ADD_VAR(esp_proj, g_phESP->v_iShowPipes);
		ADD_VAR(esp_proj, g_phESP->v_iShowStickies);
		ADD_VAR(esp_proj, g_phESP->v_iShowArrows);
	ADD_SUBLIST(esp, esp_proj);

	ADD_VAR(esp, g_phESP->v_bBox);
	ADD_VAR(esp, g_phESP->v_bEntityESP);
	ADD_VAR(esp, g_phESP->v_bLegit);
	ADD_VAR(esp, g_phESP->v_iLegitSeenTicks);
	ADD_VAR(esp, g_phESP->v_bVisCheck);
	ADD_VAR(esp, g_phESP->v_bTeammatePowerup);
	ADD_VAR(esp, g_phESP->v_bTeammates);
	ADD_VAR(esp, g_phESP->v_bSeeLocal);
	ADD_VAR(esp, g_phESP->v_bShowTank);
	ADD_VAR(esp, g_phESP->v_bShowDistance);
	ADD_VAR(esp, g_phESP->v_bShowFriendID);
	ADD_VAR(esp, g_phESP->v_bShowFriends);
	ADD_VAR(esp, g_phESP->v_bShowHealthNumbers);

	ADD_VAR(bhop, g_phBunnyhop->v_bEnabled);
	ADD_VAR(bhop, g_phBunnyhop->v_bAutoJump);
	ADD_VAR(bhop, g_phBunnyhop->v_iAutoJumpSpeed);

	ADD_VAR(trigger, g_phTriggerbot->v_bEnabled);
	ADD_VAR(trigger, g_phTriggerbot->v_iHitbox);
	ADD_VAR(trigger, g_phTriggerbot->v_iMinRange);
	ADD_VAR(trigger, g_phTriggerbot->v_bBodyshot);
	ADD_VAR(trigger, g_phTriggerbot->v_bBuildings);
	ADD_VAR(trigger, g_phTriggerbot->v_bFinishingHit);
	ADD_VAR(trigger, g_phTriggerbot->v_bIgnoreCloak);
	ADD_VAR(trigger, g_phTriggerbot->v_bZoomedOnly);

	ADD_VAR(spy, g_phSpyAlert->v_bEnabled);
	ADD_VAR(spy, g_phSpyAlert->v_flBackstabDistance);
	ADD_VAR(spy, g_phSpyAlert->v_flWarningDistance);

	ADD_VAR(main, g_phAntiDisguise->v_bEnabled);
	ADD_VAR(main, g_phAutoStrafe->v_bEnabled);

	ADD_VAR(main, g_Settings.flForceFOV);
	ADD_VAR(main, g_Settings.bIgnoreTaunting);
//	ADD_VAR(main, g_Settings.bNoZoom);
//	ADD_VAR(main, g_Settings.bNoFlinch);
	ADD_VAR(main, g_Settings.bShowLogo);
	ADD_VAR(main, g_Settings.bShowAntiAim);
	ADD_VAR(main, g_Settings.bThirdperson);

	PushList("main");
}

GUI* g_pGUI = 0;
