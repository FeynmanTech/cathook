/*
 * CBaseWidget.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "CBaseWidget.h"
#include "../common.h"

void CBaseWidget::DrawBounds() {
	int x, y;
	GetAbsolutePosition(x, y);
	int w, h;
	GetSize(w, h);
	draw::OutlineRect(x, y, w, h, colors::red);
	for (int i = 0; i < m_nChildCount; i++) {
		GetChildByIndex(i)->DrawBounds();
	}
}
