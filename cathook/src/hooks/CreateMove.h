/*
 * CreateMove.h
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#ifndef CREATEMOVE_H_
#define CREATEMOVE_H_

class CUserCmd;

typedef bool(CreateMove_t)(void*, float, CUserCmd*);
bool CreateMove_hook(void*, float, CUserCmd*);

#endif /* CREATEMOVE_H_ */
