/*
 * gui.h
 *
 *  Created on: Dec 17, 2016
 *      Author: nullifiedcat
 */

#ifndef GUI_H_
#define GUI_H_

#include "../sdk.h"

class GUI {
public:
	GUI();
	void Draw();
	bool KeyEvent(ButtonCode_t key);

	bool m_bActive;
};

extern GUI* g_pGUI;

#endif /* GUI_H_ */
