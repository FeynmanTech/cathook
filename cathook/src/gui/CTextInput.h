/*
 * CTextInput.h
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#ifndef CTEXTINPUT_H_
#define CTEXTINPUT_H_

#include "CBaseWidget.h"

typedef void(TextInputChangeCallback_t)(IWidget* thisptr, const char* oldText, const char* newText);

class CTextInput : public CBaseWidget {
public:
	CTextInput(IWidget* parent, const char* name);

	virtual void OnKeyPress(ButtonCode_t key);
	virtual void Draw();
	virtual bool ConsumesKey(ButtonCode_t key);

	void PutChar(char ch);
	void SetLength(int newlength);
	void SetMaxWidth(int width);
	void SetCallback(TextInputChangeCallback_t* callback);

	TextInputChangeCallback_t* m_pCallback;
	char* m_pszContents;
	int m_nLength;
};

#endif /* CTEXTINPUT_H_ */
