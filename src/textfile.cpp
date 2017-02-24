/*
 * textfile.cpp
 *
 *  Created on: Jan 24, 2017
 *      Author: nullifiedcat
 */

#include "textfile.h"

#include "beforecheaders.h"
#include <fstream>
#include "aftercheaders.h"

#include <pwd.h>
#include <stdio.h>

void TextFile::LoadFile(const char* name) {
	uid_t uid = geteuid();
	passwd* pw = getpwuid(uid);
	if (!pw) {
		logging::Info("can't get the username!");
		return;
	}
	std::string filename = format("/home/", pw->pw_name, "/.cathook/", name);
	std::ifstream file(filename, std::ios::in);
	if (file.bad()) {
		logging::Info("Could not open the file: %s", filename);
		return;
	}
	lines.clear();
	for (std::string line; std::getline(file, line);) {
		lines.push_back(line);
	}
}

size_t TextFile::LineCount() const {
	return lines.size();
}

const std::string& TextFile::Line(size_t id) const {
	return lines.at(id);
}
