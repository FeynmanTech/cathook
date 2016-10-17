/*
 * hooks.h
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#ifndef HOOKS_H_
#define HOOKS_H_

#include "sdk.h"

void CreateMove(void* client_mode, float input_sample, CUserCmd* cmd);
void PaintTraverse(void* panels, unsigned int panel, bool force, bool allow_force);



#endif /* HOOKS_H_ */
