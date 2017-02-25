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

float AngleDiff( float destAngle, float srcAngle )
{
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if ( destAngle > srcAngle )
	{
		if ( delta >= 180 )
			delta -= 360;
	}
	else
	{
		if ( delta <= -180 )
			delta += 360;
	}
	return delta;
}//TODO temporary

#include "../profiler.h"

bool CreateMove_hook(void* thisptr, float inputSample, CUserCmd* cmd) {
	SEGV_BEGIN;

	if (TF2C && g_phMisc->v_bMinigunJump->GetBool() && CE_GOOD(LOCAL_W)) {
		//RemoveCondition(LOCAL_E, TFCond_Slowed);
		CE_INT(LOCAL_W, netvar.iWeaponState) = 0;
		//RemoveCondition(LOCAL_E, TFCond_Taunting);
	}
	bool ret = ((CreateMove_t*)hooks::hkClientMode->GetMethod(hooks::offCreateMove))(thisptr, inputSample, cmd);

	//PROF_SECTION(CreateMove);

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
		SAFE_CALL(HACK_PROCESS_USERCMD(ESP, cmd));
		if (!g_pLocalPlayer->life_state) {
			//if (TF2) SAFE_CALL(HACK_PROCESS_USERCMD(Noisemaker, cmd));
			SAFE_CALL(HACK_PROCESS_USERCMD(Bunnyhop, cmd));
			SAFE_CALL(HACK_PROCESS_USERCMD(AutoStrafe, cmd));
			SAFE_CALL(HACK_PROCESS_USERCMD(Aimbot, cmd));
			SAFE_CALL(HACK_PROCESS_USERCMD(Airstuck, cmd));
			SAFE_CALL(HACK_PROCESS_USERCMD(AntiAim, cmd));
			if (TF) SAFE_CALL(HACK_PROCESS_USERCMD(AutoSticky, cmd));
			if (TF) SAFE_CALL(HACK_PROCESS_USERCMD(AutoReflect, cmd));
			SAFE_CALL(HACK_PROCESS_USERCMD(Triggerbot, cmd));
		}
		if (TF) SAFE_CALL(HACK_PROCESS_USERCMD(AntiDisguise, cmd));
		if (TF) SAFE_CALL(HACK_PROCESS_USERCMD(AutoHeal, cmd));
		if (TF2) SAFE_CALL(HACK_PROCESS_USERCMD(Glow, cmd));
		//SAFE_CALL(CREATE_MOVE(FollowBot));
		SAFE_CALL(HACK_PROCESS_USERCMD(Misc, cmd));
		SAFE_CALL(HACK_PROCESS_USERCMD(KillSay, cmd));
		SAFE_CALL(HACK_PROCESS_USERCMD(Spam, cmd));
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
		bool speedapplied = false;
		if (g_Settings.kRollSpeedhack->GetBool() && g_pGUI->m_bPressedState[g_Settings.kRollSpeedhack->GetInt()] && !(cmd->buttons & IN_ATTACK)) {
			float speed = cmd->forwardmove;
			if (fabs(speed) > 0.0f) {
				cmd->forwardmove = -speed;
				cmd->sidemove = 0.0f;
				cmd->viewangles.y = g_pLocalPlayer->v_OrigViewangles.y;
				cmd->viewangles.y -= 180.0f;
				if (cmd->viewangles.y < -180.0f) cmd->viewangles.y += 360.0f;
				cmd->viewangles.z = 90.0f;
				g_pLocalPlayer->bUseSilentAngles = true;
				speedapplied = true;
			}
			/*Vector vecMove( cmd->forwardmove, 0.0f, 0.0f );
			float flLength = vecMove.Length();
			if( flLength > 0.0f && !(cmd->buttons & IN_ATTACK) )
			{

				//Vector nvm = -vecMove;
				Vector angMoveReverse;
				VectorAngles( vecMove, angMoveReverse );
				cmd->forwardmove = -flLength;
				cmd->sidemove = 0.0f; // Move only backwards, no sidemove
				cmd->viewangles.y = AngleDiff( cmd->viewangles.y , angMoveReverse.y ) ;
				logging::Info("yaw %.2f", cmd->viewangles.y);
				cmd->viewangles.y += 180.0f;
				if (cmd->viewangles.y > 180.0f) cmd->viewangles.y -= 360.0f;
				cmd->viewangles.z = 89.0f; // OMFG SUPER 1337 SPEEDHAQ METHODS 8)
				g_pLocalPlayer->bUseSilentAngles = true;
			}*/
		}

		if (g_pLocalPlayer->bUseSilentAngles) {
			if (!speedapplied) {
				Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
				float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
				Vector ang;
				VectorAngles(vsilent, ang);
				float yaw = DEG2RAD(ang.y - g_pLocalPlayer->v_OrigViewangles.y + cmd->viewangles.y);
				cmd->forwardmove = cos(yaw) * speed;
				cmd->sidemove = sin(yaw) * speed;
			}

			ret = false;
		}
		if (cmd)
			g_Settings.last_angles = cmd->viewangles;
	}


//	PROF_END("CreateMove");
	g_pLocalPlayer->bAttackLastTick = (cmd->buttons & IN_ATTACK);
	return ret;

	SEGV_END;
	return true;
}
