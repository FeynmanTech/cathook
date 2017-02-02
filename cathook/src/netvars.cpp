/*
a * entity.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#include "copypasted/Netvar.h"
#include "common.h"

NetVars netvar;

void NetVars::Init() {
	this->iFlags = gNetvars.get_offset("DT_BasePlayer", "m_fFlags");
	this->iHealth = gNetvars.get_offset("DT_BasePlayer", "m_iHealth");
	this->iLifeState = gNetvars.get_offset("DT_BasePlayer", "m_lifeState");
	this->iTeamNum = gNetvars.get_offset("DT_BaseEntity", "m_iTeamNum");
	this->vViewOffset = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
	this->hActiveWeapon = gNetvars.get_offset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	this->iHitboxSet = gNetvars.get_offset("DT_BaseAnimating", "m_nHitboxSet");
	this->vVelocity = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecVelocity[0]");
	if (TF2) {
		this->bGlowEnabled = gNetvars.get_offset("DT_TFPlayer", "m_bGlowEnabled");
		this->iMaxBuffedHealth = gNetvars.get_offset("DT_TFPlayerResource", "m_iMaxBuffedHealth");
		//this->iDecapitations = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_iDecapitations");
		//this->flReloadPriorNextFire = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flReloadPriorNextFire");
		//this->flObservedCritChance = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flObservedCritChance");
		this->iItemDefinitionIndex = gNetvars.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
		this->AttributeList = gNetvars.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_AttributeList");
		this->flChargeBeginTime = gNetvars.get_offset("DT_WeaponPipebombLauncher", "PipebombLauncherLocalData", "m_flChargeBeginTime");
		this->flLastFireTime = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flLastFireTime");
		this->bDistributed = gNetvars.get_offset("DT_CurrencyPack", "m_bDistributed");
		this->_condition_bits = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_ConditionList", "_condition_bits");
	}
	if (TF2 || TF2C) {
		this->iCond = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
		this->iCond1 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx");
		this->iCond2 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx2");
		this->iCond3 = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx3");
		this->iClass = gNetvars.get_offset("DT_TFPlayer", "m_PlayerClass", "m_iClass");
		this->flChargedDamage = gNetvars.get_offset("DT_TFSniperRifle", "SniperRifleLocalData", "m_flChargedDamage");
		this->iUpgradeLevel = gNetvars.get_offset("DT_BaseObject", "m_iUpgradeLevel");
		this->iPipeType = gNetvars.get_offset("DT_TFProjectile_Pipebomb", "m_iType");
		this->iBuildingHealth = gNetvars.get_offset("DT_BaseObject", "m_iHealth");
		this->iBuildingMaxHealth = gNetvars.get_offset("DT_BaseObject", "m_iMaxHealth");
		this->iReloadMode = gNetvars.get_offset("DT_TFWeaponBase", "m_iReloadMode");
		this->iMaxHealth = gNetvars.get_offset("DT_TFPlayerResource", "m_iMaxHealth");
		this->Rocket_iDeflected = gNetvars.get_offset("DT_TFBaseRocket", "m_iDeflected");
		this->Grenade_iDeflected = gNetvars.get_offset("DT_TFWeaponBaseGrenadeProj", "m_iDeflected");
		this->nForceTauntCam = gNetvars.get_offset("DT_TFPlayer", "m_nForceTauntCam");
		this->Rocket_bCritical = gNetvars.get_offset("DT_TFProjectile_Rocket", "m_bCritical");
		this->Grenade_bCritical = gNetvars.get_offset("DT_TFWeaponBaseGrenadeProj", "m_bCritical");
		this->iPlayerClass = gNetvars.get_offset("DT_TFPlayerResource", "m_iPlayerClass");
		this->angEyeAngles = gNetvars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
	}
	this->flNextAttack = gNetvars.get_offset("DT_BaseCombatCharacter", "bcc_localdata", "m_flNextAttack");
	this->flNextPrimaryAttack = gNetvars.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	this->iNextThinkTick = gNetvars.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_nNextThinkTick");
	this->nTickBase = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_nTickBase");
	this->vecPunchAngle = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_Local", "m_vecPunchAngle");
	this->vecPunchAngleVel = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_Local", "m_vecPunchAngleVel");
	this->hThrower = gNetvars.get_offset("DT_BaseGrenade", "m_hThrower");
	this->hMyWeapons = gNetvars.get_offset("DT_BaseCombatCharacter", "m_hMyWeapons");
	this->iObserverMode = gNetvars.get_offset("DT_BasePlayer", "m_iObserverMode");
	this->hObserverTarget = gNetvars.get_offset("DT_BasePlayer", "m_hObserverTarget");
	this->deadflag = gNetvars.get_offset("DT_BasePlayer", "pl", "deadflag");
	this->iFOV = gNetvars.get_offset("DT_BasePlayer", "m_iFOV");
	this->iDefaultFOV = gNetvars.get_offset("DT_BasePlayer", "m_iDefaultFOV");
	this->hOwner = gNetvars.get_offset("DT_BaseCombatWeapon", "m_hOwner");
}

void InitNetVars() {
	netvar.Init();
}
