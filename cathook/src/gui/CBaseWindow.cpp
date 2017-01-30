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
	/*int mx = 100, my = 0;
	int cw = 0; // Column Width
	int co = 4; // X Column Offset
	int lx = cw + co; // Last X
	int ly = 24; // Last Y

	for (int i = 0; i < m_nChildCount; i++) {
		IWidget* child = GetChildByIndex(i);
		child->Update();
		int sx, sy;
		child->GetSize(sx, sy);
		if (m_nMaxY > 0) {
			int ny = ly;
			switch (child->GetPositionMode()) {
			case PositionMode::INLINE_BLOCK:
				ny += (sy + 4);
				break;
			}
			if (ny > m_nMaxY) {
				co += cw;
				ly = 24;
			}
		}
		switch (child->GetPositionMode()) {
		case PositionMode::INLINE:
			child->SetOffset(lx, ly);
			lx += (sx + 4);
			if (lx > cw) cw = lx - co + 4;
			break;
		case PositionMode::INLINE_BLOCK:
			lx = co;
			ly += (sy + 4);
			child->SetOffset(lx, ly);
			lx += (sx + 4);
			if (lx > cw) cw = lx - co;
			break;
		}
		if (m_pChildrenList[i]->ShouldResizeToFit()) {
			int ox, oy;
			child->GetOffset(ox, oy);
			if (child->GetPositionMode() == PositionMode::ABSOLUTE) {
				if (ox + sx > mx) mx = ox + sx;
				if (oy + sy > my) my = oy + sy;
			} else {
				if (cw + co >= mx) mx = cw + co;
				if (ly + sy >= my) my = (ly + sy);
			}
		}
	}*/
	int mx = 0, my = 2;
	for (int i = 0; i < GetChildrenCount(); i++) {
		IWidget* c = GetChildByIndex(i);
		if (!c->IsVisible()) continue;
		int sx, sy;
		int ox, oy;
		c->GetOffset(ox, oy);
		c->GetSize(sx, sy);
		if (c->GetPositionMode() != ABSOLUTE && c->GetPositionMode() != FLOATING)
			c->SetOffset(2, my);
		else {
			sx += ox;
			sy += oy;
		}
		if (c->ShouldResizeToFit() && c->GetPositionMode() != FLOATING)
			if (sx > mx) mx = sx;
		if (c->GetPositionMode() != FLOATING)
			my += (sy + 2);
		c->Update();
	}
	if (GetParent()) {
		m_nSizeX = mx + 4;
		m_nSizeY = my + 2;
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

void CBaseWindow::OnFocusLose() {
	m_bFocused = false;
	if (focused) focused->OnFocusLose();
}

void CBaseWindow::OnMouseLeave() {
	if (hovered) hovered->OnMouseLeave();
	hovered = 0;
}

void CBaseWindow::OnMousePress() {
	int ax, ay;
	this->GetAbsolutePosition(ax, ay);
	IWidget* newfocus = GetChildByPoint(g_pGUI->m_iMouseX - ax, g_pGUI->m_iMouseY - ay);
	if (newfocus) {
		if (focused) focused->OnFocusLose();
		focused = newfocus;
		newfocus->OnFocusGain();
		newfocus->OnMousePress();
	} else {
		if (focused) focused->OnFocusLose();
		focused = 0;
	}
}

void CBaseWindow::OnMouseRelease() {
	if (focused)
		focused->OnMouseRelease();
}

void CBaseWindow::OnKeyPress(ButtonCode_t key) {
	if (focused) {
		focused->OnKeyPress(key);
	}
}

void CBaseWindow::OnKeyRelease(ButtonCode_t key) {
	if (focused) {
		focused->OnKeyRelease(key);
	}
}

void CBaseWindow::Draw() {
	int px = 0, py = 0;
	GetAbsolutePosition(px, py);
	int sx, sy;
	GetSize(sx, sy);
	draw::DrawRect(px, py, sx, sy, colors::Transparent(colors::black));
	draw::OutlineRect(px, py, sx, sy, colors::pink);
	for (int i = m_nChildCount - 1; i >= 0; i--) {
		if (m_pChildrenList[i]->IsVisible())
			m_pChildrenList[i]->Draw();
	}
}

bool CBaseWindow::ConsumesKey(ButtonCode_t key) {
	if (focused && focused->IsVisible()) {
		return focused->ConsumesKey(key);
	}
	return false;
}
