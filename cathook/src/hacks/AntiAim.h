/*
 * AntiAim.h
 *
 *  Created on: Oct 26, 2016
 *      Author: nullifiedcat
 */

#ifndef ANTIAIM_H_
#define ANTIAIM_H_

#include "IHack.h"

class AntiAim : public IHack {
public:
	DECLARE_HACK_METHODS();
	AntiAim();
	ConVar* v_bEnabled;
	ConVar* v_flSpinSpeed;
	ConVar* v_flYaw;
	ConVar* v_flPitch;
	ConVar* v_PitchMode;
	ConVar* v_YawMode;
};

DECLARE_HACK_SINGLETON(AntiAim);

#endif /* ANTIAIM_H_ */
