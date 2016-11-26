/*
 * ipcctl.h
 *
 *  Created on: Nov 23, 2016
 *      Author: nullifiedcat
 */

#ifndef IPCCTL_H_
#define IPCCTL_H_

#include <sys/types.h>

#define MAX_SEGS 16
#define CMD_LENGTH 256
#define ACCESS_TIMEOUT 10000
#define NAME_LENGTH 32

typedef struct ipc_bot_seg {
	bool initialized;
	time_t last_access_time;
	pid_t owner_pid;
	unsigned command_number;
	char name[NAME_LENGTH];
	char command_buffer[CMD_LENGTH];
} ipc_bot_seg;

typedef struct ipc_mem_layout {
	/* 0: Global commands; 1-15: Bot-Specific commands */
	ipc_bot_seg segments[MAX_SEGS];
} ipc_mem_layout;

class ipcctl {
public:
	int key;
	int shmid;
	ipc_mem_layout* mem;
	int bot_id;

	bool Init();
	void WriteBotCommand(int bot_id, char* cmd);
	ipc_bot_seg* GetBotCommand(int bot_id);
	int NextBotID();
	bool Detach();
};

#endif /* IPCCTL_H_ */
