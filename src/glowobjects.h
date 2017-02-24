/*
 * glowobjects.h
 *
 *  Created on: Jan 16, 2017
 *      Author: nullifiedcat
 */

#ifndef GLOWOBJECTS_H_
#define GLOWOBJECTS_H_

#define END_OF_FREE_LIST -1
#define ENTRY_IN_USE -2

#include "fixsdk.h"
#include <tier1/utlvector.h>
#include <mathlib/vector.h>
#include <basehandle.h>

//template<typename T> class CUtlVector;

struct GlowObjectDefinition_t {
    CBaseHandle m_hEntity;
    Vector m_vGlowColor;
    float m_flGlowAlpha;
    bool m_bRenderWhenOccluded;
    bool m_bRenderWhenUnoccluded;
    int m_nSplitScreenSlot;
    int m_nNextFreeSlot;
};

class CGlowObjectManager {
public:
	CUtlVector<GlowObjectDefinition_t> m_GlowObjectDefinitions;
	int m_nFirstFreeSlot;
};

extern CGlowObjectManager* g_GlowObjectManager;

#endif /* GLOWOBJECTS_H_ */
