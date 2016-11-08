/*
 * entitycache.h
 *
 *  Created on: Nov 7, 2016
 *      Author: nullifiedcat
 */

#ifndef ENTITYCACHE_H_
#define ENTITYCACHE_H_

class IClientEntity;
class Color;
struct ESPStringCompound;

#define MAX_STRINGS 16

class CachedEntity {
public:
	CachedEntity();
	~CachedEntity();

	void Update(int idx);
	void AddESPString(Color color, const char* string);

	// Entity fields start here.

	bool m_bNULL;
	bool m_bDormant;
	int m_iClassID;

	// CBaseEntity
	int m_iTeamNum;
	bool m_bEnemy;

	// CBasePlayer
	bool m_bIsAlivePlayer;
	int m_fFlags;
	int m_lifeState;
	int m_iHealth;
	Vector m_vecViewOffset;

	// CBaseCombatCharacter
	int m_hActiveWeapon;
	IClientEntity* m_activeWeapon;

	// CTFPlayer
	bool m_bIsCritBoosted;
	bool m_bIsInvulnerable;

	int m_iCond;
	int m_iCondEx;
	int m_iCondEx2;
	int m_iCondEx3;
	float m_flChargeMeter;
	float m_flEnergyDrinkMeter;
	int m_nNumHealers;

	// Players, Buildings, Stickies


	// Entity fields end here.

	IClientEntity* m_pEntity;
	ESPStringCompound* m_Strings;
	int m_nESPStrings;
};

class EntityCache {
public:
	EntityCache();
	~EntityCache();

	void Update();
	CachedEntity* GetEntity(int idx);

	CachedEntity* m_pArray;
	int m_nMax;
};

extern EntityCache gEntityCache;

#endif /* ENTITYCACHE_H_ */
