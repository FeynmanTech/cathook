/*
 * CSlider.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: nullifiedcat
 */

#include "CSlider.h"
#include "../common.h"
#include "../sdk.h"

CSlider::CSlider(IWidget* parent, const char* name) : CBaseWidget(parent, name) {
	m_fValueMax = 1.0f;
	m_fValueMin = 0.0f;
	m_fValue = 0.5f;
	m_nSizeX = 80;
	m_nSizeY = 16;
	m_nLastX = 0;
	m_nSliderPos = 40;
	m_bDragInit = false;
	m_pCallback = 0;
}

void CSlider::Setup(float min, float max) {
	m_fValueMax = max;
	m_fValueMin = min;
	m_fValue = (max + min) / 2.0f;
}

void CSlider::SetCallback(CSliderCallback_t* callback) {
	m_pCallback = callback;
}

void CSlider::SetValue(float value) {
	m_fValue = value;
}

void CSlider::Update() {
	if (m_bMousePressed) {
		if (m_bDragInit) {
			int delta = m_nLastX - g_pGUI->m_iMouseX;
			if (delta) {
				int ax, ay;
				GetAbsolutePosition(ax, ay);
				int mv = g_pGUI->m_iMouseX - ax;
				if (mv < 0) mv = 0;
				if (mv > m_nSizeX) mv = m_nSizeX;
				float newval = ((float)mv / (float)m_nSizeX) * (m_fValueMax - m_fValueMin);
				if (m_pCallback && newval != m_fValue)
					m_pCallback(this, newval, m_fValue);
				m_fValue = newval;
				m_nSliderPos = mv;
			}
		}
		m_nLastX = g_pGUI->m_iMouseX;
		m_bDragInit = true;
	} else m_bDragInit = false;
}

void CSlider::Draw() {
	int ax, ay;
	GetAbsolutePosition(ax, ay);
	draw::DrawRect(ax, ay + 6, m_nSizeX, 4, colors::Create(0, 0, 0, 200));
	draw::DrawRect(ax + m_nSliderPos - 2, ay + 2, 4, m_nSizeY - 4, colors::pink);
	int tx, ty;
	char* s = strfmt("%.2f", m_fValue);
	draw::GetStringLength(fonts::MENU, s, tx, ty);
	draw::String(fonts::MENU, ax + (m_nSizeX - tx) / 2, ay + m_nSizeY / 2 - ty, colors::white, 1, s);
	delete [] s;
}
