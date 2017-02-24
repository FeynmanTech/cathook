/*
 * AutoHeal.h
 *
 *  Created on: Dec 3, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOHEAL_H_
#define HACKS_AUTOHEAL_H_

namespace hacks { namespace tf { namespace autoheal {

extern CatVar enabled;

bool CanHeal(CachedEntity& entity);
int HealingPriority(CachedEntity& entity);

void Reset();
void ProcessEntity(CUserCmd*, CachedEntity& entity);
void DoHealing();

}}}

#endif /* HACKS_AUTOHEAL_H_ */
