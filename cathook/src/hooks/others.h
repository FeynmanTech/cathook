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

typedef bool(CanPacket_t)(void*);
typedef int(IN_KeyEvent_t)(void*, int, int, const char*);
typedef bool(SendNetMsg_t)(void*, INetMessage&, bool, bool);
typedef void(Shutdown_t)(void*, const char*);
typedef void(OverrideView_t)(void*, CViewSetup*);

bool CanPacket_hook(void*);
int IN_KeyEvent_hook(void*, int, int, const char*);
bool SendNetMsg_hook(void*, INetMessage&, bool, bool);
void Shutdown_hook(void*, const char*);
void OverrideView_hook(void*, CViewSetup*);

#endif /* OTHERS_H_ */
