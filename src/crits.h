/*
 * crits.h
 *
 *  Created on: Feb 25, 2017
 *      Author: nullifiedcat
 */

#ifndef CRITS_H_
#define CRITS_H_

class CUserCmd;

bool RandomCrits();
bool WeaponCanCrit();
bool IsAttackACrit(CUserCmd* cmd);

#endif /* CRITS_H_ */
