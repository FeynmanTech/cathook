/*
 * cachedentity.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: nullifiedcat
 */

#include "cachedentity.h"

#include "common.h"
#include "profiler.h"
#include "studio.h"

const CachedEntityData CachedEntityData::zero = {
	ENTITY_UNKNOWN, PROJ_UNKNOWN, BUILD_UNKNOWN, ITEM_UNKNOWN,
	-1, false, -1, -1, -1, -1.0f, false, { 0 }, false
};

k_EItemType CachedEntity::Item() {
	if (data.item == ITEM_UNKNOWN) {
		data.item = g_ItemManager.GetItemType(this);
	}
	return data.item;
}

k_EBuilding CachedEntity::Building() {
	if (data.building == BUILD_UNKNOWN) {
		data.building = BUILD_NONE;
		int clazz = clazz;
		if (clazz == g_pClassID->CObjectDispenser)
			data.building = BUILD_DISPENSER;
		else if (clazz == g_pClassID->CObjectSentrygun)
			data.building = BUILD_SENTRYGUN;
		else if (clazz == g_pClassID->CObjectTeleporter)
			data.building = BUILD_TELEPORTER;
	}
	return data.building;
}

// TODO add more types!
k_ETFProjectile CachedEntity::Projectile() {
	if (data.projectile == PROJ_UNKNOWN) {
		data.projectile = PROJ_NONE;
		int clazz = clazz;
		if (clazz == g_pClassID->CTFProjectile_Rocket ||
			clazz == g_pClassID->CTFProjectile_SentryRocket) {
			data.projectile = PROJ_ROCKET;
		} else if (clazz == g_pClassID->CTFProjectile_Arrow) {
			data.projectile = PROJ_ARROW;
		} else if (clazz == g_pClassID->CTFGrenadePipebombProjectile) {
			if (var<int>(netvar.iPipeType) == 0) {
				data.projectile = PROJ_PIPE;
			} else {
				data.projectile = PROJ_STICKY;
			}
		}
	}
	return data.projectile;
}


void CachedEntity::Zero() {
	string_count = 0;
	clazz = -1;
	m_pClass = nullptr;
	m_bVisCheckComplete = false;
	bones_setup = false;
	data = CachedEntityData::zero;
}

void CachedEntity::AddESPString(std::string string) {
	strings[string_count].m_bColored = false;
	strings[string_count].m_string = string;
	string_count++;
}

ESPStringCompound& CachedEntity::GetESPString(int idx) {
	return &strings[idx];
}

matrix3x4_t (&CachedEntity::GetBones())[MAXSTUDIOBONES] {
	if (!bones_setup) {
		bones_setup = entptr->SetupBones(&bones[0], MAXSTUDIOBONES, 0x100, 0); // gvars->curtime
	}
	return &bones;
}

int CachedEntity::Team() {
	if (data.team == -1) {
		data.team = var<int>(netvar.iTeamNum);
		data.enemy = data.team != g_LocalPlayer.entity->Team();
	}
	return data.team;
}

int CachedEntity::Class() {
	if (data.clazz == -1) {
		data.clazz = g_pPlayerResource->GetClass(this);
	}
	return data.clazz;
}

int CachedEntity::Health() {
	if (data.health == -1) {
		data.health = vfunc<int(IClientEntity*)>(entptr, 152, 0)(entptr);
	}
	return data.health;
}

int CachedEntity::MaxHealth() {
	if (data.health_max == -1) {
		data.health_max = vfunc<int(IClientEntity*)>(entptr, 153, 0)(entptr);
	}
	return data.health_max;
}

float CachedEntity::Distance() {
	if (data.distance == -1.0f) {
		data.distance = g_LocalPlayer.v_Eye.DistTo(Origin());
	}
	return data.distance;
}

bool CachedEntity::Enemy() {
	if (data.team == -1) {
		Team();
	}
	return data.enemy;
}

const Vector& CachedEntity::Origin() {
	return entptr->GetAbsOrigin();
}

player_info_s& CachedEntity::GetPlayerInfo() {
	if (!data.has_playerinfo) {
		g_IEngine->GetPlayerInfo(m_IDX, &data.info);
	}
	return &data.info;
}

template<typename T>
T CachedEntity::operator[](unsigned int offset) const {
	return *reinterpret_cast<T*>(entptr + offset);
}

template<typename T>
T& CachedEntity::operator[](unsigned int offset) {
	return *reinterpret_cast<T*>(entptr + offset);
}

template<typename T>
T CachedEntity::var(unsigned int offset) const {
	return *reinterpret_cast<T*>(entptr + offset);
}

template<typename T>
T& CachedEntity::var(unsigned int offset) {
	return *reinterpret_cast<T*>(entptr + offset);
}


CachedEntity::CachedEntity() {
	Zero();
}

void CachedEntity::SetIDX(int idx) {
	m_IDX = idx;
}

void CachedEntity::Update() {
	SEGV_BEGIN
	bad = true;

	//m_ESPOrigin.Zero();
	Zero();
	entptr = g_IEntityList->GetClientEntity(m_IDX);
	if (!entptr) {
		return;
	}
	clazz = entptr->GetClientClass()->m_ClassID;
	bad = false;
	//Vector origin = m_pEntity->GetAbsOrigin();
	//if (TF2 && EstimateAbsVelocity) EstimateAbsVelocity(m_pEntity, m_vecVelocity);
	/*if ((gvars->realtime - m_fLastUpdate) >= 0.05f) {
		//if (gvars->tickcount - m_nLastTick > 1) {
			//logging::Info("Running %i ticks behind!", gvars->tickcount - m_nLastTick);
		//}
		//Vector velnew = (origin - m_vecVOrigin) * (0.05f / (m_fLastUpdate - gvars->realtime)) * 20;
		Vector velnew;
		if (EstimateAbsVelocity)
			EstimateAbsVelocity(m_pEntity, velnew);
		m_vecAcceleration = (velnew - m_vecVelocity);
		m_vecVelocity = (m_vecVelocity + velnew) / 2;
		//logging::Info("Multiplier for %i: %f", m_IDX, (0.1f / (m_fLastUpdate - gvars->realtime)));
		m_vecVOrigin = origin;
		m_fLastUpdate = gvars->realtime;
	}*/
	//m_vecOrigin = origin;

	/*m_bGrenadeProjectile = false;
	bones_setup = false;

	m_bVisCheckComplete = false;
	SAFE_CALL(hitboxes.Update());

	/*if (m_iClassID == g_pClassID->C_Player) {
		m_Type = k_EEntityType::ENTITY_PLAYER;
	} else if (m_iClassID == g_pClassID->CTFGrenadePipebombProjectile ||
			   m_iClassID == g_pClassID->CTFProjectile_Cleaver ||
			   m_iClassID == g_pClassID->CTFProjectile_Jar ||
			   m_iClassID == g_pClassID->CTFProjectile_JarMilk) {
		m_Type = k_EEntityType::ENTITY_PROJECTILE;
		m_bGrenadeProjectile = true;
	} else if (m_iClassID == g_pClassID->CObjectTeleporter ||
			   m_iClassID == g_pClassID->CObjectSentrygun ||
			   m_iClassID == g_pClassID->CObjectDispenser) {
		m_Type = k_EEntityType::ENTITY_BUILDING;
	} else if (m_iClassID == g_pClassID->CTFProjectile_Arrow ||
			   m_iClassID == g_pClassID->CTFProjectile_EnergyBall ||
			   m_iClassID == g_pClassID->CTFProjectile_EnergyRing ||
			   m_iClassID == g_pClassID->CTFProjectile_GrapplingHook ||
			   m_iClassID == g_pClassID->CTFProjectile_HealingBolt ||
			   m_iClassID == g_pClassID->CTFProjectile_Rocket ||
			   m_iClassID == g_pClassID->CTFProjectile_SentryRocket ||
			   m_iClassID == g_pClassID->CTFProjectile_Flare) {
		m_Type = k_EEntityType::ENTITY_PROJECTILE;
	} else {
		m_Type = k_EEntityType::ENTITY_GENERIC;
	}

	if (CE_GOOD(g_pLocalPlayer->entity)) {
		m_flDistance = (g_pLocalPlayer->v_Origin.DistTo(m_vecOrigin));
	}
	m_bAlivePlayer = false;*/

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

	//if (CE_BAD(g_pLocalPlayer->entity)) return;

	/*if (m_Type == k_EEntityType::ENTITY_PROJECTILE) {
		m_bCritProjectile = IsProjectileCrit(this);
		m_iTeam = CE_INT(this, netvar.iTeamNum);
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
	}

	if (m_Type == k_EEntityType::ENTITY_PLAYER) {
		m_bAlivePlayer = !(NET_BYTE(m_pEntity, netvar.iLifeState));
		if (m_pPlayerInfo) {
			delete m_pPlayerInfo;
			m_pPlayerInfo = 0;
		}
		m_pPlayerInfo = new player_info_s;
		g_IEngine->GetPlayerInfo(m_IDX, m_pPlayerInfo);
		m_iTeam = CE_INT(this, netvar.iTeamNum); // TODO
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
		m_iHealth = CE_INT(this, netvar.iHealth);
		m_iMaxHealth = g_pPlayerResource->GetMaxHealth(this);
		if (IsVisible()) {
			m_lLastSeen = 0;
			m_lSeenTicks++;
		} else {
			m_lSeenTicks = 0;
			m_lLastSeen++;
		}
	}
	if (m_Type == k_EEntityType::ENTITY_BUILDING) {
		m_iTeam = CE_INT(this, netvar.iTeamNum); // TODO
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
		m_iHealth = CE_INT(this, netvar.iBuildingHealth);
		m_iMaxHealth = CE_INT(this, netvar.iBuildingMaxHealth);
		if (IsVisible()) {
			m_lLastSeen = 0;
			m_lSeenTicks++;
		} else {
			m_lSeenTicks = 0;
			m_lLastSeen++;
		}
	}*/

	SEGV_END_INFO("Updating entity")
}

bool CachedEntity::IsVisible() {
	PROF_SECTION(entityvischeck, "VisChecking");
	if (m_bVisCheckComplete) return m_bAnyHitboxVisible;

	bool vischeck0 = false;
	SAFE_CALL(vischeck0 = IsEntityVectorVisible(this, Origin()));

	if (vischeck0) {
		m_bAnyHitboxVisible = true;
		m_bVisCheckComplete = true;
		return true;
	}

	for (int i = 0; i < hitboxes.HitboxCount(); i++) {
		bool vischeck = false;
		SAFE_CALL(vischeck = hitboxes.VisibilityCheck(i));
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

