/*
 * TitleBar.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#ifndef CTITLEBAR_H_
#define CTITLEBAR_H_

#include "CBaseWidget.h"

#define TITLEBAR_PADDING_W 2
#define TITLEBAR_PADDING_H 1

class TitleBar : public CBaseWidget {
public:
	TitleBar(IWidget* parent, std::string title);

	virtual void Draw(int x, int y);
	virtual void Update();

	std::string m_strTitle;
	int m_iDraggingStage;
	int m_nLastX;
	int m_nLastY;
};

#endif /* CTITLEBAR_H_ */
