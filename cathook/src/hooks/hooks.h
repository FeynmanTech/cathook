/*
 * hooks.h
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#ifndef HOOKS_H_
#define HOOKS_H_

typedef bool(CreateMove_t)(void*, float, CUserCmd*);
typedef void(PaintTraverse_t)(void*, unsigned int, bool, bool);
typedef bool(CanPacket_t)(void*);
typedef int(IN_KeyEvent_t)(void*, int, int, const char*);
typedef bool(SendNetMsg_t)(void*, INetMessage&, bool, bool);
typedef void(Shutdown_t)(void*, const char*);
typedef void(OverrideView_t)(void*, CViewSetup*);
typedef bool(DispatchUserMessage_t)(void*, int, bf_read&);
typedef void(FrameStageNotify_t)(void*, int);

#include "CreateMove.h"
#include "PaintTraverse.h"
#include "others.h"

#endif /* HOOKS_H_ */
