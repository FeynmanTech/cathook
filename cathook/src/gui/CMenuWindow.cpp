/*
 * CMenuWindow.cpp
 *
 *  Created on: Feb 3, 2017
 *      Author: nullifiedcat
 */

#include "CMenuWindow.h"
#include "CMenuList.h"
#include "CTextLabel.h"
#include "CMenuContainer.h"
#include "CCVarContainer.h"
#include "CBaseButton.h"
#include "../common.h"
#include "../sdk.h"
#include "CCTitleBar.h"

CMenuWindow::CMenuWindow(std::string name, IWidget* parent) : CBaseWindow(name, parent) {
	m_pList = new CMenuList("list", this);
	AddChild(m_pList);
	m_pActiveTab = 0;
	AddChild(m_pTitle = new CTitleBar(this, "cathook menu"));
}

#define ADDCVAR(x) tab->AddChild(new CCVarContainer(tab, x))
#define ADDLABEL(x) tab->AddChild(new CTextLabel("label", tab, x, true))

void CMenuWindow::AddElements() {
	AddTab("aimbot", "Aimbot");
	CMenuContainer* tab = GetTab("aimbot");
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bEnabled));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bAimAtTeammates));
	if (TF) tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bAimBuildings));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bAutoHitbox));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bAutoShoot));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bActiveOnlyWhenCanShoot));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bEnabledAttacking));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bInterpolation));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bProjPredFOV));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bProjPredVisibility));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bProjectileAimbot));
	if (TF) tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bRespectCloak));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bSilent));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_bZoomedOnly));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_eAimKeyMode));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_eHitbox));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_ePriorityMode));
	if (TF) tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_fAutoShootHuntsmanCharge));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_fFOV));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_fOverrideProjGravity));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_fOverrideProjSpeed));
	//tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_iMaxAutoshootRange));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_iMaxRange));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_iSeenDelay));
	tab->AddChild(new CCVarContainer(tab, g_phAimbot->v_kAimKey));
	AddTab("esp", "ESP");
	tab = GetTab("esp");
	ADDCVAR(g_phESP->v_bEnabled);
	if (TF) ADDCVAR(g_phESP->v_bShowTank);
	if (TF) ADDCVAR(g_phESP->v_bBuildingESP);
	ADDLABEL("Player ESP");
	ADDCVAR(g_phESP->v_bBox);
	ADDCVAR(g_phESP->v_bTeammates);
	ADDCVAR(g_phESP->v_bLegit);
	ADDCVAR(g_phESP->v_iLegitSeenTicks);
	ADDCVAR(g_phESP->v_bShowHealthNumbers);
	ADDCVAR(g_phESP->v_bShowName);
	if (TF) ADDCVAR(g_phESP->v_bShowClass);
	if (TF) ADDCVAR(g_phESP->v_bShowConditions);
	ADDCVAR(g_phESP->v_bSeeLocal);
	if (TF) ADDCVAR(g_phESP->v_bTeammatePowerup);
	ADDCVAR(g_phESP->v_bShowFriends);
	ADDCVAR(g_phESP->v_bShowFriendID);
	ADDLABEL("Item ESP");
	ADDCVAR(g_phESP->v_bItemESP);
	ADDCVAR(g_phESP->v_bShowMoney);
	ADDCVAR(g_phESP->v_bShowHealthPacks);
	if (TF2C) ADDCVAR(g_phESP->v_bShowWeaponSpawners);
	if (TF2C) ADDCVAR(g_phESP->v_bShowAdrenaline);
	if (TF) ADDCVAR(g_phESP->v_bShowAmmoPacks);
	ADDCVAR(g_phESP->v_bShowDroppedWeapons);
	if (TF) ADDCVAR(g_phESP->v_bShowPowerups);
	if (TF) ADDCVAR(g_phESP->v_bShowRedMoney);
	if (TF) {
		ADDLABEL("Projectile ESP");
		ADDCVAR(g_phESP->v_bProjectileESP);
		ADDCVAR(g_phESP->v_bOnlyEnemyProjectiles);
		ADDCVAR(g_phESP->v_iShowArrows);
		ADDCVAR(g_phESP->v_iShowPipes);
		ADDCVAR(g_phESP->v_iShowRockets);
		ADDCVAR(g_phESP->v_iShowStickies);
	}
	ADDLABEL("");
	ADDCVAR(g_phESP->v_bEntityESP);
	ADDCVAR(g_phESP->v_bShowEntityID);
	AddTab("triggerbot", "Triggerbot");
	tab = GetTab("triggerbot");
	ADDCVAR(g_phTriggerbot->v_bEnabled);
	if (TF) ADDCVAR(g_phTriggerbot->v_bAmbassadorCharge);
	ADDCVAR(g_phTriggerbot->v_bBodyshot);
	ADDCVAR(g_phTriggerbot->v_bBuildings);
	ADDCVAR(g_phTriggerbot->v_bFinishingHit);
	if (TF) ADDCVAR(g_phTriggerbot->v_bRespectCloak);
	if (TF) ADDCVAR(g_phTriggerbot->v_bIgnoreVaccinator);
	if (TF) ADDCVAR(g_phTriggerbot->v_bZoomedOnly);
	ADDCVAR(g_phTriggerbot->v_iHitbox);
	ADDCVAR(g_phTriggerbot->v_iMaxRange);
	if (TF) {
		ADDLABEL("AutoSticky");
		ADDCVAR(g_phAutoSticky->v_bEnabled);
		ADDCVAR(g_phAutoSticky->v_bBuildings);
		ADDCVAR(g_phAutoSticky->v_flDetonateDistance);
		ADDLABEL("AutoReflect");
		ADDCVAR(g_phAutoReflect->v_bEnabled);
		ADDCVAR(g_phAutoReflect->v_bDisableWhenAttacking);
		ADDCVAR(g_phAutoReflect->v_bReflectStickies);
		ADDCVAR(g_phAutoReflect->v_iReflectDistance);
	}
	AddTab("misc", "Misc");
	tab = GetTab("misc");
	//ADDLABEL("To lock/unlock achievements use cat_achievement_ commands!");
	/*tab->AddChild(new CBaseButton("ach_unlock", tab, "Unlock Achievements (DO NOT USE INGAME)", [this](CBaseButton*) {
		Achievements_Unlock();
	}));
	tab->AddChild(new CBaseButton("ach_lock", tab, "Lock Achievements", [this](CBaseButton*) {
		Achievements_Lock();
	}));*/
	if (TF) ADDCVAR(g_phAutoHeal->v_bEnabled);
	if (TF) ADDCVAR(g_phAntiDisguise->v_bEnabled);
	if (TF2C) {
		ADDCVAR(g_phMisc->v_bTauntSlide);
		ADDCVAR(g_phMisc->v_bCritHack);
	}
	ADDCVAR(g_phMisc->v_bAntiAFK);
	ADDCVAR(g_Settings.bCleanScreenshots);
	ADDCVAR(g_Settings.bHackEnabled);
	if (TF) ADDCVAR(g_Settings.bIgnoreTaunting);
	ADDCVAR(g_Settings.bNoVisuals);
	if (TF) ADDCVAR(g_Settings.bNoZoom);
	//ADDCVAR(g_Settings.bSendPackets);
	ADDCVAR(g_Settings.bShowLogo);
	ADDCVAR(g_Settings.bShowAntiAim);
	if (TF) ADDCVAR(g_Settings.bThirdperson);
	ADDCVAR(g_Settings.bZoomedFOV);
	ADDCVAR(g_Settings.flForceFOVZoomed);
	ADDCVAR(g_Settings.flForceFOV);
	ADDCVAR(g_Settings.sDisconnectMsg);
	ADDCVAR(g_phMisc->v_bCleanChat);
	if (TF2) {
		CBaseButton* but = new CBaseButton("schema", tab, "Load Custom Schema", [this](CBaseButton*) {
			Schema_Reload();
		});
		but->Props()->SetString("tooltip", "Loads a custom item schema from ~/.cathook/\nitems_game.txt");
		tab->AddChild(but);
	}
	if (TF2C) ADDCVAR(g_phMisc->v_bMinigunJump);
	//ADDCVAR(g_phMisc->v_bDebugInfo);
	if (HL2DM) ADDCVAR(g_phMisc->v_bFlashlightSpam);
	ADDCVAR(g_phMisc->v_iFakeLag);
	ADDCVAR(g_Settings.bFastOutline);
	if (TF) {
		ADDLABEL("Spy Alert");
		ADDCVAR(g_phSpyAlert->v_bEnabled);
		ADDCVAR(g_phSpyAlert->v_flBackstabDistance);
		ADDCVAR(g_phSpyAlert->v_flWarningDistance);
	}
	ADDLABEL("Bunnyhop");
	ADDCVAR(g_phBunnyhop->v_bEnabled);
	ADDCVAR(g_phBunnyhop->v_bAutoJump);
	ADDCVAR(g_phBunnyhop->v_iAutoJumpSpeed);
	AddTab("antiaim", "Anti-Aim");
	tab = GetTab("antiaim");
	ADDCVAR(g_phAntiAim->v_bEnabled);
	ADDCVAR(g_phAntiAim->v_PitchMode);
	ADDCVAR(g_phAntiAim->v_flPitch);
	ADDCVAR(g_phAntiAim->v_YawMode);
	ADDCVAR(g_phAntiAim->v_flYaw);
	ADDCVAR(g_phAntiAim->v_flSpinSpeed);
	ADDCVAR(g_phAntiAim->v_bNoClamping);
	ADDCVAR(g_phAntiAim->v_flRoll);
	AddTab("spam", "Spam/Killsay");
	tab = GetTab("spam");
	ADDLABEL("Spam");
	ADDCVAR(g_phSpam->v_bSpam);
	ADDCVAR(g_phSpam->v_bSpamNewlines);
	ADDCVAR(g_phSpam->v_sSpamFile);
	tab->AddChild(new CBaseButton("spam_reload", tab, "Reload spam", [this](CBaseButton*) {
		g_phSpam->Reload();
	}));
	ADDLABEL("Killsay");
	ADDCVAR(g_phKillSay->v_bEnabled);
	ADDCVAR(g_phKillSay->v_sFileName);
	tab->AddChild(new CBaseButton("killsay_reload", tab, "Reload killsays", [this](CBaseButton*) {
		g_phKillSay->Reload();
	}));
}

CMenuContainer* CMenuWindow::GetTab(std::string tab) {
	return dynamic_cast<CMenuContainer*>(ChildByName("tab_" + tab));
}

void CMenuWindow::AddTab(std::string tab, std::string name) {
	m_pList->AddEntry(tab, name);
	CMenuContainer* container = new CMenuContainer("tab_" + tab, this);
	if (!m_pActiveTab) m_pActiveTab = container;
	AddChild(container);
	container->Hide();
}

void CMenuWindow::SelectTab(std::string tab) {
	if (m_pActiveTab) m_pActiveTab->Hide();
	m_pActiveTab = GetTab(tab);
	m_pList->SelectEntry(tab);
	if (m_pActiveTab) m_pActiveTab->Show();
}

void CMenuWindow::MoveChildren() {
	auto ms = GetMaxSize();
	auto th = m_pTitle->GetSize().second;
	SetSize(ms.first, ms.second);
	m_pList->SetMaxSize(200, ms.second - th);
	m_pList->SetSize(200, ms.second - th);
	m_pList->SetOffset(0, th);
	if (m_pActiveTab) {
		m_pActiveTab->SetOffset(200, th);
		m_pActiveTab->SetMaxSize(ms.first - 200, ms.second - th);
		m_pActiveTab->SetSize(ms.first - 200, ms.second - th);
	}
}
