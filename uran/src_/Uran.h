/*
 * Uran.h
 *
 *  Created on: Aug 11, 2016
 *      Author: nullifiedcat
 */

#ifndef URAN_H_
#define URAN_H_

#include <stdio.h>
#include <pthread.h>

class SharedObject;
class ICenterPrint;
class ISteamClient017;
class ISteamFriends002;
class IVEngineClient013;
class IClientEntityList;
class ICvar;
typedef HSteamPipe int;
typedef HSteamUser int;
namespace vgui {
class ISurface;
class IPanel;
}

typedef void(__cdecl* MsgFn)(const char* msg, va_list);

namespace uran {

	FILE* file_log;

	SharedObject* so_steamclient;
	SharedObject* so_client;
	SharedObject* so_engine;
	SharedObject* so_vguimatsurface;
	SharedObject* so_vgui2;
	SharedObject* so_vstdlib;
	SharedObject* so_tier0;

	vgui::ISurface* i_Surface;
	vgui::IPanel* i_Panel;
	ICenterPrint* i_CenterPrint;
	ISteamClient017* i_SteamClient;
	ISteamFriends002* i_SteamFriends;
	IVEngineClient013* i_EngineClient;
	IClientEntityList* i_EntityList;
	ICvar* i_Cvar;

	//VMTHook* hk_CreateMove;
	//VMTHook* hk_PaintTraverse;

	HSteamPipe* p_SteamPipe;
	HSteamUser* p_SteamUser;
	MsgFn p_MsgFn;

	pthread_mutex_t mtx_Quit;
	pthread_t thr_Uran;
}

#endif /* URAN_H_ */
