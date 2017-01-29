/*
 * GUI.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#ifndef GUI_H_
#define GUI_H_

class IWidget;
class CatVar;

#define GUI_ENABLED true

#include "../fixsdk.h"
#include "../inputsystem/ButtonCode.h"

class CTooltip;

class CatGUI {
public:
	CatGUI();
	~CatGUI();

	void Update();
	void Setup();
	IWidget* GetRootWindow();

	void ShowTooltip(const char* text);
	CTooltip* m_pTooltip;

	IWidget* m_pRootWindow;
	CatVar* v_bGUIVisible;
	CatVar* v_bDrawBounds;

	bool m_bKeysInit;
	bool m_bPressedState[ButtonCode_t::MOUSE_LAST];
	int  m_iPressedFrame[ButtonCode_t::MOUSE_LAST];
	int  m_iMouseX;
	int  m_iMouseY;
};

extern CatGUI* g_pGUI;

#endif /* GUI_H_ */
