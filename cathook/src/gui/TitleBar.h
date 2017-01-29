/*
 * TitleBar.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#ifndef TITLEBAR_H_
#define TITLEBAR_H_

#include "CBaseWidget.h"

class TitleBar : public CBaseWidget {
public:
	TitleBar(IWidget* parent, const char* text);

	virtual void GetSize(int& x, int& y);
	virtual void Draw();
	virtual void Update();
	virtual bool ShouldResizeToFit();

	char m_Text[256];
	int m_iDraggingStage;
	int m_nLastX;
	int m_nLastY;
};

#endif /* TITLEBAR_H_ */
