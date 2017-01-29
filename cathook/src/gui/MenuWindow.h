/*
 * MenuWindow.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "CBaseWindow.h"

class MenuWindow : public CBaseWindow {
public:
	MenuWindow(IWidget* parent, const char* name);
	~MenuWindow();

	IWidget* m_pMenuList;
	IWidget* m_pMenuContents;

};
