/*
a * entity.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "entity.h"
#include "copypasted/Netvar.h"
#include "logging.h"

// TODO globals
EntityVariables eoffsets;

void EntityVariables::Init() {
	this->iCond = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
	this->iFlags = gNetvars.get_offset("DT_BasePlayer", "m_fFlags");
	this->iHealth = gNetvars.get_offset("DT_BasePlayer", "m_iHealth");
	this->iLifeState = gNetvars.get_offset("DT_BasePlayer", "m_lifeState");
	this->iTeamNum = gNetvars.get_offset("DT_BaseEntity", "m_iTeamNum");
	this->iClass = gNetvars.get_offset("DT_TFPlayer", "m_PlayerClass", "m_iClass");
	this->vViewOffset = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
	this->hActiveWeapon = gNetvars.get_offset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	this->flChargedDamage = gNetvars.get_offset("DT_TFSniperRifle", "SniperRifleLocalData", "m_flChargedDamage");
	this->iUpgradeLevel = gNetvars.get_offset("DT_BaseObject", "m_iUpgradeLevel");
	this->iPipeType = gNetvars.get_offset("DT_TFProjectile_Pipebomb", "m_iType");
	this->iBuildingHealth = gNetvars.get_offset("DT_BaseObject", "m_iHealth");
	this->iCond1 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx");
	this->iCond2 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx2");
	this->iCond3 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx3");
	this->iHitboxSet = gNetvars.get_offset("DT_BaseAnimating", "m_nHitboxSet");
	this->vVelocity = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecVelocity[0]");
	this->bGlowEnabled = gNetvars.get_offset("DT_TFPlayer", "m_bGlowEnabled");
}

void InitEntityOffsets() {
	eoffsets.Init();
}
