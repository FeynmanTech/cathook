/*
 * interfaces.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#include "interfaces.h"
#include "sharedobj.h"
#include "logging.h"
#include "copypasted/CSignature.h"

#include <ISteamClient017.h>

//class ISteamFriends002;

ISteamClient017* interfaces::steamClient = 0;
ISteamFriends002* interfaces::steamFriends = 0;
IVEngineClient013* interfaces::engineClient = 0;
vgui::ISurface* interfaces::surface = 0;
vgui::IPanel* interfaces::panel = 0;
IClientEntityList* interfaces::entityList = 0;
ICenterPrint* interfaces::centerPrint = 0;
IGameEventManager2* interfaces::eventManager = 0;
IBaseClientDLL* interfaces::baseClient = 0;
IEngineTrace* interfaces::trace = 0;
IVModelInfoClient* interfaces::model = 0;
IInputSystem* interfaces::input = 0;
ICvar* interfaces::cvar = 0;
CGlobalVarsBase* interfaces::gvars = 0;
IPrediction* interfaces::prediction = 0;
IGameMovement* interfaces::gamemovement = 0;
IInput* interfaces::iinput = 0;
IMatSystemSurface* interfaces::matsurface = 0;

void interfaces::CreateInterfaces() {
	interfaces::centerPrint = reinterpret_cast<ICenterPrint*>(sharedobj::client->fptr("VCENTERPRINT002", nullptr));
	interfaces::cvar = reinterpret_cast<ICvar*>(sharedobj::vstdlib->CreateInterface("VEngineCvar004"));
	interfaces::engineClient = reinterpret_cast<IVEngineClient013*>(sharedobj::engine->CreateInterface("VEngineClient013"));
	interfaces::entityList = reinterpret_cast<IClientEntityList*>(sharedobj::client->CreateInterface("VClientEntityList003"));
	interfaces::panel = reinterpret_cast<vgui::IPanel*>(sharedobj::vgui2->CreateInterface("VGUI_Panel009"));
	interfaces::steamClient = reinterpret_cast<ISteamClient017*>(sharedobj::steamclient->CreateInterface("SteamClient017"));
	interfaces::surface = reinterpret_cast<vgui::ISurface*>(sharedobj::vguimatsurface->CreateInterface("VGUI_Surface030"));
	interfaces::eventManager = reinterpret_cast<IGameEventManager2*>(sharedobj::engine->CreateInterface("GAMEEVENTSMANAGER002"));
	interfaces::baseClient = reinterpret_cast<IBaseClientDLL*>(sharedobj::client->CreateInterface("VClient017"));
	interfaces::trace = reinterpret_cast<IEngineTrace*>(sharedobj::engine->CreateInterface("EngineTraceClient003"));
	interfaces::model = reinterpret_cast<IVModelInfoClient*>(sharedobj::engine->CreateInterface("VModelInfoClient006"));
	interfaces::input = reinterpret_cast<IInputSystem*>(sharedobj::inputsystem->CreateInterface("InputSystemVersion001"));
	//interfaces::client = reinterpret_cast<IClient*>(sharedobj::client->CreateInterface("VClient017"));
	HSteamPipe sp = interfaces::steamClient->CreateSteamPipe();
	HSteamUser su = interfaces::steamClient->ConnectToGlobalUser(sp);
	interfaces::steamFriends = reinterpret_cast<ISteamFriends002*>(interfaces::steamClient->GetISteamFriends(su, sp, "SteamFriends002"));
	//uintptr_t hudupdate = (uintptr_t)hooks::GetVMT(interfaces::baseClient, 0)[11];
	//interfaces::gvars = *reinterpret_cast<CGlobalVarsBase**>(gSignatures.GetClientSignature("55 89 E5 53 83 EC ? A1 ? ? ? ? 0F B6 5D 0C F3 0F 10 40 10") + 8);
	interfaces::gvars = **(reinterpret_cast<CGlobalVarsBase***>((uintptr_t)11 + gSignatures.GetClientSignature("55 89 E5 83 EC ? 8B 45 08 8B 15 ? ? ? ? F3 0F 10")));
	//interfaces::gvars = *reinterpret_cast<CGlobalVarsBase**>(hudupdate + 13 + *reinterpret_cast<uint32_t*>(hudupdate + 13 + 3) + 7);
	interfaces::prediction = reinterpret_cast<IPrediction*>(sharedobj::client->CreateInterface("VClientPrediction001"));
	interfaces::gamemovement = reinterpret_cast<IGameMovement*>(sharedobj::client->CreateInterface("GameMovement001"));
	interfaces::iinput = **(reinterpret_cast<IInput***>((uintptr_t)1 + gSignatures.GetClientSignature("A1 ? ? ? ? C6 05 ? ? ? ? 01 8B 10 89 04 24 FF 92 B4 00 00 00 A1 ? ? ? ? 8B 10")));
	interfaces::matsurface = **reinterpret_cast<IMatSystemSurface***>((uintptr_t)19 + gSignatures.GetClientSignature("FF 92 94 02 00 00 8B 8D C4 FE FF FF 89 85 B0 FE FF FF A1 ? ? ? ? 8B 10 89 4C 24 0C"));

}
