/*
 * ipcctl.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: nullifiedcat
 */

#include "../ipc/ipcctl.h"

#include <unistd.h>
#include <time.h>
#include <string.h>
#include <steam/isteamfriends.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h>
#include <signal.h>

#include "../logging.h"
#include "../interfaces.h"

bool ipcctl::Init() {
	broken = true;
	key = 0x0D34DC47;
	logging::Info("IPC Total Size: %i", sizeof(ipc_mem_layout));
	shmid = shmget(key, sizeof(ipc_mem_layout), 0666 | IPC_CREAT);
	if (shmid == -1) {
		logging::Info("shmget error: %i", errno);
		return false;
	}
	mem = (ipc_mem_layout*)shmat(shmid, (void*)0, 0);
	if (mem == (ipc_mem_layout*)-1) {
		logging::Info("shmat error: %i", errno);
		return false;
	}
	client_id = NextClientID();
	memset(&mem->segments[client_id], 0, sizeof(ipc_client_seg));
	mem->segments[client_id].initialized = true;
	mem->segments[client_id].owner_pid = getpid();
	mem->segments[client_id].command_number = 0;
	memcpy(mem->segments[client_id].name, g_ISteamFriends->GetPersonaName(), 32);
	if (client_id == -1) return false;
	broken = false;
	return true;
}

int ipcctl::NextClientID() {
	/* Segment 0 is reserved for broadcast commands */
	for (int i = 1; i < MAX_SEGS; i++) {
		ipc_client_seg seg = mem->segments[i];
		if (seg.initialized) {
			logging::Info("Segment %i is owned by `%s`, PID %i", i, seg.name, seg.owner_pid);
			if (kill(seg.owner_pid, 0) == -1 && errno == ESRCH) {
				logging::Info("Dead process: %i", seg.owner_pid);
				return i;
			}
			if ((time(0) - seg.last_access_time) > ACCESS_TIMEOUT) {
				logging::Info("Segment timeout: %i", i);
				return i;
			}
		} else {
			return i;
		}
	}
	logging::Info("Ran out of segments for bots.");
	return -1;
}

void ipcctl::SetCommand(int client, char* cmd) {
	if (broken) return;
	if (client < 0 || client >= MAX_SEGS) return;
	char buffer[CMD_LENGTH];
	snprintf(buffer, CMD_LENGTH, "%s", cmd);
	logging::Info("Executing command `%s`, #CMD %i", cmd, mem->segments[client].command_number);
	mem->segments[client].command_number++;
	memcpy(mem->segments[client].command_buffer, buffer, sizeof(char) * CMD_LENGTH);
}

ipc_client_seg* ipcctl::GetClientSegment(int client) {
	if (broken) return (ipc_client_seg*)0;
	if (client < 0 || client >= MAX_SEGS) return (ipc_client_seg*)0;
	if (client != 0 && mem->segments[client].owner_pid != getpid()) {
		logging::Info("Warning: Shared memory violation! BotID: %i ; PID: %i ; OWNER: %i", client, getpid(), mem->segments[client].owner_pid);
		this->client_id = NextClientID();
		if (client == -1) {
			logging::Info("FAILED TO ASSIGN NEW BOT ID !!!");
			return 0;
		}
		logging::Info("New BotID: %i", client);
		memset(&mem->segments[client], 0, sizeof(ipc_client_seg));
		mem->segments[client].initialized = true;
		mem->segments[client].owner_pid = getpid();
		mem->segments[client].command_number = 0;
		memcpy(mem->segments[client].name, g_ISteamFriends->GetPersonaName(), 32);
		return 0;
	}
	mem->segments[client].last_access_time = time(0);
	return &mem->segments[client];
}

bool ipcctl::Detach() {
	if (broken) return true;
	if (mem != (ipc_mem_layout*)-1) {
		shmdt(mem);
	}
	return true;
}
