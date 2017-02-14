/*
 * AntiDisguise.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: nullifiedcat
 */

#include "AntiDisguise.h"

#include "../common.h"
#include "../sdk.h"

namespace hacks { namespace tf2 { namespace antidisguise {

CatVar enabled(CV_SWITCH, "antidisguise", "0", "Remove spy disguise", 0, "Remove disguise from all spies");

void ProcessEntity(CachedEntity& entity) {
	if (entity.m_IDX > 32) return;
	if (entity.Type() == ENTITY_PLAYER) {
		if (entity.Class() == tf_spy) {
			RemoveCondition(&entity, TFCond_Disguised);
		}
	}
}

}}}
