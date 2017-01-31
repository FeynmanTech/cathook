/*
 * CBaseContainer.cpp
 *
 *  Created on: Jan 31, 2017
 *      Author: nullifiedcat
 */

#include "CBaseContainer.h"

#include "../common.h"
#include "../sdk.h"

CBaseContainer::CBaseContainer(std::string name, IWidget* parent) : CBaseWidget(name, parent) {
	m_pFocusedChild = 0;
	m_pHoveredChild = 0;
	m_pPressedChild = 0;
}

void CBaseContainer::AddChild(IWidget* child) {
	m_children.push_back(child);
	child->SetParent(this);
}

IWidget* CBaseContainer::ChildByIndex(int idx) {
	if (idx < 0 || idx >= ChildCount()) return nullptr;
	return m_children.at(idx);
}

IWidget* CBaseContainer::ChildByName(std::string name) {
	for (auto child : m_children) {
		if (!child->GetName().compare(name)) {
			return child;
		}
	}
	return nullptr;
}

IWidget* CBaseContainer::ChildByPoint(int x, int y) {
	for (int i = ChildCount() - 1; i >= 0; i--) {
		auto child = ChildByIndex(i);
		auto co = child->GetOffset();
		auto cs = child->GetSize();
		if (x >= co.first && x <= co.first + cs.first &&
			y >= co.second && y <= co.second + cs.second) {
			return child;
		}
	}
	return nullptr;
}

int CBaseContainer::ChildCount() {
	return m_children.size();
}

void CBaseContainer::Draw(int x, int y) {
	for (auto child : m_children) {
		if (child->IsVisible()) {
			auto off = child->GetOffset();
			child->Draw(x + off.first, y + off.second);
		}
	}
}

void CBaseContainer::DrawBounds(int x, int y) {
	for (auto child : m_children) {
		if (child->IsVisible()) {
			auto off = child->GetOffset();
			child->DrawBounds(x + off.first, y + off.second);
		}
	}
	CBaseWidget::DrawBounds(x, y);
}

void CBaseContainer::FocusOn(IWidget* child) {
	if (GetFocusedChild()) GetFocusedChild()->OnFocusLose();
	m_pFocusedChild = child;
	if (child) child->OnFocusGain();
}

IWidget* CBaseContainer::GetFocusedChild() {
	return m_pFocusedChild;
}

IWidget* CBaseContainer::GetHoveredChild() {
	return m_pHoveredChild;
}

IWidget* CBaseContainer::GetPressedChild() {
	return m_pPressedChild;
}

bool CBaseContainer::ConsumesKey(ButtonCode_t key) {
	if (GetFocusedChild()) return GetFocusedChild()->ConsumesKey(key);
	return false;
}

void CBaseContainer::Hide() {
	CBaseWidget::Hide();
	if (GetHoveredChild()) GetHoveredChild()->OnMouseLeave();
	if (GetFocusedChild()) GetFocusedChild()->OnFocusLose();
	if (GetPressedChild()) GetPressedChild()->OnMouseRelease();
}

void CBaseContainer::HoverOn(IWidget* child) {
	if (GetHoveredChild()) GetHoveredChild()->OnMouseLeave();
	if (child) child->OnMouseEnter();
	m_pHoveredChild = child;
}

void CBaseContainer::MoveChildren() {
	return;
}

void CBaseContainer::OnFocusLose() {
	if (GetFocusedChild()) GetFocusedChild()->OnFocusLose();
}

void CBaseContainer::OnKeyPress(ButtonCode_t key) {
	if (GetFocusedChild()) GetFocusedChild()->OnKeyPress(key);
}

void CBaseContainer::OnKeyRelease(ButtonCode_t key) {
	if (GetFocusedChild()) GetFocusedChild()->OnKeyRelease(key);
}

void CBaseContainer::OnMouseLeave() {
	if (GetHoveredChild()) GetHoveredChild()->OnMouseLeave();
}

void CBaseContainer::OnMousePress() {
	auto abs = AbsolutePosition();
	PressOn(ChildByPoint(g_pGUI->m_iMouseX - abs.first, g_pGUI->m_iMouseY - abs.second));
}

void CBaseContainer::OnMouseRelease() {
	if (GetPressedChild()) GetPressedChild()->OnMouseRelease();
}

void CBaseContainer::PressOn(IWidget* child) {
	m_pPressedChild = child;
	if (child) child->OnMousePress();
}

void CBaseContainer::SortByZIndex() {
	std::sort(m_children.begin(), m_children.end(), [](IWidget* a, IWidget* b) -> bool {
		return a->GetZIndex() < b->GetZIndex();
	});
}

void CBaseContainer::UpdateHovers() {
	auto abs = AbsolutePosition();
	auto hovered = ChildByPoint(g_pGUI->m_iMouseX - abs.first, g_pGUI->m_iMouseY - abs.second);
	if (hovered != GetHoveredChild()) {
		HoverOn(hovered);
	}
}

void CBaseContainer::Update() {
	SortByZIndex();
	UpdateHovers();
}







