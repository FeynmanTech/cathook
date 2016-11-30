/*
 * ITargetSystem.h
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#ifndef ITARGETSYSTEM_H_
#define ITARGETSYSTEM_H_

class ITargetSystem {
public:
	virtual ~ITargetSystem();
	virtual bool ShouldTarget(int idx) = 0;
	virtual int GetScore(int idx) = 0;
};

#endif /* ITARGETSYSTEM_H_ */
