/*
 * AutoSticky.h
 *
 *  Created on: Dec 2, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOSTICKY_H_
#define HACKS_AUTOSTICKY_H_

namespace hacks { namespace tf { namespace autosticky {

extern CatVar enabled;
extern CatVar buildings;
extern CatVar distance;

bool IsATarget(CachedEntity& entity);
bool IsABomb(CachedEntity& entity);
void Reset();
void ProcessEntity(CUserCmd* cmd, CachedEntity& entity);
void CalculateAndDetonate(CUserCmd* cmd);

}}}

#endif /* HACKS_AUTOSTICKY_H_ */
