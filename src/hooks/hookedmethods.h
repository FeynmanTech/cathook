/*
 * hooks.h
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#ifndef HOOKEDMETHODS_H_
#define HOOKEDMETHODS_H_

class IStudioRender;
class CUserCmd;
class INetMessage;
class bf_read;
class CViewSetup;
class IClientEntity;

typedef bool(CreateMove_t)(void*, float, CUserCmd*);
typedef void(PaintTraverse_t)(void*, unsigned int, bool, bool);
typedef bool(CanPacket_t)(void*);
typedef int(IN_KeyEvent_t)(void*, int, int, const char*);
typedef bool(SendNetMsg_t)(void*, INetMessage&, bool, bool);
typedef void(Shutdown_t)(void*, const char*);
typedef void(OverrideView_t)(void*, CViewSetup*);
typedef bool(DispatchUserMessage_t)(void*, int, bf_read&);
typedef void(FrameStageNotify_t)(void*, int);
typedef void(LevelInit_t)(void*, const char*);
typedef void(LevelShutdown_t)(void*);
typedef void(BeginFrame_t)(IStudioRender*);
typedef bool(*CanInspect_t)(IClientEntity*);
//typedef void(*CInput__CreateMove_t)(void*, int, float, bool);
//void CInput__CreateMove_hook(void*, int sequence_number, float input_sample_frametime, bool active);
bool CanInspect_hook(IClientEntity*);
const unsigned int offCanInspect = 512;
void BeginFrame_hook(IStudioRender*);

#include "CreateMove.h"
#include "PaintTraverse.h"
#include "others.h"

#endif /* HOOKEDMETHODS_H_ */
