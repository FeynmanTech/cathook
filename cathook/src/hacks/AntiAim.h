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
	AntiAim();

	virtual void ProcessUserCmd(CUserCmd*) override;

	CatVar* v_bEnabled;
	CatVar* v_flSpinSpeed;
	CatVar* v_flYaw;
	CatVar* v_flPitch;
	CatVar* v_PitchMode;
	CatVar* v_YawMode;
	CatVar* v_bNoClamping;
	CatVar* v_flRoll;
};

DECLARE_HACK_SINGLETON(AntiAim);

#endif /* ANTIAIM_H_ */
