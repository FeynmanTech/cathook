/*
 * CTextLabel.h
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#ifndef CTEXTLABEL_H_
#define CTEXTLABEL_H_

#include "CBaseWidget.h"

#include "../fixsdk.h"
#include "Color.h"

class CTextLabel : public CBaseWidget {
public:
	CTextLabel(IWidget* parent, const char* name, const char* text = 0);
	~CTextLabel();

	void SetText(const char* text);
	const char* GetText();

	virtual void Draw();

	char* m_pszText;
};

#endif /* CTEXTLABEL_H_ */
