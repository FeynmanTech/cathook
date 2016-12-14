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
	bool ShouldTarget(int idx);
	int GetScore(int idx);
};

#endif /* TARGETSYSTEMFOV_H_ */
