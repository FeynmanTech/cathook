/*
 * TargetSystemSmart.h
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#ifndef TARGETSYSTEMSMART_H_
#define TARGETSYSTEMSMART_H_

class TargetSystemSmart : public ITargetSystem {
public:
	bool ShouldTarget(int idx);
	int GetScore(int idx);
};

#endif /* TARGETSYSTEMSMART_H_ */
