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

template<typename T>
inline T GetEntityValue(IClientEntity* ent, unsigned int offset) {
	int null = 0;
	if (ent == 0) return *(reinterpret_cast<T*>(&null));
	//logging::Info("GetEntityValue 0x%08x, 0x%08x", ent, offset);
	return *(reinterpret_cast<T*>((unsigned int)ent + offset));
}
void InitEntityOffsets();

class EntityVariables {
public:
	void Init();
	unsigned int iTeamNum;
	unsigned int iFlags;
	unsigned int iHealth;
	unsigned int iLifeState;
	unsigned int iCond;
	unsigned int iCond1;
	unsigned int iCond2;
	unsigned int iCond3;
	unsigned int iClass;
	unsigned int vViewOffset;
	unsigned int hActiveWeapon;
	unsigned int flChargedDamage;
	unsigned int iUpgradeLevel;
	unsigned int iPipeType;
	unsigned int iBuildingHealth;
	unsigned int iHitboxSet;
	unsigned int vVelocity;
};

extern EntityVariables entityvars;

#endif /* ENTITY_H_ */
