/*
 * CCVarContainer.h
 *
 *  Created on: Jan 28, 2017
 *      Author: nullifiedcat
 */

#ifndef CCVARCONTAINER_H_
#define CCVARCONTAINER_H_

#include "CSplitContainer.h"

class CatVar;

class CCvarContainer : public CSplitContainer {
public:
	CCvarContainer(IWidget* parent, const char* name, CatVar* var);

	IWidget* m_pControl;
	IWidget* m_pLabel;

	CatVar* m_pVar;
};

#endif /* CCVARCONTAINER_H_ */
