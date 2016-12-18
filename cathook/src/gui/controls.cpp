/*
 * controls.cpp
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#include "controls.h"
#include "common.h"
#include "sdk.h"

GUI_List::GUI_List(const char* name) {
	m_pszListName = name;
	m_pFirst = 0;
}

void GUI_List::Draw(int x, int y) {
	IGUIListElement* current = m_pFirst;
	int n_elements = 0;
	while (current) {
		current->Draw(x + 1, y + 1 + n_elements * VERTICAL_SPACING);
		n_elements++;
		current = current->m_pNext;
	}
	draw::OutlineRect(x, y, x + LIST_WIDTH, y + n_elements * VERTICAL_SPACING, colors::tf_blu);
}

void GUI_List::AddElement(IGUIListElement* element) {
	if (!m_pFirst) {
		m_pFirst = element;
		m_pLast = element;
	}
	IGUIListElement* current = m_pFirst;
	while (current) {
		current = current->m_pNext;
	}
	if (current) {
		current->m_pNext = element;
		element->m_pPrev = current;
		m_pLast = current;
	}
}

void GUIListElement_Var::GUIListElement_Var(ICatVar* var) {
	m_pCatVar = var;
}

void GUIListElement_Var::Draw(int x, int y) {

}
