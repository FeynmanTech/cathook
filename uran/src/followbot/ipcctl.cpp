/*
 * ipcctl.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: nullifiedcat
 */

#include "ipcctl.h"

#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#include <ISteamFriends002.h>

#include "../logging.h"
#include "../interfaces.h"

bool ipcctl::Init() {
	key = 0x0D34DC47;
	logging::Info("sizeof layout: %i", sizeof(ipc_mem_layout));
	shmid = shmget(key, sizeof(ipc_mem_layout), 0666 | IPC_CREAT);
	if (shmid == -1) {
		logging::Info("shmget errored: %i", errno);
		return false;
	}
	mem = (ipc_mem_layout*)shmat(shmid, (void*)0, 0);
	if (mem == (ipc_mem_layout*)-1) {
		logging::Info("shmat errored: %i", errno);
		return false;
	}
	bot_id = NextBotID();
	memset(&mem->segments[bot_id], 0, sizeof(ipc_bot_seg));
	mem->segments[bot_id].initialized = true;
	mem->segments[bot_id].owner_pid = getpid();
	mem->segments[bot_id].command_number = 0;
	memcpy(mem->segments[bot_id].name, interfaces::steamFriends->GetPersonaName(), 32);
	if (bot_id == -1) return false;
	return true;
}

int ipcctl::NextBotID() {
	/* Segment 0 is reserved for broadcast commands */
	for (int i = 1; i < MAX_SEGS; i++) {
		ipc_bot_seg seg = mem->segments[i];
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

void ipcctl::WriteBotCommand(int bot_id, char* cmd) {
	if (bot_id < 0 || bot_id >= MAX_SEGS) return;
	char buffer[CMD_LENGTH];
	snprintf(buffer, CMD_LENGTH, "%s", cmd);
	logging::Info("Executing command `%s`, #CMD %i", cmd, mem->segments[bot_id].command_number);
	mem->segments[bot_id].command_number++;
	memcpy(mem->segments[bot_id].command_buffer, buffer, sizeof(char) * CMD_LENGTH);
}

ipc_bot_seg* ipcctl::GetBotCommand(int bot_id) {
	if (bot_id < 0 || bot_id >= MAX_SEGS) return (ipc_bot_seg*)0;
	if (bot_id != 0 && mem->segments[bot_id].owner_pid != getpid()) {
		logging::Info("Warning: Shared memory violation! BotID: %i ; PID: %i ; OWNER: %i", bot_id, getpid(), mem->segments[bot_id].owner_pid);
		this->bot_id = NextBotID();
		if (bot_id == -1) {
			logging::Info("FAILED TO ASSIGN NEW BOT ID !!!");
			return 0;
		}
		logging::Info("New BotID: %i", bot_id);
		memset(&mem->segments[bot_id], 0, sizeof(ipc_bot_seg));
		mem->segments[bot_id].initialized = true;
		mem->segments[bot_id].owner_pid = getpid();
		mem->segments[bot_id].command_number = 0;
		memcpy(mem->segments[bot_id].name, interfaces::steamFriends->GetPersonaName(), 32);
		return 0;
	}
	mem->segments[bot_id].last_access_time = time(0);
	return &mem->segments[bot_id];
}

bool ipcctl::Detach() {
	if (mem != (ipc_mem_layout*)-1) {
		shmdt(mem);
	}
	return true;
}
