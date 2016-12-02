/*
 * TargetSystemDistance.h
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#ifndef TARGETSYSTEMDISTANCE_H_
#define TARGETSYSTEMDISTANCE_H_

#include "ITargetSystem.h"

class TargetSystemDistance : public ITargetSystem {
public:
	bool ShouldTarget(int idx);
	int GetScore(int idx);
};

#endif /* TARGETSYSTEMDISTANCE_H_ */
