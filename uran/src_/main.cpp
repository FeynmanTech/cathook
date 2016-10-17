/*#include "stdheader.h"

#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <link.h>

#include <ISteamClient017.h>
#include <cdll_int.h>
#include <iconvar.h>
#include "Log.h"
#include "Uran.h"
#include <Color.h>
#include "draw.h"
#include "tier1/convar.h"
#include "hooks.h"
#include "mem/vtable.h"
#include "SharedObject.h"*/

//(void)(static const char* hoi_valve = "hOI! if you are valve employee - dont ban pls. xoxo");

void wait_for_so() {
	/*hack::Log("Waiting for shared objects...");
	uran::so_client = (new SharedObject(DIR_TF_BIN "client.so"))->Load()->SetupFactory();
	uran::so_engine = (new SharedObject(DIR_TF2_BIN "engine.so"))->Load()->SetupFactory();
	uran::so_steamclient = (new SharedObject(DIR_STEAM_LINUX32 "steamclient.so"))->Load()->SetupFactory();
	uran::so_vgui2 = (new SharedObject(DIR_TF2_BIN "vgui2.so"))->Load()->SetupFactory();
	uran::so_vguimatsurface = (new SharedObject(DIR_TF2_BIN "vguimatsurface.so"))->Load()->SetupFactory();
	uran::so_vstdlib = (new SharedObject(DIR_TF2_BIN "libvstdlib.so"))->Load()->SetupFactory();
	uran::so_tier0 = new SharedObject(DIR_TF2_BIN "libtier0.so");
	hack::Log("Got all shared objects...");*/
}

void create_interfaces() {
	/*hack::Log("Creating interfaces...");
	/*uran::so_client->InitInterfaceFactory();
	uran::so_engine->InitInterfaceFactory();
	uran::so_steamclient->InitInterfaceFactory();
	uran::so_vgui2->InitInterfaceFactory();
	uran::so_vguimatsurface->InitInterfaceFactory();
	uran::so_vstdlib->InitInterfaceFactory();*//*
	hack::Log("Interface factories created...");
	uran::p_MsgFn = (MsgFn)dlsym(uran::so_tier0->lmap, "Msg");
	uran::i_SteamClient = uran::so_steamclient->CreateInterface<ISteamClient017*>("SteamClient017");
	HSteamPipe sp = uran::i_SteamClient->CreateSteamPipe();
	uran::p_SteamPipe = &sp;
	HSteamUser su = uran::i_SteamClient->ConnectToGlobalUser(*uran::p_SteamPipe);
	uran::p_SteamUser = &su;
	uran::i_SteamFriends = reinterpret_cast<ISteamFriends002*>(uran::i_SteamClient->GetISteamFriends(*uran::p_SteamUser, *uran::p_SteamPipe, "SteamFriends002"));;
	uran::i_EngineClient = uran::so_engine->CreateInterface<IVEngineClient013*>("VEngineClient013");
	uran::i_Surface = uran::so_vguimatsurface->CreateInterface<vgui::ISurface*>("VGUI_Surface030");
	uran::i_Panel = uran::so_vgui2->CreateInterface<vgui::IPanel*>("VGUI_Panel009");
	uran::i_EntityList = uran::so_client->CreateInterface<IClientEntityList*>("VClientEntityList003");
	uran::i_CenterPrint = uran::so_client->CreateInterface<ICenterPrint*>("VCENTERPRINT002");
	uran::i_Cvar = uran::so_vstdlib->CreateInterface<ICvar*>("VEngineCvar004");
	hack::Log("Interfaces created");*/
}

//bool thread_need_quit(pthread_mutex_t* quit_mutex) {
	/*switch (pthread_mutex_trylock(quit_mutex)) {
	case 0:
		pthread_mutex_unlock(quit_mutex);
		return true;
	case EBUSY:
		return false;
	}
	return true;*/
//}

void* uran_thread(void* arg) {
	/*pthread_mutex_t* quit_mutex = (pthread_mutex_t*)arg;
	wait_for_so();
	create_interfaces();
	hack::Log("Hooking CreateMove...");
	//uran::hk_CreateMove = new uran::VMTHook((void*)0);
	hack::Log("Hooking PaintTraverse...");
	hack::Log("It works! Name: %s", uran::i_SteamFriends->GetPersonaName());
	uran::i_Cvar->ConsolePrintf("Test console printf??? ok?\n");
	hack::Log("Creating convar...");
	//ConVar* cvar_uran_test = new ConVar("uran_test", "0", 0);
	hack::Log("Registering it...");
	//uran::i_Cvar->RegisterConCommand(cvar_uran_test);
	while(!thread_need_quit(quit_mutex)) {
		try {

			/*uran::i_CenterPrint->ColorPrint(123, 55, 123, 255, "AYY LMAO");
			ConVar* var = uran::i_Cvar->FindVar("sv_cheats");
			uran::Log("sv_cheats: %i", var->GetInt());*/
			//uran::i_EngineClient->ServerCmd("say .\n\n\n\n\n\n\n\nAYY LMAO\nY\nY\n \nL\nM\nA\nO", true);
			/*sleep(1);
		} catch (std::exception& ex) {
			// error
		}*/
	//}
	//edmp.close();
	/*Log("Exiting thread...");*/
	return 0;
}

void __attribute__((constructor)) startup(void) {
	/*hack::LogInit();
	hack::Log("Loading! BUILD: " __DATE__ " " __TIME__);
	pthread_mutex_init(&uran::mtx_Quit, 0);
	pthread_mutex_lock(&uran::mtx_Quit);
	pthread_create(&uran::thr_Uran, 0, uran_thread, &uran::mtx_Quit);*/
}

void __attribute__((destructor)) shutdown(void) {
	/*pthread_mutex_unlock(&uran::mtx_Quit);
	pthread_join(uran::thr_Uran, 0);
	/* std::remove("/var/lock/uranso.lock"); */
	/*hack::Log("Goodbye.");
	hack::LogClose();*/
	//closelog();
}
