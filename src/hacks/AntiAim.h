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

	void AddSafeTicks(int ticks);
	int m_iSafeTicks;

	CatVar_DEPRECATED* v_bEnabled;
	CatVar_DEPRECATED* v_flSpinSpeed;
	CatVar_DEPRECATED* v_flYaw;
	CatVar_DEPRECATED* v_flPitch;
	CatVar_DEPRECATED* v_PitchMode;
	CatVar_DEPRECATED* v_YawMode;
	CatVar_DEPRECATED* v_bNoClamping;
	CatVar_DEPRECATED* v_flRoll;
};

DECLARE_HACK_SINGLETON(AntiAim);

#endif /* ANTIAIM_H_ */
