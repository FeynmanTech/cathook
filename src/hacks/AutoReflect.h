/*
 * AutoReflect.h
 *
 *  Created on: Nov 18, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AUTOREFLECT_H_
#define HACKS_AUTOREFLECT_H_

namespace hacks { namespace tf { namespace autoreflect {

extern CatVar enabled;
extern CatVar distance;
extern CatVar idle_only;
extern CatVar reflect_stickies;

extern float closest_distance;
extern CachedEntity* closest_entity;

void Reset();
void DoReflects(CUserCmd*);
void ProcessEntity(CUserCmd*, CachedEntity&);

bool ShouldReflectEntity(CachedEntity&);
bool ShouldReflect(CUserCmd* cmd);

}}}

#endif /* HACKS_AUTOREFLECT_H_ */
