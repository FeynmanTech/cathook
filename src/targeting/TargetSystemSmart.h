/*
 * TargetSystemSmart.h
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#ifndef TARGETSYSTEMSMART_H_
#define TARGETSYSTEMSMART_H_

#include "ITargetSystem.h"

class ConVar;

class TargetSystemSmart : public ITargetSystem {
public:
	virtual int GetScore(int idx);
	inline virtual const char* Name() { return "SMART"; };
};

#endif /* TARGETSYSTEMSMART_H_ */
