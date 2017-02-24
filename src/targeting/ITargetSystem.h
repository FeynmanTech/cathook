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
	virtual int GetScore(int idx) = 0;
	virtual const char* Name() = 0;
};

#endif /* ITARGETSYSTEM_H_ */
