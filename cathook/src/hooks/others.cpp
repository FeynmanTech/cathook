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
	DRM_ENFORCE;
	//logging::Info("FrameStageNotify %i", stage);
	// Ambassador to festive ambassador changer. simple.
	if (!interfaces::engineClient->IsInGame()) g_Settings.bInvalid = true;
	//logging::Info("fsi begin");// TODO dbg
	if (g_Settings.bHackEnabled->GetBool() && !g_Settings.bInvalid) {
		/*if (CE_GOOD(g_pLocalPlayer->entity) && CE_GOOD(g_pLocalPlayer->weapon())) {
			int defidx = CE_INT(g_pLocalPlayer->weapon(), netvar.iItemDefinitionIndex);
			if (defidx == 61) {
				CE_INT(g_pLocalPlayer->weapon(), netvar.iItemDefinitionIndex) = 1006;
			}
		}*/
		if (g_Settings.bThirdperson->GetBool() && !g_pLocalPlayer->life_state && CE_GOOD(g_pLocalPlayer->entity)) {
			CE_INT(g_pLocalPlayer->entity, netvar.nForceTauntCam) = 1;
		}
		if (stage == 5 && g_Settings.bShowAntiAim->GetBool() && interfaces::iinput->CAM_IsThirdPerson()) {
			if (CE_GOOD(g_pLocalPlayer->entity)) {
				CE_FLOAT(g_pLocalPlayer->entity, netvar.deadflag + 4) = g_Settings.last_angles.x;
				CE_FLOAT(g_pLocalPlayer->entity, netvar.deadflag + 8) = g_Settings.last_angles.y;
			}
		}
	}
	((FrameStageNotify_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify))(thisptr, stage);
	if (g_Settings.bHackEnabled->GetBool() && !g_Settings.bInvalid) {
		/*if (stage == 5 && g_Settings.bNoFlinch->GetBool()) {
			static Vector oldPunchAngles = Vector();
			Vector punchAngles = CE_VECTOR(g_pLocalPlayer->entity, netvar.vecPunchAngle);
			QAngle viewAngles;
			interfaces::engineClient->GetViewAngles(viewAngles);
			viewAngles -= VectorToQAngle(punchAngles - oldPunchAngles);
			oldPunchAngles = punchAngles;
			interfaces::engineClient->SetViewAngles(viewAngles);
		}*/

		if (g_Settings.bNoZoom->GetBool()) {
			if (CE_GOOD(g_pLocalPlayer->entity)) {
				RemoveCondition(g_pLocalPlayer->entity, condition::TFCond_Zoomed);
			}
		}
	}
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
	return ((DispatchUserMessage_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify + 1))(thisptr, type, buf);
}

void LevelInit_hook(void* thisptr, const char* newmap) {
	((LevelInit_t*) hooks::hkClientMode->GetMethod(hooks::offLevelInit))(thisptr, newmap);
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


}

