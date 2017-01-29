/*
 * CBaseButton.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#include "CBaseButton.h"

#include "../common.h"
#include "../sdk.h"

CBaseButton::CBaseButton(IWidget* parent, const char* name) : CTextLabel(parent, name) {
	m_pCallback = 0;
}

void CBaseButton::SetCallback(ButtonCallback_t& callback) {
	m_pCallback = callback;
}

void CBaseButton::Update() {
	int sx, sy;
	if (m_pszText) {
		draw::GetStringLength(fonts::MENU, m_pszText, sx, sy);
		m_nSizeX = sx + 8;
		m_nSizeY = sy + 6;
	}
	if (m_bMouseInside) g_pGUI->ShowTooltip("press\npls\nkthx");
}

void CBaseButton::Draw() {
	int x, y;
	GetAbsolutePosition(x, y);
	int textcolor = colors::pink;
	if (m_bMousePressed) {
		draw::DrawRect(x, y, m_nSizeX, m_nSizeY, colors::pink);
		textcolor = colors::white;
	}
	draw::OutlineRect(x, y, m_nSizeX, m_nSizeY, colors::pink);
	if (m_pszText)
		draw::String(fonts::MENU, x + 4, y + 3, textcolor, 1, m_pszText);
}

void CBaseButton::OnMousePress() {
	CBaseWidget::OnMousePress();
	if (m_pCallback)
		m_pCallback(this);
}
