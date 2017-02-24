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

#include "../beforecheaders.h"
#include <string>
#include "../aftercheaders.h"

#include "../inputsystem/ButtonCode.h"

class CTooltip;
class RootWindow;

class CatGUI {
public:
	CatGUI();
	~CatGUI();

	void Update();
	void Setup();
	RootWindow* GetRootWindow();
	bool ConsumesKey(ButtonCode_t key);

	void ShowTooltip(std::string text);

	CTooltip* m_pTooltip;
	RootWindow* m_pRootWindow;
	CatVar* v_bGUIVisible;
	CatVar* v_bDrawBounds;

	bool m_bShowTooltip;
	bool m_bConsumeKeys;
	bool m_bKeysInit;
	bool m_bPressedState[ButtonCode_t::BUTTON_CODE_COUNT];
	int  m_iPressedFrame[ButtonCode_t::BUTTON_CODE_COUNT];
	int  m_iMouseX;
	int  m_iMouseY;
};

extern CatGUI* g_pGUI;

#endif /* GUI_H_ */
