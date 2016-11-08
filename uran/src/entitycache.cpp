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

void CachedEntity::Update(int idx) {
	m_pEntity = interfaces::entityList->GetClientEntity(idx);
	if (!m_pEntity) {
		//logging::Info("Tried to cache entity with index %i, null");
		m_bNULL = true;
		return;
	}
	m_nESPStrings = 0;

	m_iClassID = m_pEntity->GetClientClass()->m_ClassID;
	m_bDormant = m_pEntity->IsDormant();

	if (m_iClassID == ClassID::CTFPlayer
		|| m_iClassID == ClassID::CObjectSentrygun
		|| m_iClassID == ClassID::CObjectDispenser
		|| m_iClassID == ClassID::CObjectTeleporter
		|| m_iClassID == ClassID::CTFStickBomb
		|| m_iClassID == ClassID::CTFGrenadePipebombProjectile) {

		m_iTeam = GetEntityValue<int>(m_pEntity, eoffsets.iTeamNum);
		m_bEnemy = (m_iTeam != g_pLocalPlayer->team);
		m_iHealth = GetEntityValue<int>(m_pEntity, eoffsets.iHealth);
	}
}

void CachedEntity::AddESPString(Color color, const char* string) {
	if (m_nESPStrings >= MAX_STRINGS) {
		logging::Info("Can't attach more than %i strings to an entity", MAX_STRINGS);
		return;
	}
	if (m_nESPStrings < 0) {
		logging::Info("Invalid string count !!!");
		return;
	}
	m_Strings[m_nESPStrings].m_Color = &color;
	m_Strings[m_nESPStrings].m_String = (char*)string;
	m_nESPStrings++;
}

EntityCache::EntityCache() {
	m_pArray = new CachedEntity[4096]();
}

EntityCache::~EntityCache() {
	delete m_pArray;
}

void EntityCache::Update() {
	m_nMax = interfaces::entityList->GetHighestEntityIndex();
	for (int i = 0; i < m_nMax; i++) {
		m_pArray[i].Update(i);
	}
}

CachedEntity* EntityCache::GetEntity(int idx) {
	if (idx < 0 || idx >= m_nMax) {
		logging::Info("Requested invalid entity: %i", idx);
	}
	return &(m_pArray[idx]);
}

EntityCache gEntityCache;
