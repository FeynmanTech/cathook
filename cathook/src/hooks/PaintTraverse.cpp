/*
 * PaintTraverse.cpp
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#include "PaintTraverse.h"
#include "../common.h"
#include "../segvcatch/segvcatch.h"

void PaintTraverse_hook(void* thisptr, unsigned int panel, bool b1, bool b2) {
	SEGV_BEGIN;
	SEGV_END;
}

