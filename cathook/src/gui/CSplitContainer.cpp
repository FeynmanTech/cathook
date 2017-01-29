/*
 * CSplitContainer.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: nullifiedcat
 */

#include "CSplitContainer.h"
#include "../common.h"

CSplitContainer::CSplitContainer(IWidget* parent, const char* name) : CBaseWindow(parent, name) {

}

void CSplitContainer::Update() {
	m_nSizeX = m_nMaxX;
	if (!GetChildrenCount()) return;
	int width = ((m_nSizeX - 4) / GetChildrenCount()) - 2;
	for (int i = 0; i < GetChildrenCount(); i++) {
		IWidget* child = GetChildByIndex(i);
		child->SetOffset(2 + i * width, 2);
		child->SetMaxSize(width, -1);
		child->Update();
		int sx, sy;
		child->GetSize(sx, sy);
		if (sy + 4 > m_nSizeY) m_nSizeY = sy + 4;
	}
	int ax, ay;
	GetAbsolutePosition(ax, ay);
	IWidget* nhov = GetChildByPoint(g_pGUI->m_iMouseX - ax, g_pGUI->m_iMouseY - ay);
	if (hovered) {
		if (nhov != hovered) hovered->OnMouseLeave();
		if (nhov) nhov->OnMouseEnter();
	}
	hovered = nhov;
}
