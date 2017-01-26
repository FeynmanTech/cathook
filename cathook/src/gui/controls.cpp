/*
 * controls.cpp
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#include "controls.h"
#include "gui.h"
#include "../common.h"
#include "../sdk.h"

GUI_List::GUI_List(const char* name, const char* title) {
	m_pszListID = name;
	m_pszListTitle = title;
	m_pFirst = 0;
	m_nElementCount = 0;
}

void GUI_List::Move(int x, int y) {
	this->x = x;
	this->y = y;
}

void GUI_List::Draw() {
	IGUIListElement* current = m_pFirst;
	int curidx = 0;
	while (current) {
		if (current == m_pSelected) {
			draw::DrawRect(x, y + curidx * VERTICAL_SPACING - curidx, LIST_WIDTH, VERTICAL_SPACING, colors::Transparent(colors::pink, 0.1f));
		} else {
			//draw::DrawRect(x, y + curidx * VERTICAL_SPACING, LIST_WIDTH, VERTICAL_SPACING, colors::Transparent(colors::black));
		}
		draw::OutlineRect(x, y + curidx * VERTICAL_SPACING - curidx, LIST_WIDTH, VERTICAL_SPACING, colors::pink);
		current->Draw(x + 1, y + 1 + curidx++ * VERTICAL_SPACING - curidx, (current == m_pSelected));

		current = current->m_pNext;
	}
	//draw::OutlineRect(x, y, LIST_WIDTH, m_nElementCount * VERTICAL_SPACING, colors::pink);
}

void GUI_List::AddElement(IGUIListElement* element) {
	if (!m_pFirst) {
		m_pSelected = element;
		m_pFirst = element;
		m_pLast = element;
		element->m_nIndex = m_nElementCount++;
		element->m_pParentList = this;
		return;
	}
	IGUIListElement* current = m_pFirst;
	while (current) {
		if (current->m_pNext)
			current = current->m_pNext;
		else {
			break;
		}
	}
	if (current) {
		current->m_pNext = element;
		element->m_pPrev = current;
		element->m_nIndex = m_nElementCount++;
		element->m_pParentList = this;
		m_pLast = element;
	}
}

IGUIListElement::~IGUIListElement() {}

void GUI_List::SelectNext() {
	if (!m_pSelected) {
		m_pSelected = m_pFirst;
	} else {
		m_pSelected = m_pSelected->m_pNext;
	}
	if (!m_pSelected)
		m_pSelected = m_pFirst;
}

void GUI_List::SelectPrev() {
	if (!m_pSelected) {
		m_pSelected = m_pFirst;
	} else {
		m_pSelected = m_pSelected->m_pPrev;
	}
	if (!m_pSelected)
		m_pSelected = m_pLast;
}

void GUI_List::KeyEvent(ButtonCode_t key) {
	switch (key) {
	case ButtonCode_t::KEY_UP:
		SelectPrev(); break;
	case ButtonCode_t::KEY_DOWN:
		SelectNext(); break;
	default:
		if (m_pSelected) {
			m_pSelected->KeyEvent(key);
		}
	}
}

GUIListElement_TitleList::GUIListElement_TitleList(const char* title) {
	m_pszTitle = title; // TODO strcpy
	m_pNext = 0;
	m_pPrev = 0;
}

void GUIListElement_TitleList::Draw(int x, int y, bool selected) {
	int l, h;
	draw::GetStringLength((char*)m_pszTitle, l, h);
	draw::DrawString(draw::font_handle_menu, x + (LIST_WIDTH - l) / 2, y, colors::white, "%s", m_pszTitle);
}

void GUIListElement_TitleList::KeyEvent(ButtonCode_t key) {
	return;
}

GUIListElement_Var::GUIListElement_Var(CatVar* var) {
	m_pCatVar = var;
	m_pNext = 0;
	m_pPrev = 0;
}

void GUIListElement_Var::Draw(int x, int y, bool selected) {
	switch (m_pCatVar->m_Type) {
	case CatVar_t::CV_SWITCH: {
		draw::DrawString(draw::font_handle_menu, x, y, colors::white, "%s", m_pCatVar->GetConVar()->GetHelpText());
		int l, h;
		bool enabled = m_pCatVar->GetConVar()->GetInt();
		draw::GetStringLength(enabled ? (char*)"ON" : (char*)"OFF", l, h);
		draw::DrawString(draw::font_handle_menu, x + LIST_WIDTH - l - 3, y, colors::white, enabled ? "ON" : "OFF");
	} break;
	case CatVar_t::CV_FLOAT: {
		draw::DrawString(draw::font_handle_menu, x, y, colors::white, "%s", m_pCatVar->GetConVar()->GetHelpText());
		int l, h;
		draw::GetStringLength(strfmt("%.1f", m_pCatVar->GetConVar()->GetFloat()), l, h);
		draw::DrawString(draw::font_handle_menu, x + LIST_WIDTH - l - 3, y, colors::white, "%.1f", m_pCatVar->GetConVar()->GetFloat());
	} break;
	case CatVar_t::CV_INT: {
		draw::DrawString(draw::font_handle_menu, x, y, colors::white, "%s", m_pCatVar->GetConVar()->GetHelpText());
		int l, h;
		draw::GetStringLength(strfmt("%i", m_pCatVar->GetConVar()->GetInt()), l, h);
		draw::DrawString(draw::font_handle_menu, x + LIST_WIDTH - l - 3, y, colors::white, "%i", m_pCatVar->GetConVar()->GetInt());
	} break;
	case CatVar_t::CV_ENUM: {
		draw::DrawString(draw::font_handle_menu, x, y, colors::white, "%s", m_pCatVar->GetConVar()->GetHelpText());
		int l, h;
		const char* str = m_pCatVar->m_EnumType->Name(m_pCatVar->GetInt());
		if (str) {
			draw::GetStringLength((char*)str, l, h);
			draw::DrawString(draw::font_handle_menu, x + LIST_WIDTH - l - 3, y, colors::white, str);
		} else {
			draw::GetStringLength(strfmt("%i", m_pCatVar->GetConVar()->GetInt()), l, h);
			draw::DrawString(draw::font_handle_menu, x + LIST_WIDTH - l - 3, y, colors::white, "%i", m_pCatVar->GetConVar()->GetInt());
		}
	} break;
	}
}

void GUIListElement_Var::KeyEvent(ButtonCode_t key) {
	int factor = 1;
	//if (g_pGUI->m_bPressedState[ButtonCode_t::KEY_LSHIFT]) factor *= 10;
	//if (g_pGUI->m_bPressedState[ButtonCode_t::KEY_LCONTROL]) factor *= 100;
	switch (key) {
	case ButtonCode_t::KEY_SPACE:
	case ButtonCode_t::KEY_ENTER:
		m_pCatVar->Increment(factor);
		break;
	case ButtonCode_t::KEY_RIGHT:
		m_pCatVar->Increment(factor);
		break;
	case ButtonCode_t::KEY_LEFT:
		m_pCatVar->Decrement(factor);
		break;
	}
}

GUIListElement_SubList::GUIListElement_SubList(GUI_List* list) {
	m_pList = list;
	m_pNext = 0;
	m_pPrev = 0;
}

void GUIListElement_SubList::Draw(int x, int y, bool selected) {
	if (selected) {
		draw::DrawString(draw::font_handle_menu, x, y, colors::white, "[-] %s", m_pList->m_pszListTitle);
	} else {
		draw::DrawString(draw::font_handle_menu, x, y, colors::white, "[+] %s", m_pList->m_pszListTitle);
	}

}

void GUIListElement_SubList::KeyEvent(ButtonCode_t key) {
	switch (key) {
	case ButtonCode_t::KEY_ENTER:
	case ButtonCode_t::KEY_SPACE:
	case ButtonCode_t::KEY_RIGHT:
		m_pList->Move(m_pParentList->x + LIST_WIDTH - 1, m_pParentList->y + VERTICAL_SPACING * m_nIndex - m_nIndex);
		//g_pGUI->PushList(m_pList->m_pszListID);
	}
}
