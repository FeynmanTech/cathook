/*
 * CBaseButton.h
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#ifndef CBASEBUTTON_H_
#define CBASEBUTTON_H_

#include "CTextLabel.h"

typedef void(ButtonCallback_t)(IWidget*);

class CBaseButton : public CTextLabel {
public:
	CBaseButton(IWidget* parent, const char* name);

	virtual void Draw();
	virtual void OnMousePress();
	virtual void Update();

	void SetCallback(ButtonCallback_t& callback);

	ButtonCallback_t* m_pCallback;
};

#endif /* CBASEBUTTON_H_ */
