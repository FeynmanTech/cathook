/*
 * conditions.cpp
 *
 *  Created on: Jan 15, 2017
 *      Author: nullifiedcat
 */

#include "conditions.h"
#include "common.h"

bool BitCheck(condition_data_s data, condition cond) {
	if (cond >= 32 * 3) {
		return data.cond_3 & (1 << (cond % 32));
	}
	if (cond >= 32 * 2) {
		return data.cond_2 & (1 << (cond % 32));
	}
	if (cond >= 32 * 1) {
		return data.cond_1 & (1 << (cond % 32));
	}
	return data.cond_0 & (1 << (cond));
}

void CondBitSet(condition_data_s data, condition cond, bool state) {
	if (state) {
		if (cond > 32 * 3) {
			data.cond_3 |= (1 << (cond % 32));
		} else	if (cond > 32 * 2) {
			data.cond_2 |= (1 << (cond % 32));
		} else if (cond > 32 * 1) {
			data.cond_1 |= (1 << (cond % 32));
		} else {
			data.cond_0 |= (1 << (cond));
		}
	} else {
		if (cond > 32 * 3) {
			data.cond_3 &= ~(1 << (cond % 32));
		} else	if (cond > 32 * 2) {
			data.cond_2 &= ~(1 << (cond % 32));
		} else if (cond > 32 * 1) {
			data.cond_1 &= ~(1 << (cond % 32));
		} else {
			data.cond_0 &= ~(1 << (cond));
		}
	}
}

void OldCondSet(CachedEntity* ent, condition cond, bool state) {
	if (state) {
		if (cond > 32 * 3) {
			ent->var<int>(netvar.iCond3) |= (1 << (cond % 32));
		} else	if (cond > 32 * 2) {
			ent->var<int>(netvar.iCond2) |= (1 << (cond % 32));
		} else if (cond > 32 * 1) {
			ent->var<int>(netvar.iCond1) |= (1 << (cond % 32));
		} else {
			ent->var<int>(netvar.iCond) |= (1 << (cond));
		}
	} else {
		if (cond > 32 * 3) {
			ent->var<int>(netvar.iCond3) &= ~(1 << (cond % 32));
		} else	if (cond > 32 * 2) {
			ent->var<int>(netvar.iCond2) &= ~(1 << (cond % 32));
		} else if (cond > 32 * 1) {
			ent->var<int>(netvar.iCond1) &= ~(1 << (cond % 32));
		} else {
			ent->var<int>(netvar.iCond) &= ~(1 << (cond));
		}
	}
}

condition_data_s FromOldNetvars(CachedEntity* ent) {
	condition_data_s result;
	result.cond_0 = ent->var<int>(netvar.iCond);
	result.cond_1 = ent->var<int>(netvar.iCond1);
	result.cond_2 = ent->var<int>(netvar.iCond2);
	result.cond_3 = ent->var<int>(netvar.iCond3);
	return result;
}

bool HasCondition(CachedEntity* ent, condition cond) {
	if (!TF) return false;
	if (TF2) return BitCheck(ent->var<condition_data_s>(netvar._condition_bits), cond) || BitCheck(FromOldNetvars(ent), cond);
	return BitCheck(FromOldNetvars(ent), cond);
}

void AddCondition(CachedEntity* ent, condition cond) {
	if (!TF) return;
	if (TF2) CondBitSet(ent->var<condition_data_s>(netvar._condition_bits), cond, true);
	OldCondSet(ent, cond, true);
}

void RemoveCondition(CachedEntity* ent, condition cond) {
	if (!TF) return;
	if (TF2) CondBitSet(ent->var<condition_data_s>(netvar._condition_bits), cond, false);
	OldCondSet(ent, cond, false);
}
