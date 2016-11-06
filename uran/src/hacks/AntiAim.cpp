/*
 * AntiAim.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: nullifiedcat
 */

#include "AntiAim.h"
#include "../localplayer.h"
#include "../usercmd.h"
#include "../helpers.h"
#include "../sdk/in_buttons.h"

void AntiAim::Create() {
	this->v_bEnabled = CreateConVar("u_antiaim_enabled", "0", "Enable AntiAim");
}

bool AntiAim::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if ((cmd->buttons & (IN_ATTACK | IN_ATTACK2 | IN_USE))) return true;
	if (g_pLocalPlayer->bAttackLastTick) return true;
	Vector angl = Vector(rand() % 1000, rand() % 1000, rand() % 1000);
	fClampAngle(angl);
	cmd->viewangles = angl;
	g_pLocalPlayer->bUseSilentAngles = true;
	/*Vector vsilent(cmd->forwardmove, cmd->sidemove, cmd->upmove);
	float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);
	Vector ang;
	VectorAngles(vsilent, ang);
	float yaw = deg2rad(ang.y - g_pLocalPlayer->v_OrigViewangles.y + cmd->viewangles.y);
	cmd->forwardmove = cos(yaw) * speed;
	cmd->sidemove = sin(yaw) * speed;*/
	//FixMovement(*cmd, g_pLocalPlayer->v_OrigViewangles);
	return false;
}

void AntiAim::Destroy() {}
void AntiAim::PaintTraverse(void*, unsigned int, bool, bool) {}
