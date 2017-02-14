/*
 * HTrigger.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HTRIGGER_H_
#define HTRIGGER_H_

namespace hacks { namespace shared { namespace triggerbot {

extern CatVar enabled;
extern CatVar hitbox;
extern CatVar respect_cloak;
extern CatVar zoomed_only;
extern CatVar bodyshot;
extern CatVar finishing;
extern CatVar range;
extern CatVar buildings;
extern CatVar vaccinator;
extern CatVar ambassador;
extern CatVar accuracy;

void ProcessUserCmd(CUserCmd* cmd);

}

#endif /* HTRIGGER_H_ */
