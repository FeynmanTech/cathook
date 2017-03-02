/*
 * entitycache.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: nullifiedcat
 */

#include "common.h"
#include "sdk.h"

#include <time.h>
#include "profiler.h"

CachedEntity::CachedEntity() {
	m_pEntity = nullptr;
	m_Strings = new ESPStringCompound[MAX_STRINGS]();
	m_nESPStrings = 0;
	m_Bones = 0;
	m_Bones = new matrix3x4_t[MAXSTUDIOBONES];
	m_pHitboxCache = new EntityHitboxCache(this);
	m_pPlayerInfo = 0;
	m_fLastUpdate = 0.0f;
}

CachedEntity::~CachedEntity() {
	delete [] m_Strings;
	delete [] m_Bones;
	delete m_pHitboxCache;
}

IClientEntity* CachedEntity::InternalEntity() {
	return m_pEntity;
}

void EntityCache::Invalidate() {
	delete [] m_pArray;
	m_pArray = new CachedEntity[MAX_ENTITIES]();
}

void CachedEntity::Update(int idx) {
	SEGV_BEGIN

	m_ESPOrigin.Zero();

	m_IDX = idx;
	m_pEntity = interfaces::entityList->GetClientEntity(idx);
	if (!m_pEntity) {
		return;
	}
	m_iClassID = m_pEntity->GetClientClass()->m_ClassID;

	Vector origin = m_pEntity->GetAbsOrigin();
	//if (TF2 && EstimateAbsVelocity) EstimateAbsVelocity(m_pEntity, m_vecVelocity);
	/*if ((interfaces::gvars->realtime - m_fLastUpdate) >= 0.05f) {
		//if (interfaces::gvars->tickcount - m_nLastTick > 1) {
			//logging::Info("Running %i ticks behind!", interfaces::gvars->tickcount - m_nLastTick);
		//}
		//Vector velnew = (origin - m_vecVOrigin) * (0.05f / (m_fLastUpdate - interfaces::gvars->realtime)) * 20;
		Vector velnew;
		if (EstimateAbsVelocity)
			EstimateAbsVelocity(m_pEntity, velnew);
		m_vecAcceleration = (velnew - m_vecVelocity);
		m_vecVelocity = (m_vecVelocity + velnew) / 2;
		//logging::Info("Multiplier for %i: %f", m_IDX, (0.1f / (m_fLastUpdate - interfaces::gvars->realtime)));
		m_vecVOrigin = origin;
		m_fLastUpdate = interfaces::gvars->realtime;
	}*/
	m_vecOrigin = origin;

	m_ItemType = ITEM_NONE;

	m_lSeenTicks = 0;
	m_lLastSeen = 0;

	m_bGrenadeProjectile = false;
	m_bBonesSetup = false;

	m_bVisCheckComplete = false;
	if (m_pHitboxCache) {
		SAFE_CALL(m_pHitboxCache->Update());
	}

	if (m_iClassID == g_pClassID->C_Player) {
		m_Type = EntityType::ENTITY_PLAYER;
	} else if (m_iClassID == g_pClassID->CTFGrenadePipebombProjectile ||
			   m_iClassID == g_pClassID->CTFProjectile_Cleaver ||
			   m_iClassID == g_pClassID->CTFProjectile_Jar ||
			   m_iClassID == g_pClassID->CTFProjectile_JarMilk) {
		m_Type = EntityType::ENTITY_PROJECTILE;
		m_bGrenadeProjectile = true;
	} else if (m_iClassID == g_pClassID->CObjectTeleporter ||
			   m_iClassID == g_pClassID->CObjectSentrygun ||
			   m_iClassID == g_pClassID->CObjectDispenser) {
		m_Type = EntityType::ENTITY_BUILDING;
	} else if (m_iClassID == g_pClassID->CTFProjectile_Arrow ||
			   m_iClassID == g_pClassID->CTFProjectile_EnergyBall ||
			   m_iClassID == g_pClassID->CTFProjectile_EnergyRing ||
			   m_iClassID == g_pClassID->CTFProjectile_GrapplingHook ||
			   m_iClassID == g_pClassID->CTFProjectile_HealingBolt ||
			   m_iClassID == g_pClassID->CTFProjectile_Rocket ||
			   m_iClassID == g_pClassID->CTFProjectile_SentryRocket ||
			   m_iClassID == g_pClassID->CTFProjectile_Flare) {
		m_Type = EntityType::ENTITY_PROJECTILE;
	} else {
		m_ItemType = g_ItemManager.GetItemType(this);
		m_Type = EntityType::ENTITY_GENERIC;
	}

	if (CE_GOOD(g_pLocalPlayer->entity)) {
		m_flDistance = (g_pLocalPlayer->v_Origin.DistTo(m_vecOrigin));
	}
	m_bAlivePlayer = false;

	// TODO temporary!
	/*m_bCritProjectile = false;
	m_bIsVisible = false;
	m_iTeam = 0;
	m_bEnemy = false;
	m_bAlivePlayer = false;
	m_pPlayerInfo = 0;
	m_iHealth = 0;
	m_iMaxHealth = 0;
	m_lLastSeen = 0;
	m_lSeenTicks = 0;*/

	if (CE_BAD(g_pLocalPlayer->entity)) return;

	if (m_Type == EntityType::ENTITY_PROJECTILE) {
		m_bCritProjectile = IsProjectileCrit(this);
		m_iTeam = CE_INT(this, netvar.iTeamNum);
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
	}

	if (m_Type == EntityType::ENTITY_PLAYER) {
		m_bAlivePlayer = !(NET_BYTE(m_pEntity, netvar.iLifeState));
		if (m_pPlayerInfo) {
			delete m_pPlayerInfo;
			m_pPlayerInfo = 0;
		}
		m_pPlayerInfo = new player_info_s;
		interfaces::engineClient->GetPlayerInfo(m_IDX, m_pPlayerInfo);
		m_iTeam = CE_INT(this, netvar.iTeamNum); // TODO
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
		m_iHealth = CE_INT(this, netvar.iHealth);
		m_iMaxHealth = g_pPlayerResource->GetMaxHealth(this);
	}
	if (m_Type == EntityType::ENTITY_BUILDING) {
		m_iTeam = CE_INT(this, netvar.iTeamNum); // TODO
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
		m_iHealth = CE_INT(this, netvar.iBuildingHealth);
		m_iMaxHealth = CE_INT(this, netvar.iBuildingMaxHealth);
	}
	SEGV_END_INFO("Updating entity")
}

bool CachedEntity::IsVisible() {
	PROF_SECTION(CE_IsVisible);
	if (m_bVisCheckComplete) return m_bAnyHitboxVisible;

	bool vischeck0 = false;
	SAFE_CALL(vischeck0 = IsEntityVectorVisible(this, m_vecOrigin));

	if (vischeck0) {
		m_bAnyHitboxVisible = true;
		m_bVisCheckComplete = true;
		return true;
	}

	if (m_Type == ENTITY_PLAYER && g_Settings.bFastVischeck->GetBool()) {
		static const int hitboxes[] = { hitbox_t::head, hitbox_t::foot_L, hitbox_t::hand_R, hitbox_t::spine_1 };
		for (int i = 0; i < 4; i++) {
			if (m_pHitboxCache->VisibilityCheck(hitboxes[i])) {
				m_bAnyHitboxVisible = true;
				m_bVisCheckComplete = true;
				return true;
			}
		}
		m_bAnyHitboxVisible = false;
		m_bVisCheckComplete = true;
		return false;
	}

	for (int i = 0; i < m_pHitboxCache->m_nNumHitboxes; i++) {
		bool vischeck = false;
		SAFE_CALL(vischeck = m_pHitboxCache->VisibilityCheck(i));
		if (vischeck) {
			m_bAnyHitboxVisible = true;
			m_bVisCheckComplete = true;
			return true;
		}
	}
	m_bAnyHitboxVisible = false;
	m_bVisCheckComplete = true;

	return false;
}

void CachedEntity::AddESPString(const char* fmt, ...) {
	if (m_Strings[m_nESPStrings].m_String) {
		delete m_Strings[m_nESPStrings].m_String;
	}
	m_Strings[m_nESPStrings].m_bColored = false;
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
	m_Strings[m_nESPStrings].m_String = buffer;
	//logging::Info("String: %s", m_Strings[m_nESPStrings].m_String);
	m_nESPStrings++;
}

ESPStringCompound& CachedEntity::GetESPString(int idx) {
	//if (idx >= 0 && idx < m_nESPStrings) {
	return m_Strings[idx];

}

matrix3x4_t* CachedEntity::GetBones() {
	if (!m_bBonesSetup) {
		m_bBonesSetup = m_pEntity->SetupBones(m_Bones, MAXSTUDIOBONES, 0x100, 0); // interfaces::gvars->curtime
	}
	return m_Bones;
}

EntityCache::EntityCache() {
	m_pArray = new CachedEntity[MAX_ENTITIES]();
}

EntityCache::~EntityCache() {
	delete [] m_pArray;
}

void CachedEntity::PruneStrings() {
	m_nESPStrings = 0;
}

void EntityCache::PruneStrings() {
	for (int i = 0; i < m_nMax && i < MAX_ENTITIES; i++) m_pArray[i].PruneStrings();
}

void EntityCache::Update() {
	m_nMax = interfaces::entityList->GetHighestEntityIndex();
	for (int i = 0; i < m_nMax && i < MAX_ENTITIES; i++) {
		//logging::Info("Updating %i", i);
		m_pArray[i].Update(i);
		//logging::Info("Back!");
	}
}

CachedEntity* EntityCache::GetEntity(int idx) {
	if (idx < 0 || idx >= m_nMax) {
		logging::Info("Requested invalid entity: %i max %i", idx, m_nMax);
	}
	//logging::Info("Request entity: %i, 0x%08x", idx, m_pArray[idx].m_pEntity);
	return &(m_pArray[idx]);
}

EntityCache gEntityCache;
