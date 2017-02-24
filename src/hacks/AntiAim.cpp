/*
 * AntiAim.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: nullifiedcat
 */

#include "AntiAim.h"

#include "../common.h"
#include "../sdk.h"

namespace hacks { namespace shared { namespace antiaim {

CatVar enabled(CV_SWITCH, "aa_enabled", "0", "Anti-Aim", NULL, "Master AntiAim switch");
CatVar yaw(CV_FLOAT, "aa_yaw", "0.0", "Yaw", NULL, "Static yaw (left/right)", true, 360.0);
CatVar pitch(CV_FLOAT, "aa_pitch", "-89.0", "Pitch", NULL, "Static pitch (up/down)", true, 89.0, -89.0);
CatVar yaw_mode(CV_ENUM, "aa_yaw_mode", "3", "Yaw mode", new CatEnum({ "KEEP", "STATIC", "RANDOM", "SPIN" }), "Yaw mode");
CatVar pitch_mode(CV_ENUM, "aa_pitch_mode", "1", "Pitch mode", new CatEnum({ "KEEP", "STATIC", "RANDOM" }), "Pitch mode");
CatVar roll(CV_FLOAT, "aa_roll", "0", "Roll", NULL, "Roll angle (viewangles.z)", true, -180, 180);
CatVar no_clamping(CV_SWITCH, "aa_no_clamp", "0", "Don't clamp angles", NULL, "Use this with STATIC mode for unclamped manual angles");
CatVar spin(CV_FLOAT, "aa_spin", "10.0", "Spin speed", NULL, "Spin speed (degrees/second)");
CatVar lisp(CV_SWITCH, "aa_lisp", "0", "Lisp angles", NULL, "Big numbers");

int safe_space = 0;

void SetSafeSpace(int safespace) {
	if (safespace > safe_space) safe_space = safespace;
}

bool ShouldAA(CUserCmd* cmd) {
	if (!enabled) return false;
	if (cmd->buttons & IN_USE) return false;
	if (cmd->buttons & IN_ATTACK) {
		if (!(TF2 && g_LocalPlayer.weapon()->clazz == g_pClassID->CTFCompoundBow)) {
			if (CanShoot()) return false;
		}
	}
	if ((cmd->buttons & IN_ATTACK2) && g_LocalPlayer.weapon()->clazz == g_pClassID->CTFLunchBox) return false;
	switch (GetWeaponMode(g_LocalPlayer.entity)) {
	case weapon_projectile:
		if (g_LocalPlayer.weapon()->clazz == g_pClassID->CTFCompoundBow) {
			if (!(cmd->buttons & IN_ATTACK)) {
				if (g_LocalPlayer.bAttackLastTick) SetSafeSpace(4);
			}
			break;
		}
		/* no break */
	case weapon_melee:
	case weapon_throwable:
		if ((cmd->buttons & (IN_ATTACK | IN_ATTACK2)) || g_LocalPlayer.bAttackLastTick) {
			SetSafeSpace(4);
			return false;
		}
	}
	if (safe_space) {
		safe_space--;
		if (safe_space < 0) safe_space = 0;
		return false;
	}
	return true;
}

void ProcessUserCmd(CUserCmd* cmd) {
	if (!ShouldAA(cmd)) return;
	float& p = cmd->viewangles.x;
	float& y = cmd->viewangles.y;
	switch (yaw_mode) {
	case 1: // FIXED
		y = yaw;
		break;
	case 2: // RANDOM
		y = RandFloatRange(-180.0f, 180.0f);
		break;
	case 3: // SPIN
		yaw += spin;
		if (yaw > 180) yaw = -180;
		y = yaw;
		break;
	}
	switch (pitch_mode) {
	case 1:
		p = pitch;
		break;
	case 2:
		p = RandFloatRange(-89.0f, 89.0f);
		break;
	}

	if (!no_clamping) fClampAngle(angl);
	if (roll) cmd->viewangles.z = roll;
	g_LocalPlayer.bUseSilentAngles = true;
}

}}}
