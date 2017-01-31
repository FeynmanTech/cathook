/*
 * CSlider.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: nullifiedcat
 */

#include "CSlider.h"
#include "../common.h"
#include "../sdk.h"

CSlider::CSlider(std::string name, IWidget* parent) : CBaseWidget(name, parent) {
	Setup(0.0f, 1.0f);
	SetSize(80, 16);
	m_nLastX = 0;
	m_nSliderPos = 40;
	m_bDragInit = false;
	m_pCallback = 0;
}

void CSlider::Setup(float min, float max) {
	Props()->SetFloat("value_min", min);
	Props()->SetFloat("value_max", min);
	SetValue((min + max) / 2.0f);
}

void CSlider::SetCallback(SliderCallbackFn_t callback) {
	m_pCallback = callback;
}

void CSlider::SetValue(float value) {
	float old = Value();
	Props()->SetFloat("value", value);
	if (old != value) {
		if (m_pCallback) {
			m_pCallback(this, old, value);
		}
	}
}

float CSlider::Value() {
	return Props()->GetFloat("value");
}

void CSlider::Update() {
	if (IsPressed()) {
		if (m_bDragInit) {
			int delta = m_nLastX - g_pGUI->m_iMouseX;
			if (delta) {
				auto abs = AbsolutePosition();
				auto size = GetSize();
				int mv = g_pGUI->m_iMouseX - abs.first;
				if (mv < 0) mv = 0;
				if (mv > size.first) mv = size.first;
				SetValue(((float)mv / (float)size.first) * (Props()->GetFloat("value_max") - Props()->GetFloat("value_min")));
				m_nSliderPos = mv;
			}
		}
		m_nLastX = g_pGUI->m_iMouseX;
		m_bDragInit = true;
	} else m_bDragInit = false;
}

void CSlider::Draw(int x, int y) {
	auto size = GetSize();
	draw::DrawRect(x, y + size.second / 2 - 2, size.first, 4, colors::Create(0, 0, 0, 200));
	draw::DrawRect(x + m_nSliderPos - 2, y + 2, 4, size.second - 4, colors::pink);
	std::string str(strfmt("%.2f", Value()));
	auto sl = draw::GetStringLength(fonts::MENU, str);
	draw::String(fonts::MENU, x + (size.first - sl.first) / 2, y + (size.second - sl.second) / 2, colors::white, 1, str);
}
