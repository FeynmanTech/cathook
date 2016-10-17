/*
 * HTesting.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#include "HSaySpecial.h"

#include <string>
#include <algorithm>

#include "../logging.h"
#include "../usercmd.h"
#include "../interfaces.h"
#include "../helpers.h"

#include "../fixsdk.h"
#include <cdll_int.h>
#include <tier1/convar.h>

void CC_SayLines(const CCommand& args) {
	char cmd[256];
	sprintf(cmd, "say %s", args.ArgS());
	std::string str(cmd);
	std::replace(str.begin(), str.end(), '^', '\n');
	interfaces::engineClient->ServerCmd(str.c_str());
}

void HSaySpecial::Create() {
	logging::Info("Setting up command");
	this->c_SayLine = CreateConCommand("u_say_lines", CC_SayLines, "Uses ^ as a newline character");
}

bool HSaySpecial::CreateMove(void*, float, CUserCmd*) { return true; }
void HSaySpecial::Destroy() {}
void HSaySpecial::PaintTraverse(void*, unsigned int, bool, bool) {};
