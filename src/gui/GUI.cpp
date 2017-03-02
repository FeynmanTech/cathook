/*
 * GUI.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "GUI.h"
#include "IWidget.h"
#include "RootWindow.h"
#include "CTooltip.h"

#include "../common.h"
#include "../sdk.h"

void GUIVisibleCallback(IConVar* var, const char* pOldValue, float flOldValue) {
	interfaces::input->SetCursorPosition(draw::width / 2, draw::height / 2);
	interfaces::surface->SetCursor(vgui::CursorCode::dc_none);
	interfaces::matsurface->SetCursorAlwaysVisible(false);
	if (g_pGUI->v_bGUIVisible) {
		if (g_pGUI->v_bGUIVisible->GetBool()) {
			interfaces::matsurface->UnlockCursor();
		} else {
			interfaces::matsurface->LockCursor();
		}
	}
}

CatGUI::CatGUI() {
	m_pRootWindow = 0;
	v_bGUIVisible = new CatVar(CV_SWITCH, "gui_visible", "0", "GUI Active", NULL, "GUI switch (bind it to a key!)");
	v_bDrawBounds = new CatVar(CV_SWITCH, "gui_bounds", "0", "Draw Bounds", NULL, "Draw GUI elements' bounding boxes");
}

CatGUI::~CatGUI() {
	delete m_pRootWindow;
}

void CatGUI::Setup() {
	m_pRootWindow = new RootWindow();
	m_pRootWindow->Setup();
	v_bGUIVisible->OnRegister([](CatVar* var) {
		var->convar->InstallChangeCallback(GUIVisibleCallback);
	});
}

void CatGUI::ShowTooltip(std::string text) {
	m_pTooltip->SetText(text);
	m_pTooltip->SetOffset(m_iMouseX + 5, m_iMouseY + 5);
	m_pTooltip->Show();
	m_bShowTooltip = true;
}

void CatGUI::Update() {
	m_bShowTooltip = false;
	for (int i = 0; i < ButtonCode_t::BUTTON_CODE_COUNT; i++) {
		bool down = interfaces::input->IsButtonDown((ButtonCode_t)(i));
		bool changed = m_bPressedState[i] != down;
		if (changed && down) m_iPressedFrame[i] = interfaces::gvars->framecount;
		m_bPressedState[i] = down;
		if (m_bKeysInit) {
			if (changed) {
				//logging::Info("Key %i changed! Now %i.", i, down);
				if (i == ButtonCode_t::MOUSE_LEFT) {
					if (down) m_pRootWindow->OnMousePress();
					else m_pRootWindow->OnMouseRelease();
				} else {
					if (i == ButtonCode_t::KEY_INSERT && down) {
						v_bGUIVisible->SetValue(!v_bGUIVisible->GetBool());
					}
					if (down) m_pRootWindow->OnKeyPress((ButtonCode_t)i, false);
					else m_pRootWindow->OnKeyRelease((ButtonCode_t)i);
				}
			} else {
				if (down) {
					int frame = interfaces::gvars->framecount - m_iPressedFrame[i];
					bool shouldrepeat = false;
					if (frame) {
						if (frame > 150) {
							if (frame > 400) {
								if (frame % 30 == 0) shouldrepeat = true;
							} else {
								if (frame % 80 == 0) shouldrepeat = true;
							}
						}
					}
					if (shouldrepeat) m_pRootWindow->OnKeyPress((ButtonCode_t)i, true);
				}
			}
		}
	}


	m_iMouseX = interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_X);
	m_iMouseY = interfaces::input->GetAnalogValue(AnalogCode_t::MOUSE_Y);

	if (!m_bKeysInit) m_bKeysInit = 1;
	if (v_bGUIVisible->GetBool()) {
		if (!m_pRootWindow->IsVisible())
			m_pRootWindow->Show();
		m_pRootWindow->Update();
		if (!m_bShowTooltip && m_pTooltip->IsVisible()) m_pTooltip->Hide();
		m_pRootWindow->Draw(0, 0);
		draw::DrawRect(m_iMouseX - 5, m_iMouseY - 5, 10, 10, colors::Transparent(colors::white));
		draw::OutlineRect(m_iMouseX - 5, m_iMouseY - 5, 10, 10, colors::pink);
		if (v_bDrawBounds->GetBool()) {
			m_pRootWindow->DrawBounds(0, 0);
		}
	} else {
		if (m_pRootWindow->IsVisible())
			m_pRootWindow->Hide();
	}


}

bool CatGUI::ConsumesKey(ButtonCode_t key) {
	if (m_pRootWindow->IsVisible())
		return m_pRootWindow->ConsumesKey(key);
	else return false;
}

RootWindow* CatGUI::GetRootWindow() {
	return m_pRootWindow;
}

CatGUI* g_pGUI = 0;
