/*
 * others.cpp
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#include "hooks.h"
#include "../common.h"
#include "../netmessage.h"

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
	if (g_Settings.bHackEnabled->GetBool()) {
		if (CE_GOOD(g_pLocalPlayer->entity) && CE_GOOD(g_pLocalPlayer->weapon())) {
			int defidx = CE_INT(g_pLocalPlayer->weapon(), netvar.iItemDefinitionIndex);
			if (defidx == 61) {
				CE_INT(g_pLocalPlayer->weapon(), netvar.iItemDefinitionIndex) = 1006;
			}
		}
		if (g_Settings.bThirdperson->GetBool() && g_pLocalPlayer->entity) {
			CE_INT(g_pLocalPlayer->entity, netvar.nForceTauntCam) = 1;
		}
		if (stage == 5 && g_Settings.bShowAntiAim->GetBool() && interfaces::iinput->CAM_IsThirdPerson()) {
			if (CE_GOOD(g_pLocalPlayer->entity)) {
				CE_FLOAT(g_pLocalPlayer->entity, netvar.deadflag + 4) = last_angles.x;
				CE_FLOAT(g_pLocalPlayer->entity, netvar.deadflag + 8) = last_angles.y;
			}
		}
	}
	((FrameStageNotify_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify))(thisptr, stage);
	if (g_Settings.bHackEnabled->GetBool()) {
		if (stage == 5 && g_Settings.bNoFlinch->GetBool()) {
			static Vector oldPunchAngles = Vector();
			Vector punchAngles = CE_VECTOR(g_pLocalPlayer->entity, netvar.vecPunchAngle);
			QAngle viewAngles;
			interfaces::engineClient->GetViewAngles(viewAngles);
			viewAngles -= VectorToQAngle(punchAngles - oldPunchAngles);
			oldPunchAngles = punchAngles;
			interfaces::engineClient->SetViewAngles(viewAngles);
		}

		if (g_Settings.bNoZoom->GetBool()) {
			if (CE_GOOD(g_pLocalPlayer->entity)) {
				//g_pLocalPlayer->bWasZoomed = NET_INT(g_pLocalPlayer->entity, netvar.iCond) & cond::zoomed;
				CE_INT(g_pLocalPlayer->entity, netvar.iCond) = CE_INT(g_pLocalPlayer->entity, netvar.iCond) &~ cond::zoomed;
			}
		}
	}
	SEGV_END;
}

void OverrideView_hook(void* thisptr, CViewSetup* setup) {
	SEGV_BEGIN;
	((OverrideView_t*)hooks::hkClientMode->GetMethod(hooks::offOverrideView))(thisptr, setup);
	if (!g_Settings.bHackEnabled->GetBool()) return;
	if (g_Settings.flForceFOV && g_Settings.flForceFOV->GetBool()) {
		setup->fov = g_Settings.flForceFOV->GetFloat();
	}
	SEGV_END;
}

bool DispatchUserMessage_hook(void* thisptr, int type, bf_read& buf) {
	return ((DispatchUserMessage_t*)hooks::hkClient->GetMethod(hooks::offFrameStageNotify + 1))(thisptr, type, buf);
}
