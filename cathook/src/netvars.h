/*
 * entity.h
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#ifndef NETVARS_H_
#define NETVARS_H_

class IClientEntity;

#define NET_VAR(entity, offset, type) \
	(*(reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(entity) + offset)))

#define NET_INT(entity, offset) \
	NET_VAR(entity, offset, int)

#define NET_FLOAT(entity, offset) \
	NET_VAR(entity, offset, float)

#define NET_BYTE(entity, offset) \
	NET_VAR(entity, offset, unsigned char)

#define NET_VECTOR(entity, offset) \
	NET_VAR(entity, offset, Vector)

typedef unsigned int offset_t;

void InitNetVars();

class NetVars {
public:
	void Init();
	offset_t iTeamNum;
	offset_t iFlags;
	offset_t iHealth;
	offset_t iLifeState;
	offset_t iCond;
	offset_t iCond1;
	offset_t iCond2;
	offset_t iCond3;
	offset_t iClass;
	offset_t vViewOffset;
	offset_t hActiveWeapon;
	offset_t flChargedDamage;
	offset_t iUpgradeLevel;
	offset_t iPipeType;
	offset_t iBuildingHealth;
	offset_t iBuildingMaxHealth;
	offset_t iHitboxSet;
	offset_t vVelocity;
	offset_t bGlowEnabled;
	offset_t movetype;
	offset_t iGlowIndex;
	offset_t iReloadMode;
	offset_t iMaxHealth;
	offset_t flNextAttack;
	offset_t iNextMeleeCrit;
	offset_t flNextPrimaryAttack;
	offset_t iNextThinkTick;
	//offset_t flReloadPriorNextFire;
	//offset_t flObservedCritChance;
	offset_t nTickBase;
	//offset_t iDecapitations;
	offset_t iMaxBuffedHealth;
	offset_t bRespawning;
	offset_t iItemDefinitionIndex;
	offset_t AttributeList;

	offset_t vecPunchAngle;
	offset_t vecPunchAngleVel;

	offset_t iObserverMode;
	offset_t hObserverTarget;

	offset_t flChargeBeginTime;
	offset_t flLastFireTime;
	offset_t hThrower;
	offset_t hMyWeapons;

	offset_t Rocket_iDeflected;
	offset_t Grenade_iDeflected;
	offset_t Rocket_bCritical;
	offset_t Grenade_bCritical;

	offset_t bDistributed;

	offset_t angEyeAngles;
	offset_t deadflag;
	offset_t nForceTauntCam;

	offset_t iDefaultFOV;
	offset_t iFOV;
	offset_t _condition_bits;
	offset_t iPlayerClass;

	offset_t hOwner;
	offset_t iWeaponState;
	offset_t iCritMult; // TF2C
};

extern NetVars netvar;

#endif /* NETVARS_H_ */
