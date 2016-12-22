/*
 * AntiAim.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: nullifiedcat
 */

#include "AntiAim.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(AntiAim);

const char* AntiAim::GetName() {
	return "ANTI-AIM";
}

const char* psza__YawMode[] = { "KEEP", "STATIC", "RANDOM", "SPIN" };
const char* psza__PitchMode[] = { "KEEP", "STATIC", "RANDOM" };


AntiAim::AntiAim() {
	this->v_bEnabled = CREATE_CV(CV_SWITCH, "aa_enabled", "0", "Enable");
	this->v_flPitch = CREATE_CV(CV_FLOAT, "aa_pitch", "-89.0", "Pitch");
	this->v_flYaw = CREATE_CV(CV_FLOAT, "aa_yaw", "0.0", "Yaw");
	this->v_flSpinSpeed = CREATE_CV(CV_FLOAT, "aa_spin", "10.0", "Spin speed");
	this->v_PitchMode = CREATE_CV(new CatEnum(psza__PitchMode, ARRAYSIZE(psza__PitchMode)), "aa_pitch_mode", "1", "Pitch mode");
	this->v_YawMode = CREATE_CV(new CatEnum(psza__YawMode, ARRAYSIZE(psza__YawMode)), "aa_yaw_mode", "3", "Yaw mode");
}

float yaw = -180;
float pitch = -89;

bool AntiAim::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (cmd->buttons & IN_USE) {
		return true;
	}
	if (cmd->buttons & IN_ATTACK) {
		if (CanShoot()) return true;
	}
	if ((cmd->buttons & IN_ATTACK2) && g_pLocalPlayer->weapon && g_pLocalPlayer->weapon->GetClientClass()->m_ClassID == ClassID::CTFLunchBox) return true;
	if (g_pLocalPlayer->bAttackLastTick) return true;
	if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee) return true;
	float p = cmd->viewangles.x;
	float y = cmd->viewangles.y;
	switch (this->v_YawMode->GetInt()) {
	case 1: // FIXED
		y = this->v_flYaw->GetFloat();
		break;
	case 2: // RANDOM
		y = RandFloatRange(-180.0f, 180.0f);
		break;
	case 3: // SPIN
		yaw += v_flSpinSpeed->GetFloat();
		if (yaw > 180) yaw = -180;
		y = yaw;
		break;
	}
	switch (this->v_PitchMode->GetInt()) {
	case 1:
		p = this->v_flPitch->GetFloat();
		break;
	case 2:
		p = RandFloatRange(-89.0f, 89.0f);
		break;
	}

	Vector angl = Vector(p, y, 0);
	fClampAngle(angl);
	//angl.z = 180;
	cmd->viewangles = angl;
	g_pLocalPlayer->bUseSilentAngles = true;
	return false;
}

void AntiAim::PaintTraverse(void*, unsigned int, bool, bool) {}
