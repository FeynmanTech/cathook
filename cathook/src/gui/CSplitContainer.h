/*
 * CSplitContainer.h
 *
 *  Created on: Jan 28, 2017
 *      Author: nullifiedcat
 */

#ifndef CSPLITCONTAINER_H_
#define CSPLITCONTAINER_H_

#include "CBaseWindow.h"

class CSplitContainer : public CBaseWindow {
public:
	CSplitContainer(IWidget* parent, const char* name);

	virtual void Update();
};



#endif /* CSPLITCONTAINER_H_ */
