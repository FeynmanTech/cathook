/*
 * CBaseWindow.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "CBaseWindow.h"

#include "../common.h"
#include "GUI.h"

void CBaseWindow::MoveChildren() {
	int mx = 0, my = 2;
	for (auto c : m_children) {
		if (!c->IsVisible()) continue;
		auto off = c->GetOffset();
		auto size = c->GetSize();
		if (c->GetPositionMode() != ABSOLUTE && c->GetPositionMode() != FLOATING)
			c->SetOffset(2, my);
		else {
			size.first += off.first;
			size.second += off.second;
		}
		if (c->GetPositionMode() != FLOATING)
			if (size.first > mx) mx = size.first;
		if (c->GetPositionMode() != FLOATING)
			my += (size.second + 2);
		c->Update();
	}
	if (GetParent()) {
		SetSize(mx + 4, my + 2);
	}
}

void CBaseWindow::Draw(int x, int y) {
	auto abs = AbsolutePosition();
	auto size = GetSize();
	draw::DrawRect(abs.first, abs.second, size.first, size.second, colors::Transparent(colors::black));
	draw::OutlineRect(abs.first, abs.second, size.first, size.second, colors::pink);
	CBaseContainer::Draw(x, y);
}
