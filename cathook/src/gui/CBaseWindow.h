/*
 * CBaseWindow.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#ifndef CBASEWINDOW_H_
#define CBASEWINDOW_H_

#include "CBaseWidget.h"

#define BASEWINDOW_CAPACITY 128

class CBaseWindow : public CBaseWidget, public virtual IWidget {
public:
	inline CBaseWindow(IWidget* parent, const char* name) : CBaseWidget(parent, name) {
		pressed = 0;
	}

	inline virtual ~CBaseWindow() {};

	virtual void Update();

	/*virtual void OnMouseEnter();
	virtual void OnMouseLeave();*/
	virtual void OnMousePress();
	virtual void OnMouseRelease();
	/*virtual void OnKeyPress(ButtonCode_t key);
	virtual void OnKeyRelease(ButtonCode_t key);

	virtual void SetOffset(int x, int y);
	virtual void GetOffset(int& x, int& y);*/
	virtual void Draw();
	/*virtual void GetSize(int& width, int& height);

	virtual IWidget* GetParent();*/

	IWidget* pressed;
};



#endif /* CBASEWINDOW_H_ */
