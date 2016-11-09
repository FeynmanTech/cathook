/*
 * trace.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: nullifiedcat
 */

#include "trace.h"
#include "logging.h"

#include <cstdint>

#include "fixsdk.h"
#include <client_class.h>
#include <icliententity.h>


trace::FilterDefault::FilterDefault() {
	m_pSelf = nullptr;
}

trace::FilterDefault::~FilterDefault() {};

void trace::FilterDefault::SetSelf(IClientEntity* self) {
	if (self == nullptr) {
		logging::Info("nullptr in FilterDefault::SetSelf");
	}
	m_pSelf = self;
}

bool trace::FilterDefault::ShouldHitEntity(IHandleEntity* handle, int mask) {
	if (!handle) return false;
	IClientEntity* entity = (IClientEntity*) handle;
	ClientClass* clazz = entity->GetClientClass();
	/* Ignore invisible entities that we don't wanna hit */
	switch(clazz->m_ClassID) {
	// TODO magic numbers: invisible entity ids
	case 64:
	case 225:
	case 55:
		return false;
	}
	/* Do not hit yourself. Idiot. */
	if (entity == m_pSelf) return false;
	return true;
}

TraceType_t trace::FilterDefault::GetTraceType() const {
	return TRACE_EVERYTHING;
}

trace::FilterNoPlayer::FilterNoPlayer() {
	m_pSelf = nullptr;
}

trace::FilterNoPlayer::~FilterNoPlayer() {};

void trace::FilterNoPlayer::SetSelf(IClientEntity* self) {
	if (self == nullptr) {
		logging::Info("nullptr in FilterNoPlayer::SetSelf");
		return;
	}
	m_pSelf = self;
}

bool trace::FilterNoPlayer::ShouldHitEntity(IHandleEntity* handle, int mask) {
	if (!handle) return false;
	IClientEntity* entity = (IClientEntity*) handle;
	ClientClass* clazz = entity->GetClientClass();
	/* Ignore invisible entities that we don't wanna hit */
	switch(clazz->m_ClassID) {
	// TODO magic numbers: invisible entity ids
	case 64:
	case 225:
	case 55:
	case 241:
		return false;
	}
	/* Do not hit yourself. Idiot. */
	if (entity == m_pSelf) return false;
	return true;
}

TraceType_t trace::FilterNoPlayer::GetTraceType() const {
	return TRACE_EVERYTHING;
}
