/*
 * BeginFrame.cpp
 *
 *  Created on: Feb 10, 2017
 *      Author: nullifiedcat
 */

#include "hookedmethods.h"
#include "../common.h"
#include "../sdk.h"

void BeginFrame_hook(IStudioRender* _this) {
	((BeginFrame_t*)hooks::hkStudioRender->GetMethod(hooks::offBeginFrame))(_this);
	if (!g_Settings.bInvalid)
		interfaces::debug->AddTextOverlay(LOCAL_E->m_vecOrigin, -1.0f, "AyyLmao");

}
