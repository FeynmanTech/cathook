/*
 * cachedentity.h
 *
 *  Created on: Feb 13, 2017
 *      Author: nullifiedcat
 */

#ifndef CACHEDENTITY_H_
#define CACHEDENTITY_H_

#define MAX_STRINGS 16

#include "beforecheaders.h"
#include <string>
#include "aftercheaders.h"

#include "entityhitboxcache.h"

#include "sdk.h"

struct CachedEntityData {

	k_EEntityType type;
	k_ETFProjectile projectile;
	k_EBuilding building;
	k_EItemType item;

	int team;
	bool enemy;
	int clazz;
	int health;
	int health_max;

	float distance;

	bool has_playerinfo;
	player_info_s info;
	bool esp_enabled;

	static const CachedEntityData zero;
};

class CachedEntity {
public:
	CachedEntity();

	bool bad;

	void SetIDX(int idx);
	void Update();

	template<typename T>
	T var(unsigned int offset) const;
	template<typename T>
	T& var(unsigned int offset);

	template<typename T>
	T operator[](unsigned int offset) const;
	template<typename T>
	T& operator[](unsigned int offset);

	// No longer friends with c-style strings.
	// Now STL is my best friend.
	void AddESPString(std::string string);
	ESPStringCompound& GetESPString(int idx);

	void Zero();

	CachedEntityData data;

	k_EEntityType Type();
	k_ETFProjectile Projectile();
	k_EBuilding Building();
	k_EItemType Item();
	int Team();
	bool Enemy();
	int Class();
	int Health();
	int MaxHealth();
	float Distance();
	const Vector& Origin();
	player_info_s& GetPlayerInfo();

	int clazz;
	ClientClass* m_pClass;

	bool m_bAnyHitboxVisible;
	bool m_bVisCheckComplete;
	bool IsVisible();

	bool bones_setup;
	matrix3x4_t bones[MAXSTUDIOBONES];
	matrix3x4_t (&GetBones())[MAXSTUDIOBONES];

	int ESPcolor;
	EntityHitboxCache hitboxes;
	ESPStringCompound strings[MAX_STRINGS];
	unsigned string_count;
	int m_IDX;
	IClientEntity* entptr;
	Vector m_ESPOrigin;
};

#endif /* CACHEDENTITY_H_ */
