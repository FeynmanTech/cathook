/*
 * CBaseWindow.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "CBaseWindow.h"

#include "../common.h"
#include "GUI.h"

void CBaseWindow::Update() {
	int mx = 0, my = 0;
	for (int i = 0; i < m_nChildCount; i++) {
		m_pChildrenList[i]->Update();
		int sx, sy, ox, oy;
		m_pChildrenList[i]->GetOffset(ox, oy);
		m_pChildrenList[i]->GetSize(sx, sy);
		if (sx + ox > mx) mx = sx + ox;
		if (sy + oy > my) my = sy + oy;
	}
	if (GetParent()) {
		m_nSizeX = mx;
		m_nSizeY = my;
	}
}

void CBaseWindow::OnMousePress() {
	int ax, ay;
	this->GetAbsolutePosition(ax, ay);
	logging::Info("%s MousePress! %i %i", GetName(), g_pGUI->m_iMouseX - ax, g_pGUI->m_iMouseY - ay);
	pressed = GetChildByPoint(g_pGUI->m_iMouseX - ax, g_pGUI->m_iMouseY - ay);
	if (pressed) {
		logging::Info("%s Child MousePress! %s", GetName(), pressed->GetName());
		pressed->OnMousePress();
	}
}

void CBaseWindow::OnMouseRelease() {
	if (pressed)
		pressed->OnMouseRelease();
}


void CBaseWindow::Draw() {
	int px = 0, py = 0;
	GetAbsolutePosition(px, py);
	int sx, sy;
	GetSize(sx, sy);
	draw::DrawRect(px, py, sx, sy, colors::Transparent(colors::black));
	draw::OutlineRect(px, py, sx, sy, colors::pink);
	for (int i = 0; i < m_nChildCount; i++) {
		m_pChildrenList[i]->Draw();
	}
}
