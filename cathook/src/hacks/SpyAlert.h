/*
 * SpyAlert.h
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_SPYALERT_H_
#define HACKS_SPYALERT_H_

namespace hacks { namespace tf { namespace spyalert {

extern CatVar enabled;
extern CatVar warning_distance;
extern CatVar backstab_distance;

void DrawEntity(CachedEntity& entity);

}}}

#endif /* HACKS_SPYALERT_H_ */
