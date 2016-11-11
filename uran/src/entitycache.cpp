/*
 * entitycache.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: nullifiedcat
 */

#include "entitycache.h"
#include "drawing.h"
#include "interfaces.h"
#include "logging.h"
#include "enums.h"
#include "entity.h"
#include "localplayer.h"
#include "helpers.h"

#include "fixsdk.h"
#include <icliententitylist.h>
#include <Color.h>
#include <icliententity.h>
#include <client_class.h>

CachedEntity::CachedEntity() {
	m_pEntity = nullptr;
	m_Strings = new ESPStringCompound[MAX_STRINGS]();
	m_nESPStrings = 0;
}

CachedEntity::~CachedEntity() {
	delete m_Strings;
}

template<typename T>
T CachedEntity::Var(unsigned int offset) {
	if (m_bNULL) {
		logging::Info("Trying to get netvar of NULL entity!");
		return reinterpret_cast<T>(0);
	}
	return GetEntityValue<T>(m_pEntity, offset);
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
	m_bDormant = m_pEntity->IsDormant();
	if (g_pLocalPlayer->entity) {
		m_flDistance = (g_pLocalPlayer->entity->GetAbsOrigin().DistTo(m_pEntity->GetAbsOrigin()));
	}
	m_bAlivePlayer = false;
	if (m_iClassID == ClassID::CTFPlayer) {
		m_bAlivePlayer = !(m_bNULL || m_bDormant || GetEntityValue<char>(m_pEntity, eoffsets.iLifeState));
		m_iTeam = Var<int>(eoffsets.iTeamNum); // TODO
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
		m_bIsVisible = (IsEntityVisible(m_pEntity, 0) || IsEntityVisible(m_pEntity, 4));
		if (m_bIsVisible) m_lLastSeen = 0;
		else m_lLastSeen++;
	}
}

void CachedEntity::AddESPString(Color color, const char* fmt, ...) {
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
	m_Strings[m_nESPStrings].m_String = buffer;
	//logging::Info("String: %s", m_Strings[m_nESPStrings].m_String);
	m_nESPStrings++;
}

ESPStringCompound CachedEntity::GetESPString(int idx) {
	if (idx >= 0 && idx < m_nESPStrings) {
		return m_Strings[idx];
	} else {
		return ESPStringCompound();
	}
}

EntityCache::EntityCache() {
	m_pArray = new CachedEntity[4096]();
}

EntityCache::~EntityCache() {
	logging::Info("Destroying EntityCache!");
	delete m_pArray;
}

void EntityCache::Update() {
	m_nMax = interfaces::entityList->GetHighestEntityIndex();
	for (int i = 0; i < m_nMax && i < 4096; i++) {
		//logging::Info("Updating %i", i);
		m_pArray[i].Update(i);
		//logging::Info("Back!");
	}
}

CachedEntity* EntityCache::GetEntity(int idx) {
	if (idx < 0 || idx >= m_nMax) {
		logging::Info("Requested invalid entity: %i", idx);
	}
	//logging::Info("Request entity: %i, 0x%08x", idx, m_pArray[idx].m_pEntity);
	return &(m_pArray[idx]);
}

EntityCache gEntityCache;
