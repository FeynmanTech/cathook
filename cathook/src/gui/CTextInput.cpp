/*
 * CTextInput.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#include "CTextInput.h"

#include "../common.h"
#include "../sdk.h"
#include "GUI.h"

CTextInput::CTextInput(IWidget* parent, const char* name) : CBaseWidget(parent, name) {
	m_pszContents = new char[256];
	m_pCallback = 0;
	m_nLength = 0;
}

void CTextInput::SetMaxWidth(int width) {
	int length, height;
	draw::GetStringLength(fonts::MENU, "W", length, height);
	m_nSizeX = width * length + 4;
	m_nSizeY = height + 4;
}

void CTextInput::Draw() {
	int ax, ay;
	GetAbsolutePosition(ax, ay);
	int height, length;
	draw::GetStringLength(fonts::MENU, "W", length, height);
	int color = colors::Create(0, 0, 0, 80);
	if (m_bFocused) color = colors::Create(255, 255, 255, 80);
	draw::DrawRect(ax, ay, m_nSizeX, height + 4, color);
	draw::OutlineRect(ax, ay, m_nSizeX, height + 4, colors::pink);
	int ml = 0;
	int md = 0;
	int tx, ty;
	draw::GetStringLength(fonts::MENU, "...", tx, ty);
	for (int i = 0; i < strlen(m_pszContents); i++) {
		int w, h;
		draw::GetStringLength(fonts::MENU, m_pszContents + i, w, h);
		if (w + 4 + tx > m_nSizeX) md = i;
		if (w + 4 > m_nSizeX) ml = i;
	}
	if (ml) {
		draw::FString(fonts::MENU, ax + 2, ay + 2, colors::white, 1, "...%s", (m_pszContents + md));
	} else {
		draw::String(fonts::MENU, ax + 2, ay + 2, colors::white, 1, m_pszContents);
	}
}

void CTextInput::SetCallback(TextInputChangeCallback_t* callback) {
	m_pCallback = callback;
}

void CTextInput::SetLength(int newlength) {
	m_pszContents[newlength] = 0;
	m_nLength = newlength;
}

void CTextInput::PutChar(char ch) {
	if (m_nLength >= 255) return;
	char* old = new char[256];
	memcpy(old, m_pszContents, 256);
	m_pszContents[m_nLength] = ch;
	SetLength(m_nLength + 1);
	if (m_pCallback) {
		m_pCallback(this, old, m_pszContents);
	}
	delete [] old;
}

void CTextInput::OnKeyPress(ButtonCode_t key) {
	if (key == ButtonCode_t::KEY_BACKSPACE) {
		if (m_nLength > 0) {
			char* old = new char[256];
			memcpy(old, m_pszContents, 256);
			SetLength(m_nLength - 1);
			if (m_pCallback) {
				m_pCallback(this, old, m_pszContents);
			}
			delete [] old;
		}
		return;
	} else if (key == ButtonCode_t::KEY_SPACE) {
		PutChar(' ');
		return;
	} else {
		if (strlen(interfaces::input->ButtonCodeToString(key)) == 1) {
			if (g_pGUI->m_bPressedState[ButtonCode_t::KEY_LSHIFT] || g_pGUI->m_bPressedState[ButtonCode_t::KEY_RSHIFT]) {
				PutChar(GetUpperChar(key));
			} else {
				PutChar(GetChar(key));
			}
		}
	}
}

