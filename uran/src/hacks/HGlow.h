/*
 * HGlow.h
 *
 *  Created on: Oct 22, 2016
 *      Author: nullifiedcat
 */

#ifndef HGLOW_H_
#define HGLOW_H_

#include "IHack.h"

#include "../fixsdk.h"
#include "basehandle.h"

class CGlowObject {
	CBaseHandle handle;
	float r, g, b, a;
	unsigned char padding[0x20 - 0x14];
};

class CGlowManager {
public:
	CGlowObject objects[1];
};

class HGlow : public IHack {
public:
	void Create();
	bool CreateMove(void*, float, CUserCmd*);
	void Destroy();
	void PaintTraverse(void*, unsigned int, bool, bool);
	CGlowManager* m_pGlowManager;
};


#endif /* HGLOW_H_ */
