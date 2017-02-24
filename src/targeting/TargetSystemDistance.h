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
	virtual int GetScore(int idx);
	inline virtual const char* Name() { return "CLOSEST ENEMY"; };
};

#endif /* TARGETSYSTEMDISTANCE_H_ */
