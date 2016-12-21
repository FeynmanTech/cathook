/*
 * gui.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef GUI_H_
#define GUI_H_

#include "../sdk.h"

#define LISTS_MAX 64

class GUI_List;

class GUI {
public:
	GUI();
	void Draw();
	bool KeyEvent(ButtonCode_t key);
	void AddList(GUI_List* list);

	void PushList(const char* id);
	void PopList();
	void UpdateKeys();

	void Setup();

	bool m_bKeysInit;
	bool m_bPressedState[ButtonCode_t::KEY_COUNT];
	int m_iPressedFrame[ButtonCode_t::KEY_COUNT];
	GUI_List** m_Lists;
	GUI_List** m_ListStack;
	int m_nListCount;
	int m_nStackSize;
	bool m_bActive;
};

extern GUI* g_pGUI;

#endif /* GUI_H_ */
