/*
 * TargetSystemFOV.h
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#ifndef TARGETSYSTEMFOV_H_
#define TARGETSYSTEMFOV_H_

#include "ITargetSystem.h"

class TargetSystemFOV : public ITargetSystem {
public:
	virtual int GetScore(int idx);
	inline virtual const char* Name() { return "FOV"; };
};

#endif /* TARGETSYSTEMFOV_H_ */
