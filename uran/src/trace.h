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

}

#endif /* TRACE_H_ */
