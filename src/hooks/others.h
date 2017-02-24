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
class SDL_Window;

bool CanPacket_hook(void*);
int IN_KeyEvent_hook(void*, int, int, const char*);
bool SendNetMsg_hook(void*, INetMessage&, bool, bool);
void Shutdown_hook(void*, const char*);
void OverrideView_hook(void*, CViewSetup*);
bool DispatchUserMessage_hook(void*, int, bf_read&);
void FrameStageNotify_hook(void*, int);
void LevelInit_hook(void*, const char*);
void LevelShutdown_hook(void*);
void SwapWindow_hook(SDL_Window* window);

//extern unsigned int* swapwindow_ptr;
//extern unsigned int  swapwindow_orig;

#endif /* OTHERS_H_ */
