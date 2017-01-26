/*
 * MenuList.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#ifndef MENULIST_H_
#define MENULIST_H_

#include "CBaseWindow.h"

class MenuList : public CBaseWindow {
public:
	MenuList(IWidget* parent);

	virtual void GetSize(int& x, int& y);
};


#endif /* MENULIST_H_ */
