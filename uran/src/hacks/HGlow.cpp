/*
 * HGlow.cpp
 *
 *  Created on: Oct 22, 2016
 *      Author: nullifiedcat
 */

#include "HGlow.h"

#include "../copypasted/CSignature.h"
#include "../logging.h"
#include "../entity.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <Color.h>

void ToggleEntityGlow(IClientEntity* entity, bool glow) {
	if (GetEntityValue<int>(entity, eoffsets.bGlowEnabled) == glow) return;
	SetEntityValue<int>(entity, eoffsets.bGlowEnabled, (int)glow);
	typedef void (*UpdateGlowEffect_t)(IClientEntity* entity);
	//((UpdateGlowEffect_t)((*(uintptr_t*)entity)[0x35C / 4]))(entity);
}

void SetEntityGlowColor(IClientEntity* entity, Color& color) {
	if (!GetEntityValue<int>(entity, eoffsets.bGlowEnabled)) return;

}

void HGlow::Create() {
	m_pGlowManager = **(CGlowManager***)(gSignatures.GetClientSignature("A1 ? ? ? ? C7 04 24 ? ? ? ? 89 44 24 04 E8 ? ? ? ? 8B 35 ? ? ? ? 89 C7 C1 E0 05 89 45 CC 01 C6 E9") + 8);
	if (m_pGlowManager == 0) {
		logging::Info("GlowManager sigsearch failed");
	}
	logging::Info("GlowManager: 0x%08f", m_pGlowManager);
}

bool HGlow::CreateMove(void*, float, CUserCmd*) {
	return true;
}

void HGlow::Destroy() {

}

void HGlow::PaintTraverse(void*, unsigned int, bool, bool) {

}
