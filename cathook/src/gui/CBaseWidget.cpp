/*
 * CBaseWidget.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "CBaseWidget.h"
#include "../common.h"

void CBaseWidget::DrawBounds(int x, int y) {
	if (m_KeyValues->IsEmpty("bounds_color")) {
		m_KeyValues->SetInt("bounds_color", colors::Create(rand() % 255, rand() % 255, rand() % 255, 255));
	}
	auto size = GetSize();
	draw::DrawRect(x, y, size.first, size.second, colors::Transparent(m_KeyValues->GetInt("bounds_color"), 0.25f));
	draw::OutlineRect(x, y, size.first, size.second, m_KeyValues->GetInt("bounds_color"));
}

CBaseWidget::CBaseWidget(std::string name, IWidget* parent) : m_KeyValues(std::string("cat_widget_" + name).c_str()) {
	m_pParent = parent;
	Props()->SetString("name", name.c_str());
	SetPositionMode(INLINE);
	Show();
}

void CBaseWidget::Update() {
	//logging::Info("Updating! %s", GetName().c_str());
}

std::pair<int, int> CBaseWidget::AbsolutePosition() {
	auto result = GetOffset();
	auto parent = GetParent();
	while (parent) {
		auto poffset = parent->GetOffset();
		result.first += poffset.first;
		result.second += poffset.second;
		parent = parent->GetParent();
	}
	return result;
}
