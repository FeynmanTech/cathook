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

	CatVar_DEPRECATED* v_bEnabled;
	CatVar_DEPRECATED* v_flWarningDistance;
	CatVar_DEPRECATED* v_flBackstabDistance;
};

DECLARE_HACK_SINGLETON(SpyAlert);

#endif /* HACKS_SPYALERT_H_ */
