/*
 * CCheckbox.h
 *
 *  Created on: Jan 26, 2017
 *      Author: nullifiedcat
 */

#ifndef CCHECKBOX_H_
#define CCHECKBOX_H_

#include "CBaseWidget.h"

class CCheckbox : public CBaseWidget {
public:
	CCheckbox(IWidget* parent, const char* name, bool checked = false);

	void SetWidth(int width);
	inline bool Value() { return m_bChecked; }

	virtual void OnMousePress();
	virtual void Draw();

	int m_nWidth;
	bool m_bChecked;
};



#endif /* CCHECKBOX_H_ */
