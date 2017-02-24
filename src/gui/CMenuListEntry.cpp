/*
 * CMenuListEntry.cpp
 *
 *  Created on: Feb 3, 2017
 *      Author: nullifiedcat
 */

#include "CMenuListEntry.h"
#include "CMenuList.h"

#include "../common.h"
#include "../sdk.h"

CMenuListEntry::CMenuListEntry(std::string name, CMenuList* parent, std::string entry) : CBaseButton(name, parent, entry) {
	Props()->SetString("entry", entry.c_str());
}

void CMenuListEntry::SetMaxSize(int x, int y) {
	CBaseWidget::SetMaxSize(x, y);
	SetSize(x, y);
}

bool CMenuListEntry::IsSelected() {
	return (dynamic_cast<CMenuList*>(GetParent())->m_pSelected == this);
}

void CMenuListEntry::Draw(int x, int y) {
	auto texts = draw::GetStringLength(fonts::MENU_BIG, GetText());
	auto size = GetSize();
	if (IsSelected()) {
		draw::DrawLine(x, y, size.first, 0, colors::pink);
		draw::DrawLine(x, y + size.second, size.first, 0, colors::pink);
		draw::DrawLine(x, y, 0, size.second, colors::pink);
	} else {
		draw::OutlineRect(x, y, size.first, size.second, colors::pink);
	}
	if (IsHovered()) {
		draw::DrawRect(x, y, size.first, size.second, colors::Transparent(colors::pink, 0.25));
	}
	draw::String(fonts::MENU_BIG, x + (size.first - texts.first) / 2, y + (size.second - texts.second) / 2, IsSelected() ? colors::white : colors::pink, 1, GetText());
}
