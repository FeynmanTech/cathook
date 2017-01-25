/*
 * others.cpp
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#include "../common.h"
#include "../netmessage.h"
#include "../gui/gui.h"
#include "../hack.h"
#include "../hwid.h"
#include "hookedmethods.h"

bool CanPacket_hook(void* thisptr) {
	SEGV_BEGIN;
	return g_Settings.bSendPackets->GetBool() && ((CanPacket_t*)hooks::hkNetChannel->GetMethod(hooks::offCanPacket))(thisptr);
	SEGV_END;
	return false;
}

int IN_KeyEvent_hook(void* thisptr, int eventcode, int keynum, const char* pszCurrentBinding) {
	SEGV_BEGIN;
	if (eventcode == 1) {
		if (g_pGUI->KeyEvent((ButtonCode_t)keynum)) return 1;
	}
	return ((IN_KeyEvent_t*)hooks::hkClient->GetMethod(hooks::offKeyEvent))(thisptr, eventcode, keynum, pszCurrentBinding);
	SEGV_END;
	return 0;
}

bool SendNetMsg_hook(void* thisptr, INetMessage& msg, bool bForceReliable = false, bool bVoice = false) {
	SEGV_BEGIN;

	//logging::Info("Sending NetMsg! %i", msg.GetType());
	if (g_phAirstuck->v_bStuck->GetBool() && g_Settings.bHackEnabled->GetBool()) {
		switch (msg.GetType()) {
		case net_NOP:
		case net_SignonState:
		case net_StringCmd:
			break;
		default:
			return false;
		}
	}
	return ((SendNetMsg_t*)hooks::hkNetChannel->GetMethod(hooks::offSendNetMsg))(thisptr, msg, bForceReliable, bVoice);
	SEGV_END;
	return false;
}

void Shutdown_hook(void* thisptr, const char* reason) {
	SEGV_BEGIN;
	if (g_Settings.bHackEnabled->GetBool()) {
		const char* new_reason = reason;
		if (g_Settings.sDisconnectMsg->m_StringLength > 3) {
			new_reason = g_Settings.sDisconnectMsg->GetString();
		}
		((Shutdown_t*)hooks::hkNetChannel->GetMethod(hooks::offShutdown))(thisptr, new_reason);
	} else {
		((Shutdown_t*)hooks::hkNetChannel->GetMethod(hooks::offShutdown))(thisptr, reason);
	}
	SEGV_END;
}

void FrameStageNotify_hook(void* thisptr, int stage) {
	SEGV_BEGIN;
	//logging::Info("FrameStageNotify %i", stage);
	// Ambassador to festive ambassador changer. simple.
	if (!interfaces::engineClient->IsInGame()) g_Settings.bInvalid = true;
	//logging::Info("fsi begin");// TODO dbg
	SVDBG("FSN %i", __LINE__);
	// TODO hack FSN hook
	if (g_Settings.bHackEnabled->GetBool() && !g_Settings.bInvalid && stage == FRAME_RENDER_START) {
		/*SEGV_BEGIN
			for (int i = 0; i < g_GlowObjectManager->m_GlowObjectDefinitions.Size(); i++) {
			GlowObjectDefinition_t& obj = g_GlowObjectManager->m_GlowObjectDefinitions.Element(i);
			if (obj.m_nNextFreeSlot != ENTRY_IN_USE) continue;
			CachedEntity* ent;
			SAFE_CALL(ent = ENTITY(interfaces::entityList->GetClientEntityFromHandle(obj.m_hEntity)->entindex()));
			//logging::Info("Glowing Entity %i <%s> LIFE_STATE %i DORMANT %i", ent->m_IDX, ent->m_pEntity->GetClientClass()->GetName(), CE_BYTE(ent, netvar.iLifeState), ent->m_pEntity->IsDormant());
			if (CE_BAD(ent)) continue;
			//logging::Info("adding glow to %i", ent->m_IDX);
			//obj.m_vGlowColor = Vector(0.0f, 1.0f, 0.0f);
			switch (ent->m_Type) {
			case ENTITY_PLAYER:
			case ENTITY_BUILDING:
				if (!ent->m_bEnemy) continue;
				Color c = colors::Health(ent->m_iHealth, ent->m_iMaxHealth);
				//logging::Info("Adding color");
				SAFE_CALL(obj.m_vGlowColor = Vector((float)c.r() / 255.0f, (float)c.g() / 255.0f, (float)c.b() / 255.0f));
				//logging::Info("Color added");
			}
		}
		SEGV_END_INFO("Glow");*/

		/*if (CE_GOOD(g_pLocalPlayer->entity) && CE_GOOD(g_pLocalPlayer->weapon())) {
			int defidx = CE_INT(g_pLocalPlayer->weapon(), netvar.iItemDefinitionIndex);
			if (defidx == 61) {
				CE_INT(g_pLocalPlayer->weapon(), netvar.iItemDefinitionIndex) = 1006;
			}
		}*/
		SVDBG("FSN %i", __LINE__);
		if (g_Settings.bThirdperson->GetBool() && !g_pLocalPlayer->life_state && CE_GOOD(g_pLocalPlayer->entity)) {
			SVDBG("FSN %i", __LINE__);
			CE_INT(g_pLocalPlayer->entity, netvar.nForceTauntCam) = 1;
		}
		SVDBG("FSN %i", __LINE__);
		if (stage == 5 && g_Settings.bShowAntiAim->GetBool() && interfaces::iinput->CAM_IsThirdPerson()) {
			SVDBG("FSN %i", __LINE__);
			if (CE_GOOD(g_pLocalPlayer->entity)) {
				CE_FLOAT(g_pLocalPlayer->entity, netvar.deadflag + 4) = g_Settings.last_angles.x;
				CE_FLOAT(g_pLocalPlayer->entity, netvar.deadflag + 8) = g_Settings.last_angles.y;
			}
			SVDBG("FSN %i", __LINE__);
		}
		/*if (g_Settings.bNoZoom->GetBool()) {
			SVDBG("FSN %i GOOD?", __LINE__);
			SVDBG("GOOD %i", CE_GOOD(LOCAL_E));
			if (CE_GOOD(g_pLocalPlayer->entity)) {
				SVDBG("FSN %i", __LINE__);
				RemoveCondition(g_pLocalPlayer->entity, condition::TFCond_Zoomed);
			}
		}*/
	}
	SVDBG("FSN %i", __LINE__);
	SAFE_CALL(((FrameStageNotify_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify))(thisptr, stage));
	SVDBG("FSN %i", __LINE__);
	//if (g_Settings.bHackEnabled->GetBool() && !g_Settings.bInvalid && stage == FRAME_RENDER_START) {
		/*if (stage == 5 && g_Settings.bNoFlinch->GetBool()) {
			static Vector oldPunchAngles = Vector();
			Vector punchAngles = CE_VECTOR(g_pLocalPlayer->entity, netvar.vecPunchAngle);
			QAngle viewAngles;
			interfaces::engineClient->GetViewAngles(viewAngles);
			viewAngles -= VectorToQAngle(punchAngles - oldPunchAngles);
			oldPunchAngles = punchAngles;
			interfaces::engineClient->SetViewAngles(viewAngles);
		}*/
		SVDBG("FSN %i", __LINE__);
		/*if (g_Settings.bNoZoom->GetBool()) {
			SVDBG("FSN %i GOOD?", __LINE__);
			SVDBG("GOOD %i", CE_GOOD(LOCAL_E));
			if (CE_GOOD(g_pLocalPlayer->entity)) {
				SVDBG("FSN %i", __LINE__);
				RemoveCondition(g_pLocalPlayer->entity, condition::TFCond_Zoomed);
			}
		}*/
		SVDBG("FSN %i", __LINE__);
	//}
	SVDBG("FSN %i", __LINE__);
	//logging::Info("fsi end");// TODO dbg
	SEGV_END;
}

void OverrideView_hook(void* thisptr, CViewSetup* setup) {
	SEGV_BEGIN;
	((OverrideView_t*)hooks::hkClientMode->GetMethod(hooks::offOverrideView))(thisptr, setup);
	if (!g_Settings.bHackEnabled->GetBool()) return;
	if (g_Settings.flForceFOV && g_Settings.flForceFOVZoomed && g_Settings.bZoomedFOV) {
		bool zoomed = g_pLocalPlayer->bZoomed;
		if (g_Settings.bZoomedFOV->GetBool() && zoomed) {
			if (g_Settings.flForceFOVZoomed->GetBool()) {
				setup->fov = g_Settings.flForceFOVZoomed->GetFloat();
			}
		} else {
			if (g_Settings.flForceFOV->GetBool()) {
		setup->fov = g_Settings.flForceFOV->GetFloat();
			}
		}
	}
	SEGV_END;
}

bool DispatchUserMessage_hook(void* thisptr, int type, bf_read& buf) {
	SEGV_BEGIN;
	if (type == 4) {
		int s = buf.GetNumBytesLeft();
		char* data = new char[s];
		for (int i = 0; i < s; i++)
			data[i] = buf.ReadByte();
		int j = 0;
		for (int i = 0; i < 3; i++) {
			while (char c = data[j++]) {
				if (c == '\n') data[j - 1] = ' ';
			}
		}
		buf = bf_read(data, s);
		buf.Seek(0);
	}
	return ((DispatchUserMessage_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify + 1))(thisptr, type, buf);
	SEGV_END; return false;
}

void LevelInit_hook(void* thisptr, const char* newmap) {
	((LevelInit_t*) hooks::hkClientMode->GetMethod(hooks::offLevelInit))(thisptr, newmap);
	DRM_ENFORCE;
	logging::Info("LevelInit %s", newmap);
	LEVEL_INIT(Aimbot, newmap);
	LEVEL_INIT(Airstuck, newmap);
	LEVEL_INIT(AntiAim, newmap);
	LEVEL_INIT(AntiDisguise, newmap);
	LEVEL_INIT(AutoHeal, newmap);
	LEVEL_INIT(AutoReflect, newmap);
	LEVEL_INIT(AutoSticky, newmap);
	LEVEL_INIT(AutoStrafe, newmap);
	LEVEL_INIT(Bunnyhop, newmap);
	LEVEL_INIT(ESP, newmap);
//	LEVEL_SHUTDOWN(FollowBot);
	LEVEL_INIT(Misc, newmap);
	LEVEL_INIT(SpyAlert, newmap);
	LEVEL_INIT(Triggerbot, newmap);
	LEVEL_INIT(Glow, newmap);
	g_pChatStack->Reset();
}

void LevelShutdown_hook(void* thisptr) {
	((LevelShutdown_t*) hooks::hkClientMode->GetMethod(hooks::offLevelShutdown))(thisptr);
	g_Settings.bInvalid = true;
	logging::Info("LevelShutdown");
	LEVEL_SHUTDOWN(Aimbot);
	LEVEL_SHUTDOWN(Airstuck);
	LEVEL_SHUTDOWN(AntiAim);
	LEVEL_SHUTDOWN(AntiDisguise);
	LEVEL_SHUTDOWN(AutoHeal);
	LEVEL_SHUTDOWN(AutoReflect);
	LEVEL_SHUTDOWN(AutoSticky);
	LEVEL_SHUTDOWN(AutoStrafe);
	LEVEL_SHUTDOWN(Bunnyhop);
	LEVEL_SHUTDOWN(ESP);
//	LEVEL_SHUTDOWN(FollowBot);
	LEVEL_SHUTDOWN(Misc);
	LEVEL_SHUTDOWN(SpyAlert);
	LEVEL_SHUTDOWN(Triggerbot);
	LEVEL_SHUTDOWN(Glow);
	g_pChatStack->Reset();
}

