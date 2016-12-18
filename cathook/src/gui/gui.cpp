/*
 * gui.cpp
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#include "gui.h"
#include "../common.h"
#include "../sdk.h"

GUI::GUI() {
	m_bActive = false;
}

void GUI::Draw() {
	if (!m_bActive) return;
	draw::DrawString(50, 50, colors::pink, colors::black, false, "Menu Active!!");
}

bool GUI::KeyEvent(ButtonCode_t key) {
	if (key == KEY_INSERT)
		m_bActive = !m_bActive;
	if (!m_bActive) return false;
	switch (key) {
	}
	return false;
}

GUI* g_pGUI = 0;
