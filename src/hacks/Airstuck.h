/*
 * Airstuck.h
 *
 *  Created on: Nov 26, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_AIRSTUCK_H_
#define HACKS_AIRSTUCK_H_

namespace hacks { namespace shared { namespace airstuck {

extern CatVar stuck;
extern CatVar stuck_key;

extern bool is_stuck;

void SendNOP();
void ProcessUserCmd(CUserCmd*);
void Reset();

}}}

#endif /* HACKS_AIRSTUCK_H_ */
