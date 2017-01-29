/*
 * CSlider.h
 *
 *  Created on: Jan 27, 2017
 *      Author: nullifiedcat
 */

#ifndef CSLIDER_H_
#define CSLIDER_H_

#include "CBaseWidget.h"

class CSlider;

typedef void(CSliderCallback_t)(CSlider*, float, float);

class CSlider : public CBaseWidget {
public:
	CSlider(IWidget* parent, const char* name);

	void Setup(float min, float max);
	void SetValue(float value);
	void SetCallback(CSliderCallback_t* callback);

	virtual void Update();
	virtual void Draw();

	CSliderCallback_t* m_pCallback;

	int m_nLastX;
	bool m_bDragInit;
	int m_nSliderPos;

	float m_fValue;
	float m_fValueMax;
	float m_fValueMin;

};


#endif /* CSLIDER_H_ */
