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
	logging::Info("Pushing list %s", id);
	if (m_nStackSize == LISTS_MAX - 1) return;
	for (int i = 0; i < m_nListCount; i++) {
		GUI_List* list = m_Lists[i];
		if (strcmp(list->m_pszListID, id) == 0) {
			logging::Info("Found!");
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

bool GUI::KeyEvent(ButtonCode_t key) {
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
	return false;
}

#define CREATE_LIST(id, name) \
	GUI_List* list_##id = new GUI_List(#id, name); \
	AddList(list_##id);

#define ADD_SUBLIST(list, sublist) \
	list_##list->AddElement(new GUIListElement_SubList(list_##sublist));

#define ADD_SWITCH(list, var) \
	list_##list->AddElement(new GUIListElement_Var(new CatVar(var, CatVar_t::CV_SWITCH)));

#define ADD_INT(list, var) \
	list_##list->AddElement(new GUIListElement_Var(new CatVar(var, CatVar_t::CV_INT)));

#define ADD_FLOAT(list, var) \
	list_##list->AddElement(new GUIListElement_Var(new CatVar(var, CatVar_t::CV_FLOAT)));


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

	list_main->Move(100, 100);

	ADD_SUBLIST(main, aimbot);
	ADD_SUBLIST(main, antiaim);
	ADD_SUBLIST(main, autoreflect);
	ADD_SUBLIST(main, autosticky);
	ADD_SUBLIST(main, esp);
	ADD_SUBLIST(main, autoheal);
	ADD_SUBLIST(main, bhop);

	ADD_SWITCH(aimbot, g_phAimbot->v_bEnabled);
	// TODO enums
	ADD_INT(aimbot, g_phAimbot->v_iAimKeyMode);
	ADD_INT(aimbot, g_phAimbot->v_iAimKey);
	ADD_INT(aimbot, g_phAimbot->v_iHitbox);
	ADD_SWITCH(aimbot, g_phAimbot->v_bAutoHitbox);
	ADD_SWITCH(aimbot, g_phAimbot->v_bPrediction);
	ADD_SWITCH(aimbot, g_phAimbot->v_bAutoShoot);
	ADD_SWITCH(aimbot, g_phAimbot->v_bSilent);
	ADD_SWITCH(aimbot, g_phAimbot->v_bZoomedOnly);
	ADD_SWITCH(aimbot, g_phAimbot->v_bRespectCloak);
	ADD_SWITCH(aimbot, g_phAimbot->v_bAimBuildings);
	ADD_FLOAT(aimbot, g_phAimbot->v_fFOV);
	ADD_SWITCH(aimbot, g_phAimbot->v_bMachinaPenetration);
	CREATE_LIST(aimbot_smooth, "Smooth")
	ADD_SUBLIST(aimbot, aimbot_smooth);
	ADD_SWITCH(aimbot_smooth, g_phAimbot->v_bSmooth);
	ADD_INT(aimbot, g_phAimbot->v_iSeenDelay);

	ADD_SWITCH(antiaim, g_phAntiAim->v_bEnabled);
	ADD_FLOAT(antiaim, g_phAntiAim->v_flPitch);
	ADD_FLOAT(antiaim, g_phAntiAim->v_flSpinSpeed);

	ADD_SWITCH(main, g_phAntiDisguise->v_bEnabled);
	ADD_SWITCH(autoheal, g_phAutoHeal->v_bEnabled);
	ADD_SWITCH(autoheal, g_phAutoHeal->v_bSilent);

	ADD_SWITCH(autoreflect, g_phAutoReflect->v_bEnabled);
	ADD_SWITCH(autoreflect, g_phAutoReflect->v_bDisableWhenAttacking);
	ADD_SWITCH(autoreflect, g_phAutoReflect->v_bReflectStickies);
	ADD_INT(autoreflect, g_phAutoReflect->v_iReflectDistance);

	ADD_SWITCH(autosticky, g_phAutoSticky->v_bEnabled);
	// TODO step increment

	ADD_SWITCH(esp, g_phESP->v_bEnabled);
	ADD_SWITCH(esp, g_phESP->v_bBox);
	ADD_SWITCH(esp, g_phESP->v_bEntityESP);
	CREATE_LIST(esp_item, "Item ESP");

	ADD_SUBLIST(esp, esp_item);
	ADD_SWITCH(esp_item, g_phESP->v_bItemESP);
	ADD_SWITCH(esp_item, g_phESP->v_bShowAmmoPacks);
	ADD_SWITCH(esp_item, g_phESP->v_bShowHealthPacks);
	ADD_SWITCH(esp_item, g_phESP->v_bShowDroppedWeapons);
	ADD_SWITCH(esp_item, g_phESP->v_bShowPowerups);
	ADD_SWITCH(esp, g_phESP->v_bLegit);
	ADD_INT(esp, g_phESP->v_iLegitSeenTicks);
	ADD_SWITCH(esp, g_phESP->v_bVisCheck);
	ADD_SWITCH(esp, g_phESP->v_bTeammatePowerup);
	ADD_SWITCH(esp, g_phESP->v_bTeammates);

	ADD_SWITCH(bhop, g_phBunnyhop->v_bEnabled);
	ADD_SWITCH(bhop, g_phBunnyhop->v_bAutoJump);

	ADD_SWITCH(trigger, g_phTriggerbot->v_bBodyshot);
	ADD_SWITCH(trigger, g_phTriggerbot->v_bBuildings);
	ADD_SWITCH(trigger, g_phTriggerbot->v_bEnabled);
	ADD_SWITCH(trigger, g_phTriggerbot->v_bFinishingHit);
	ADD_SWITCH(trigger, g_phTriggerbot->v_bIgnoreCloak);
	ADD_SWITCH(trigger, g_phTriggerbot->v_bZoomedOnly);

	PushList("main");
}

GUI* g_pGUI = 0;
