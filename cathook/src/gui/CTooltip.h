/*
 * CTooltip.h
 *
 *  Created on: Jan 28, 2017
 *      Author: nullifiedcat
 */

#ifndef CTOOLTIP_H_
#define CTOOLTIP_H_

#include "CTextLabel.h"

class CTooltip : public CBaseWidget {
public:
	CTooltip(IWidget* parent, const char* name);
	void SetText(const char* text);

	virtual void Draw();
	virtual void Update();
	inline virtual bool ShouldResizeToFit() { return false; }
	inline virtual PositionMode GetPositionMode() { return PositionMode::FLOATING; }

	const char* m_pszText;
};

#endif /* CTOOLTIP_H_ */
