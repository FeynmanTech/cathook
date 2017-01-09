/*
 * others.h
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#ifndef OTHERS_H_
#define OTHERS_H_

class INetMessage;
class CViewSetup;
class bf_read;

typedef bool(CanPacket_t)(void*);
typedef int(IN_KeyEvent_t)(void*, int, int, const char*);
typedef bool(SendNetMsg_t)(void*, INetMessage&, bool, bool);
typedef void(Shutdown_t)(void*, const char*);
typedef void(OverrideView_t)(void*, CViewSetup*);
typedef bool(DispatchUserMessage_t)(void*, int, bf_read&);
typedef void(FrameStageNotify_t)(void*, int);

bool CanPacket_hook(void*);
int IN_KeyEvent_hook(void*, int, int, const char*);
bool SendNetMsg_hook(void*, INetMessage&, bool, bool);
void Shutdown_hook(void*, const char*);
void OverrideView_hook(void*, CViewSetup*);
bool DispatchUserMessage_hook(void*, int, bf_read&);
void FrameStageNotify_hook(void*, int);

#endif /* OTHERS_H_ */
