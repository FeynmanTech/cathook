/*
 * GUI.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "GUI.h"
#include "IWidget.h"
#include "RootWindow.h"

#include "../common.h"
#include "../sdk.h"

void GUIVisibleCallback(IConVar* var, const char* pOldValue, float flOldValue) {
	interfaces::input->SetCursorPosition(0, 0);
}

CatGUI::CatGUI() {
	m_pRootWindow = 0;
	v_bGUIVisible = CREATE_CV(CV_SWITCH, "gui_visible", "0", "GUI Active");
	v_bDrawBounds = CREATE_CV(CV_SWITCH, "gui_bounds", "0", "Draw Bounds");
}

CatGUI::~CatGUI() {
	delete m_pRootWindow;
}

void CatGUI::Setup() {
	m_pRootWindow = new RootWindow();
	v_bGUIVisible->m_pConVar->InstallChangeCallback(GUIVisibleCallback);
}

void CatGUI::Update() {
	for (int i = 0; i < ButtonCode_t::MOUSE_LAST; i++) {
		bool down = interfaces::input->IsButtonDown((ButtonCode_t)(KEY_FIRST + i));
		bool changed = m_bPressedState[i] != down;
		if (changed && down) m_iPressedFrame[i] = interfaces::gvars->framecount;
		m_bPressedState[i] = down;
		if (m_bKeysInit) {
			if (changed) {
				//logging::Info("Key %i changed! Now %i.", i, down);
				if (i >= ButtonCode_t::MOUSE_FIRST && i <= ButtonCode_t::MOUSE_LEFT) {
					if (down) m_pRootWindow->OnMousePress();
					else m_pRootWindow->OnMouseRelease();
				} else {
					if (down) m_pRootWindow->OnKeyPress((ButtonCode_t)i);
					else m_pRootWindow->OnKeyRelease((ButtonCode_t)i);
				}
			}
		}
	}

	m_iMouseX = interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_X);
	m_iMouseY = interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_Y);

	if (!m_bKeysInit) m_bKeysInit = 1;
	if (v_bGUIVisible->GetBool()) {
		m_pRootWindow->Update();
		m_pRootWindow->Draw();
		draw::DrawRect(m_iMouseX - 5, m_iMouseY - 5, 10, 10, colors::Transparent(colors::white));
		draw::OutlineRect(m_iMouseX - 5, m_iMouseY - 5, 10, 10, colors::pink);
		if (v_bDrawBounds->GetBool()) {
			m_pRootWindow->DrawBounds();
		}
	}


}

IWidget* CatGUI::GetRootWindow() {
	return m_pRootWindow;
}
