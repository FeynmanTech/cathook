/*
 * crits.h
 *
 *  Created on: Feb 25, 2017
 *      Author: nullifiedcat
 */

#ifndef CRITS_H_
#define CRITS_H_

class CUserCmd;
class IClientEntity;

bool RandomCrits();
bool WeaponCanCrit();
bool IsAttackACrit(CUserCmd* cmd);

//bool CalcIsAttackCritical(IClientEntity* weapon);


#endif /* CRITS_H_ */
