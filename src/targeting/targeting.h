/*
 * targeting.h
 *
 *  Created on: Feb 18, 2017
 *      Author: nullifiedcat
 */

#ifndef TARGETING_H_
#define TARGETING_H_

class CachedEntity;

namespace targeting {

extern CatVar teammates;
extern CatVar buildings;
extern CatVar friendly_buildings;
extern CatVar stickybombs;
extern CatVar taunting;
extern CatVar invisible;

bool ShouldTarget(CachedEntity& entity);

}

#endif /* TARGETING_H_ */
