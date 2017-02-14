/*
 * SpyAlert.cpp
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#include "SpyAlert.h"

#include "../common.h"
#include "../sdk.h"

namespace hacks { namespace tf { namespace spyalert {

CatVar enabled(CV_SWITCH, "spyalert_enabled", "0", "Show spy warnings", NULL, "Displays warnings about nearby enemy spies");
CatVar warning_distance(CV_FLOAT, "spyalert_warning", "500.0", "Warning distance", NULL, "Yellow warning distance");
CatVar backstab_distance(CV_FLOAT, "spyalert_backstab", "200.0", "Backstab distance", NULL, "Red warning distance");

void DrawEntity(CachedEntity& entity) {
	if (!enabled) return;
	if (entity.clazz != g_pClassID->CTFPlayer) return;
	if (entity.var<unsigned char>(netvar.iLifeState)) return;
	if (entity.Class() != tf_class::tf_spy) return;
	if (!entity.Enemy()) return;
	float distance = entity.Distance();
	if (distance < this->v_flBackstabDistance->GetFloat()) {
		AddCenterString(colors::red, format("Backstab warning! ", (int)(distance / 64 * 1.22f), 'm'));
	} else if (distance < this->v_flWarningDistance->GetFloat()) {
		AddCenterString(colors::yellow, format("Incoming spy! ", (int)(distance / 64 * 1.22f), 'm'));
	}
}

}}}
