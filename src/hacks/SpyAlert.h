/*
 * SpyAlert.h
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#ifndef HACKS_SPYALERT_H_
#define HACKS_SPYALERT_H_

#include "IHack.h"

class SpyAlert : public IHack {
public:
	SpyAlert();

	virtual void Draw() override;

	CatVar* v_bEnabled;
	CatVar* v_flWarningDistance;
	CatVar* v_flBackstabDistance;
};

DECLARE_HACK_SINGLETON(SpyAlert);

#endif /* HACKS_SPYALERT_H_ */
