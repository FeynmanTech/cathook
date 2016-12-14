/*
 * trace.h
 *
 *  Created on: Oct 10, 2016
 *      Author: nullifiedcat
 */

#ifndef TRACE_H_
#define TRACE_H_

#include "fixsdk.h"
#include <engine/IEngineTrace.h>

// This file is a mess. I need to fix it. TODO

class IClientEntity;

namespace trace {

class FilterDefault : public ITraceFilter {
public:
	IClientEntity* m_pSelf;
public:
	virtual ~FilterDefault();
	FilterDefault();
	virtual bool ShouldHitEntity(IHandleEntity* entity, int mask);
	void SetSelf(IClientEntity* self);
	virtual TraceType_t GetTraceType() const;
};

class FilterNoPlayer : public ITraceFilter {
public:
	IClientEntity* m_pSelf;
public:
	virtual ~FilterNoPlayer();
	FilterNoPlayer();
	virtual bool ShouldHitEntity(IHandleEntity* entity, int mask);
	void SetSelf(IClientEntity* self);
	virtual TraceType_t GetTraceType() const;
};

class FilterPenetration : public ITraceFilter {
public:
	IClientEntity* m_pSelf;
	IClientEntity* m_pIgnoreFirst;
public:
	virtual ~FilterPenetration();
	FilterPenetration();
	virtual bool ShouldHitEntity(IHandleEntity* entity, int mask);
	void SetSelf(IClientEntity* self);
	void Reset();
	virtual TraceType_t GetTraceType() const;
};

extern FilterDefault* g_pFilterDefault;
extern FilterNoPlayer* g_pFilterNoPlayer;
extern FilterPenetration* g_pFilterPenetration;

}

#endif /* TRACE_H_ */
