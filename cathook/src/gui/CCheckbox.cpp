/*
 * CCheckbox.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#include "CCheckbox.h"

#include "../common.h"
#include "../sdk.h"

CCheckbox::CCheckbox(IWidget* parent, const char* name, bool checked) : CBaseWidget(parent, name) {
	SetWidth(16);
	this->m_bChecked = checked;
}

void CCheckbox::SetWidth(int width) {
	m_nWidth = width;
	m_nSizeX = width;
	m_nSizeY = width;
}

void CCheckbox::Draw() {
	int x, y;
	GetAbsolutePosition(x, y);
	draw::OutlineRect(x, y, m_nWidth, m_nWidth, colors::pink);
	if (m_bChecked) {
		draw::DrawRect(x + 2, y + 2, m_nWidth - 4, m_nWidth - 4, colors::pink);
	}
}

void CCheckbox::OnMousePress() {
	m_bChecked = !m_bChecked;
}
