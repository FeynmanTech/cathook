/*
 * conditions.cpp
 *
 *  Created on: Jan 15, 2017
 *      Author: nullifiedcat
 */

#include "conditions.h"
#include "common.h"

bool BitCheck(condition_data_s data, condition cond) {
	if (cond > 32 * 3) {
		return data.cond_3 & (1 << (cond % 32));
	}
	if (cond > 32 * 2) {
		return data.cond_2 & (1 << (cond % 32));
	}
	if (cond > 32 * 1) {
		return data.cond_1 & (1 << (cond % 32));
	}
	return data.cond_1 & (1 << (cond));
}

bool HasCondition(CachedEntity* ent, condition cond) {
	return BitCheck(CE_VAR(ent, netvar._condition_bits, condition_data_s), cond);
}
