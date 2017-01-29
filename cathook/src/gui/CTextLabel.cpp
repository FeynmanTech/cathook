/*
 * CTextLabel.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#include "CTextLabel.h"

#include "../common.h"
#include "../sdk.h"

CTextLabel::CTextLabel(IWidget* parent, const char* name, const char* text) : CBaseWidget(parent, name) {
	m_pszText = 0;
	if (text) SetText(text);
}

CTextLabel::~CTextLabel() {
	if (m_pszText) delete [] m_pszText;
}

void CTextLabel::SetText(const char* text) {
	m_pszText = new char[strlen(text) + 1];
	strcpy(m_pszText, text);
	draw::GetStringLength(fonts::MENU, m_pszText, m_nSizeX, m_nSizeY);
}

const char* CTextLabel::GetText() {
	return (const char*)m_pszText;
}

void CTextLabel::Draw() {
	int ax, ay;
	GetAbsolutePosition(ax, ay);
	if (m_pszText)
		draw::String(fonts::MENU, ax, ay, colors::white, 1, m_pszText);
}
