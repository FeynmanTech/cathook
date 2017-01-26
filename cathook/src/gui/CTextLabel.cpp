/*
 * CTextLabel.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#include "CTextLabel.h"

#include "../common.h"
#include "../sdk.h"

CTextLabel::CTextLabel(IWidget* parent, const char* name) : CBaseWidget(parent, name) {
	m_pszText = 0;
}

CTextLabel::~CTextLabel() {
	if (m_pszText) delete [] m_pszText;
}

void CTextLabel::SetText(const char* text) {
	m_pszText = new char[strlen(text) + 1];
	strcpy(m_pszText, text);
	draw::GetStringLength(m_pszText, m_nSizeX, m_nSizeY);
}

const char* CTextLabel::GetText() {
	return (const char*)m_pszText;
}

void CTextLabel::Draw() {
	int ax, ay;
	GetAbsolutePosition(ax, ay);
	draw::DrawString(ax, ay, colors::white, "%s", m_pszText);
}
