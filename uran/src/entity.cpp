/*
a * entity.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "copypasted/Netvar.h"
#include "common.h"

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
	this->iMaxBuffedHealth = gNetvars.get_offset("DT_TFPlayerResource", "m_iMaxBuffedHealth");
	this->flNextAttack = gNetvars.get_offset("DT_BaseCombatCharacter", "bcc_localdata", "m_flNextAttack");
	this->iNextMeleeCrit = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_iNextMeleeCrit");
	this->iDecapitations = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_iDecapitations");
	this->flNextPrimaryAttack = gNetvars.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	this->iNextThinkTick = gNetvars.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_nNextThinkTick");
	this->flReloadPriorNextFire = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flReloadPriorNextFire");
	this->flObservedCritChance = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flObservedCritChance");
	this->nTickBase = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_nTickBase");
	this->vecPunchAngle = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_Local", "m_vecPunchAngle");
	this->vecPunchAngleVel = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_Local", "m_vecPunchAngleVel");
	this->iItemDefinitionIndex = gNetvars.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
	this->AttributeList = gNetvars.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_AttributeList");
	this->flChargeBeginTime = gNetvars.get_offset("DT_WeaponPipebombLauncher", "PipebombLauncherLocalData", "m_flChargeBeginTime");
	this->flLastFireTime = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flLastFireTime");
	this->hThrower = gNetvars.get_offset("DT_BaseGrenade", "m_hThrower");
	this->hMyWeapons = gNetvars.get_offset("DT_BaseCombatCharacter", "m_hMyWeapons");
	this->iObserverMode = gNetvars.get_offset("DT_BasePlayer", "m_iObserverMode");
	this->hObserverTarget = gNetvars.get_offset("DT_BasePlayer", "m_hObserverTarget");
	this->Rocket_iDeflected = gNetvars.get_offset("DT_TFBaseRocket", "m_iDeflected");
	this->Grenade_iDeflected = gNetvars.get_offset("DT_TFWeaponBaseGrenadeProj", "m_iDeflected");
}

void InitEntityOffsets() {
	eoffsets.Init();
}
