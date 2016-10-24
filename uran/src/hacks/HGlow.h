/*
 * HGlow.h
 *
 *  Created on: Oct 22, 2016
 *      Author: nullifiedcat
 */

#ifndef HGLOW_H_
#define HGLOW_H_

#include "IHack.h"

class HGlow : public IHack {
public:
	void Create();
	bool CreateMove(void*, float, CUserCmd*);
	void Destroy();
	void PaintTraverse(void*, unsigned int, bool, bool);
};


#endif /* HGLOW_H_ */
