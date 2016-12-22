/*
 * entitycache.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"

#include <time.h>

#if ENTITY_CACHE_PROFILER == true
	CatVar* g_vEntityCacheProfiling = 0;
#endif

CachedEntity::CachedEntity() {
	m_pEntity = nullptr;
	m_Strings = new ESPStringCompound[MAX_STRINGS]();
	m_nESPStrings = 0;
}

CachedEntity::~CachedEntity() {
	delete m_Strings;
}

void CachedEntity::Update(int idx) {
	m_ESPOrigin.Zero();
	m_nESPStrings = 0;
	m_IDX = idx;
	m_pEntity = interfaces::entityList->GetClientEntity(idx);
	if (!m_pEntity) {
		//logging::Info("Tried to cache entity with index %i, null");
		m_bNULL = true;
		return;
	} else {
		m_bNULL = false;
	}
	m_iClassID = m_pEntity->GetClientClass()->m_ClassID;

	switch (m_iClassID) {
	case ClassID::CTFPlayer:
		m_Type = EntityType::ENTITY_PLAYER; break;
	case ClassID::CObjectTeleporter:
	case ClassID::CObjectSentrygun:
	case ClassID::CObjectDispenser:
		m_Type = EntityType::ENTITY_BUILDING; break;
	case ClassID::CTFGrenadePipebombProjectile:
	case ClassID::CTFProjectile_Arrow:
	case ClassID::CTFProjectile_Cleaver:
	case ClassID::CTFProjectile_EnergyBall:
	case ClassID::CTFProjectile_EnergyRing:
	case ClassID::CTFProjectile_Flare:
	case ClassID::CTFProjectile_GrapplingHook:
	case ClassID::CTFProjectile_HealingBolt:
	case ClassID::CTFProjectile_Jar:
	case ClassID::CTFProjectile_JarMilk:
	case ClassID::CTFProjectile_Rocket:
	case ClassID::CTFProjectile_SentryRocket:
		// TODO Spells
		m_Type = EntityType::ENTITY_PROJECTILE; break;
	default:
		m_Type = EntityType::ENTITY_GENERIC; break;
	}

	m_vecOrigin = m_pEntity->GetAbsOrigin();

	m_bDormant = m_pEntity->IsDormant();
	if (g_pLocalPlayer->entity) {
		m_flDistance = (g_pLocalPlayer->entity->GetAbsOrigin().DistTo(m_pEntity->GetAbsOrigin()));
	}
	m_bAlivePlayer = false;
	if (IsProjectile(m_pEntity)) {
		m_bCritProjectile = IsProjectileCrit(m_pEntity);
		m_bIsVisible = IsEntityVisible(m_pEntity, -1);
		m_iTeam = CE_INT(this, netvar.iTeamNum);
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
	}

	if (m_iClassID == ClassID::CTFPlayer) {
		m_bAlivePlayer = !(m_bNULL || m_bDormant || NET_BYTE(m_pEntity, netvar.iLifeState));
		m_iTeam = CE_INT(this, netvar.iTeamNum); // TODO
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
		m_bIsVisible = (IsEntityVisible(m_pEntity, 0) || IsEntityVisible(m_pEntity, 4));
		m_iHealth = CE_INT(this, netvar.iHealth);
		m_iMaxHealth = g_pPlayerResource->GetMaxHealth(m_pEntity);
		if (m_bIsVisible) {
			m_lLastSeen = 0;
			m_lSeenTicks++;
		} else {
			m_lSeenTicks = 0;
			m_lLastSeen++;
		}
	}
	if (m_iClassID == ClassID::CObjectSentrygun || m_iClassID == ClassID::CObjectDispenser || m_iClassID == ClassID::CObjectTeleporter) {
		m_iTeam = CE_INT(this, netvar.iTeamNum); // TODO
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
		m_bIsVisible = (IsEntityVisible(m_pEntity, 0));
		m_iHealth = CE_INT(this, netvar.iBuildingHealth);
		m_iMaxHealth = CE_INT(this, netvar.iBuildingMaxHealth);
		if (m_bIsVisible) {
			m_lLastSeen = 0;
			m_lSeenTicks++;
		} else {
			m_lSeenTicks = 0;
			m_lLastSeen++;
		}
	}
}

void CachedEntity::AddESPString(Color color, Color background, const char* fmt, ...) {
#if ENTITY_CACHE_PROFILER == true
	gEntityCache.m_nStringsAdded++;
#endif
	if (m_Strings[m_nESPStrings].m_String) {
		delete m_Strings[m_nESPStrings].m_String;
	}
	char* buffer = new char[1024]();
	va_list list;
	va_start(list, fmt);
	vsprintf(buffer, fmt, list);
	va_end(list);
	if (m_nESPStrings >= MAX_STRINGS) {
		logging::Info("Can't attach more than %i strings to an entity", MAX_STRINGS);
		return;
	}
	if (m_nESPStrings < 0) {
		logging::Info("Invalid string count !!!");
		return;
	}
	m_Strings[m_nESPStrings].m_Color = color;
	m_Strings[m_nESPStrings].m_Background = background;
	m_Strings[m_nESPStrings].m_String = buffer;
	//logging::Info("String: %s", m_Strings[m_nESPStrings].m_String);
	m_nESPStrings++;
}

ESPStringCompound CachedEntity::GetESPString(int idx) {
#if ENTITY_CACHE_PROFILER == true
	gEntityCache.m_nStringsQueued++;
#endif
	if (idx >= 0 && idx < m_nESPStrings) {
		return m_Strings[idx];
	} else {
		return ESPStringCompound();
	}
}

EntityCache::EntityCache() {
	m_pArray = new CachedEntity[4096]();
	m_lLastLog = 0;
}

EntityCache::~EntityCache() {
	logging::Info("Destroying EntityCache!");
	delete m_pArray;
}

void EntityCache::Update() {
	m_nMax = interfaces::entityList->GetHighestEntityIndex();
	for (int i = 0; i < m_nMax && i < 1024; i++) {
		//logging::Info("Updating %i", i);
		m_pArray[i].Update(i);
		//logging::Info("Back!");
	}
#if ENTITY_CACHE_PROFILER == true
	m_nUpdates++;
	if (time(0) != m_lLastLog) {
		m_lLastLog = time(0);
		if (g_vEntityCacheProfiling && g_vEntityCacheProfiling->GetBool()) {
			logging::Info("[EntityCache] TOTAL: UPS=%i QPS=%i SQPS=%i SAPS=%i", m_nUpdates, m_nQueues, m_nStringsQueued, m_nStringsAdded);
			if (m_nUpdates != 0) logging::Info("[EntityCache] AVG: QPU=%i SQPU=%i SAPU=%i", m_nQueues / m_nUpdates, m_nStringsQueued / m_nUpdates, m_nStringsAdded / m_nUpdates);
			m_nUpdates = 0;
			m_nQueues = 0;
			m_nStringsQueued = 0;
			m_nStringsAdded = 0;
		}
	}
#endif
}

CachedEntity* EntityCache::GetEntity(int idx) {
	if (idx < 0 || idx >= m_nMax) {
		logging::Info("Requested invalid entity: %i max %i", idx, m_nMax);
	}
	//logging::Info("Request entity: %i, 0x%08x", idx, m_pArray[idx].m_pEntity);
#if ENTITY_CACHE_PROFILER == true
	m_nQueues++;
#endif
	return &(m_pArray[idx]);
}

EntityCache gEntityCache;
