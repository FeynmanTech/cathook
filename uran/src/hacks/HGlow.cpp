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
#include "../helpers.h"
#include "../interfaces.h"
#include "../localplayer.h"

#include "../fixsdk.h"
#include <icliententity.h>
#include <Color.h>
#include <client_class.h>
#include <icliententitylist.h>
#include <cdll_int.h>
#include <tier1/convar.h>
#include <basehandle.h>

// -------------------------------------------------------------------------------------------------- //
template< class T >
class CHandle : public CBaseHandle
{
public:

			CHandle();
			CHandle( int iEntry, int iSerialNumber );
			CHandle( const CBaseHandle &handle );
			CHandle( T *pVal );

	// The index should have come from a call to ToInt(). If it hasn't, you're in trouble.
	static CHandle<T> FromIndex( int index );

	T*		Get() const;
	void	Set( const T* pVal );

			operator T*();
			operator T*() const;

	bool	operator !() const;
	bool	operator==( T *val ) const;
	bool	operator!=( T *val ) const;
	const CBaseHandle& operator=( const T *val );

	T*		operator->() const;
};

struct GlowObjectDefinition_t
{
	CHandle<CBaseEntity> m_hEntity;
	Vector m_vGlowColor;
	float m_flGlowAlpha;

	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	int m_nSplitScreenSlot;

	// Linked list of free slots
	int m_nNextFreeSlot;
};

struct CGlowManager
{
	CUtlVector<GlowObjectDefinition_t> m_GlowObjectDefinitions;
	int RegisterGlowObject(CBaseEntity *pEntity, const Vector &vGlowColor, float flGlowAlpha, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot);
};


template<typename T>
T GetFunction(void* obj, unsigned idx) {
	const void** vtbl = *reinterpret_cast<const void***>((size_t*)obj);
	return reinterpret_cast<T>(vtbl[idx]);
}

void UpdateGlowEffect(IClientEntity* entity);
void DestroyGlowEffect(IClientEntity* entity);

//void GetHealthColor(IClientEntity* entity, Color)

void ToggleEntityGlow(IClientEntity* entity, bool glow) {
	if (!entity) return;
	if (GetEntityValue<int>(entity, eoffsets.bGlowEnabled) == glow) return;
	SetEntityValue<int>(entity, eoffsets.bGlowEnabled, (int)glow);
	if (glow) UpdateGlowEffect(entity);
	else DestroyGlowEffect(entity);
}

void UpdateGlowEffect(IClientEntity* entity) {
	typedef void (*UpdateGlowEffect_t)(IClientEntity* entity);
	GetFunction<UpdateGlowEffect_t>(entity, 0x35c / 4);
}

void DestroyGlowEffect(IClientEntity* entity) {
	typedef void (*DestroyGlowEffect_t)(IClientEntity* entity);
	GetFunction<DestroyGlowEffect_t>(entity, 0x360 / 4);
}

void SetEntityGlowColor(IClientEntity* entity, Color& color) {
	if (!GetEntityValue<int>(entity, eoffsets.bGlowEnabled)) return;

}

CGlowManager* g_pGlowManager = 0;

void CC_GlowSigSearch(const CCommand& args) {
	logging::Info("Searching for %s", args.ArgS());
	g_pGlowManager = *(CGlowManager**)(gSignatures.GetClientSignature((char*)args.ArgS()) + 8);
	if (g_pGlowManager == 0) {
		logging::Info("GlowManager sigsearch failed");
	}
	logging::Info("GlowManager: 0x%08f", g_pGlowManager);
}

void HGlow::Create() {
	cmd_scan = CreateConCommand("u_glow_scan", CC_GlowSigSearch, "Re-scans the memory for glow ptr");
	v_bEnabled = CreateConVar("u_glow_enabled", "0", "Glow enabled. Experimental.");
	v_bEnemyOnly = CreateConVar("u_glow_enemy", "1", "Only enemies will glow");
	g_pGlowManager = *(CGlowManager**)(gSignatures.GetClientSignature("A1 ? ? ? ? C7 04 24 ? ? ? ? 89 44 24 04 E8 ? ? ? ? 8B 35 ? ? ? ? 89 C7 C1 E0 05 89 45 CC 01 C6 E9") + 8);
	if (g_pGlowManager == 0) {
		logging::Info("GlowManager sigsearch failed");
	}
	logging::Info("GlowManager: 0x%08f", g_pGlowManager);
}

bool HGlow::CreateMove(void*, float, CUserCmd*) {
	if (!g_pGlowManager) return true;
	if (!v_bEnabled->GetBool()) return true;
	for (int i = 0; i < interfaces::entityList->GetHighestEntityIndex(); i++) {
		IClientEntity* entity = interfaces::entityList->GetClientEntity(i);
		if (i == interfaces::engineClient->GetLocalPlayer()) continue;
		if (!entity) continue;
		if (entity->GetClientClass()->m_ClassID != 241) continue;
		if (entity->IsDormant()) {
			ToggleEntityGlow(entity, false);
			continue;
		}
		if (GetEntityValue<char>(entity, eoffsets.iLifeState)) {
			ToggleEntityGlow(entity, false);
			continue;
		}
		if (v_bEnemyOnly->GetBool()) {
			if (GetEntityValue<int>(entity, eoffsets.iTeamNum) == g_pLocalPlayer->team) continue;
		}
		ToggleEntityGlow(entity, true);
	}
	return true;
}

void HGlow::Destroy() {

}

void HGlow::PaintTraverse(void*, unsigned int, bool, bool) {

}
