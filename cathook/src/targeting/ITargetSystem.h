/*
 * ITargetSystem.h
 *
 *  Created on: Nov 30, 2016
 *      Author: nullifiedcat
 */

#ifndef ITARGETSYSTEM_H_
#define ITARGETSYSTEM_H_

// TODO Targeting.

class ITargetSystem {
public:
	virtual ~ITargetSystem();
	virtual bool ShouldTarget(int idx) = 0;
	virtual int GetScore(int idx) = 0;
};

class ConVar;
struct TargetingConVars {
	ConVar* bTargetInvisible;
	ConVar* flMaxFOV;
	ConVar* iMaxDistance;
};

extern TargetingConVars* g_pTargetingConVars;

void InitTargetingConVars();

#endif /* ITARGETSYSTEM_H_ */
