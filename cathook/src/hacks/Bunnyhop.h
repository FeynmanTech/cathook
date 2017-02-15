/*
 * HBunnyhop.h
 *
 *  Created on: Oct 6, 2016
 *      Author: nullifiedcat
 */

#ifndef HBUNNYHOP_H_
#define HBUNNYHOP_H_

namespace hacks { namespace shared { namespace bunnyhop {

extern CatVar enabled;
extern CatVar perfect_jumps;
extern CatVar imperfect;
extern CatVar autojump;
extern CatVar autojump_speed;
extern bool jumping;

void ProcessUserCmd(CUserCmd*);

}}}

#endif /* HBUNNYHOP_H_ */
