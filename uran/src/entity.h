/*
 * entity.h
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include "logging.h"

class IClientEntity;

typedef unsigned int offset_t;

template<typename T>
inline T GetEntityValue(IClientEntity* ent, unsigned int offset) {
	int nullv = 0;
	if (ent == 0) return *(reinterpret_cast<T*>(&nullv));
	//logging::Info("GetEntityValue 0x%08x, 0x%08x", ent, offset);
	return *(reinterpret_cast<T*>((unsigned int)ent + offset));
}

template<typename T>
void SetEntityValue(IClientEntity* ent, unsigned int offset, T value) {
	*(reinterpret_cast<T*>((unsigned int)ent + offset)) = value;
}

void InitEntityOffsets();

// TODO move this.

class EntityVariables {
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
	offset_t iGlowIndex;
	offset_t iReloadMode;
	offset_t iMaxHealth;
	offset_t flNextAttack;
	offset_t iNextMeleeCrit;
	offset_t flNextPrimaryAttack;
	offset_t iNextThinkTick;
	offset_t flReloadPriorNextFire;
	offset_t flObservedCritChance;
	offset_t nTickBase;
	offset_t iDecapitations;
	offset_t iMaxBuffedHealth;

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
};

// TODO globals
extern EntityVariables eoffsets;

#endif /* ENTITY_H_ */
