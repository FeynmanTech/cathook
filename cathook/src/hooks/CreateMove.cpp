/*
 * CreateMove.cpp
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#include "CreateMove.h"

#include "../hooks.h"
#include "../hack.h"
#include "../common.h"
#include "hookedmethods.h"

bool CreateMove_hook(void* thisptr, float inputSample, CUserCmd* cmd) {
	SEGV_BEGIN;
	bool ret = ((CreateMove_t*)hooks::hkClientMode->GetMethod(hooks::offCreateMove))(thisptr, inputSample, cmd);

	if (!cmd) {
		return ret;
	}

	if (!g_Settings.bHackEnabled->GetBool()) {
		return ret;
	}

	if (!interfaces::engineClient->IsInGame()) {
		g_Settings.bInvalid = true;
		return true;
	}

//	PROF_BEGIN();

	INetChannel* ch = (INetChannel*)interfaces::engineClient->GetNetChannelInfo();
	if (ch && !hooks::IsHooked((void*)((uintptr_t)ch))) {
		hooks::hkNetChannel = new hooks::VMTHook();
		hooks::hkNetChannel->Init(ch, 0);
		hooks::hkNetChannel->HookMethod((void*)CanPacket_hook, hooks::offCanPacket);
		hooks::hkNetChannel->HookMethod((void*)SendNetMsg_hook, hooks::offSendNetMsg);
		hooks::hkNetChannel->HookMethod((void*)Shutdown_hook, hooks::offShutdown);
		hooks::hkNetChannel->Apply();
	}
	//logging::Info("canpacket: %i", ch->CanPacket());
	//if (!cmd) return ret;

	bool time_replaced = false;
	float curtime_old = interfaces::gvars->curtime;;
	if (CE_GOOD(g_pLocalPlayer->entity)) {
		float servertime = (float)CE_INT(g_pLocalPlayer->entity, netvar.nTickBase) * interfaces::gvars->interval_per_tick;
		interfaces::gvars->curtime = servertime;
		time_replaced = true;
	}
	if (g_Settings.bInvalid) {
		gEntityCache.Invalidate();
	}
//	PROF_BEGIN();
	SAFE_CALL(gEntityCache.Update());
//	PROF_END("Entity Cache updating");
	SAFE_CALL(g_pPlayerResource->Update());
	SAFE_CALL(g_pLocalPlayer->Update());
	g_Settings.bInvalid = false;
	if (CE_GOOD(g_pLocalPlayer->entity)) {
			g_pLocalPlayer->v_OrigViewangles = cmd->viewangles;
//		PROF_BEGIN();
		//RunEnginePrediction(g_pLocalPlayer->entity, cmd);
		SAFE_CALL(CREATE_MOVE(ESP));
		if (!g_pLocalPlayer->life_state) {
			if (TF2) SAFE_CALL(CREATE_MOVE(Noisemaker));
			SAFE_CALL(CREATE_MOVE(Bunnyhop));
			SAFE_CALL(CREATE_MOVE(AutoStrafe));
			SAFE_CALL(CREATE_MOVE(Aimbot));
			SAFE_CALL(CREATE_MOVE(Airstuck));
			SAFE_CALL(CREATE_MOVE(AntiAim));
			if (TF) SAFE_CALL(CREATE_MOVE(AutoSticky));
			if (TF) SAFE_CALL(CREATE_MOVE(AutoReflect));
			SAFE_CALL(CREATE_MOVE(Triggerbot));
			if (TF) SAFE_CALL(CREATE_MOVE(HuntsmanCompensation));
		}
		if (TF) SAFE_CALL(CREATE_MOVE(AntiDisguise));
		if (TF) SAFE_CALL(CREATE_MOVE(AutoHeal));
		if (TF2) SAFE_CALL(CREATE_MOVE(Glow));
		//SAFE_CALL(CREATE_MOVE(FollowBot));
		SAFE_CALL(CREATE_MOVE(Misc));
		SAFE_CALL(CREATE_MOVE(KillSay));
		SAFE_CALL(CREATE_MOVE(Spam));
//		PROF_END("Hacks processing");
		if (time_replaced) interfaces::gvars->curtime = curtime_old;
	}
	/*for (IHack* i_hack : hack::hacks) {
		if (!i_hack->CreateMove(thisptr, inputSample, cmd)) {
			ret = false;
		}
	}*/
	g_Settings.bInvalid = false;
	if (g_pChatStack)
		g_pChatStack->OnCreateMove();
	if (CE_GOOD(g_pLocalPlayer->entity)) {
		if (g_pLocalPlayer->bUseSilentAngles) {
			Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
			float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
			Vector ang;
			VectorAngles(vsilent, ang);
			float yaw = DEG2RAD(ang.y - g_pLocalPlayer->v_OrigViewangles.y + cmd->viewangles.y);
			cmd->forwardmove = cos(yaw) * speed;
			cmd->sidemove = sin(yaw) * speed;
			ret = false;
		}
	}
	if (cmd)
		g_Settings.last_angles = cmd->viewangles;

//	PROF_END("CreateMove");
	return ret;

	SEGV_END;
	return true;
}
