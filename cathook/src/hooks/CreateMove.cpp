/*
 * CreateMove.cpp
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#include "CreateMove.h"

#include "../fixsdk.h"
#include "../common.h"
#include "./others.h"
#include "hooks.h"
#include "../profiler.h"

bool CreateMove_hook(void* thisptr, float inputSample, CUserCmd* cmd) {
	SEGV_BEGIN;
	SEGV_END;
	return true;
}
