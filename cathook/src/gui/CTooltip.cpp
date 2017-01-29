/*
 * CTooltip.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: nullifiedcat
 */

#include "CTooltip.h"
#include "../common.h"
#include "../sdk.h"

CTooltip::CTooltip(IWidget* parent, const char* name) : CBaseWidget(parent, name) {

}

void CTooltip::SetText(const char* text) {
	m_pszText = text;
}

void CTooltip::Draw() {
	if (!m_pszText) return;
	int ax, ay;
	int sx, sy;
	GetAbsolutePosition(ax, ay);
	draw::DrawRect(ax, ay, m_nSizeX, m_nSizeY, colors::Create(0, 0, 0, 200));
	draw::OutlineRect(ax, ay, m_nSizeX, m_nSizeY, colors::pink);
	draw::String(fonts::MENU, ax + 2, ay + 2, colors::pink, 1, m_pszText);
}

void CTooltip::Update() {
	if (!m_pszText) return;
	int sx, sy;
	draw::GetStringLength(fonts::MENU, (char*)m_pszText, sx, sy);
	m_nSizeX = sx + 4;
	m_nSizeY = sy + 4;
}
