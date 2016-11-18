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
	this->iBuildingMaxHealth = gNetvars.get_offset("DT_BaseObject", "m_iMaxHealth");
	this->iCond1 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx");
	this->iCond2 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx2");
	this->iCond3 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx3");
	this->iHitboxSet = gNetvars.get_offset("DT_BaseAnimating", "m_nHitboxSet");
	this->vVelocity = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecVelocity[0]");
	this->bGlowEnabled = gNetvars.get_offset("DT_TFPlayer", "m_bGlowEnabled");
	this->iReloadMode = gNetvars.get_offset("DT_TFWeaponBase", "m_iReloadMode");
	this->iMaxHealth = gNetvars.get_offset("DT_TFPlayerResource", "m_iMaxHealth");
	this->flNextAttack = gNetvars.get_offset("DT_BaseCombatCharacter", "bcc_localdata", "m_flNextAttack");
	this->iNextMeleeCrit = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_iNextMeleeCrit");
	this->iDecapitations = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_iDecapitations");
	this->flNextPrimaryAttack = gNetvars.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	this->iNextThinkTick = gNetvars.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_nNextThinkTick");
	this->flReloadPriorNextFire = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flReloadPriorNextFire");
	this->flObservedCritChance = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flObservedCritChance");
	this->nTickBase = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_nTickBase");
}

void InitEntityOffsets() {
	eoffsets.Init();
}
