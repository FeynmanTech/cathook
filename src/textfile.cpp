/*
 * textfile.cpp
 *
 *  Created on: Jan 24, 2017
 *      Author: nullifiedcat
 */

#include "textfile.h"
#include "common.h"

#include <pwd.h>
#include <stdio.h>

TextFile::TextFile(unsigned length, unsigned lines) {
	m_nCount = lines;
	m_nLength = length;
	m_Buffer = new char[length * lines];
}

TextFile::~TextFile() {
	delete [] m_Buffer;
}

void TextFile::LoadFile(const char* name) {
	uid_t uid = geteuid();
	passwd* pw = getpwuid(uid);
	if (!pw) {
		logging::Info("can't get the username!");
		return;
	}
	char* filename = strfmt("/home/%s/.cathook/%s", pw->pw_name, name);
	FILE* file = fopen(filename, "r");
	if (!file) {
		logging::Info("Could not open the file: %s", filename);
		delete filename;
		return;
	}
	delete filename;
	char* buffer = new char[m_nLength];
	for (m_nCount = 0; m_nCount < m_nLines; m_nCount++) {
		if(fgets(buffer, m_nLength - 1, file)) {
			if (buffer[strlen(buffer) - 1] == '\n')
				buffer[strlen(buffer) - 1] = '\0';
			strncpy(&m_Buffer[m_nCount * m_nLength], buffer, m_nLength - 1);
		} else break;
	}
	delete [] buffer;
}

const char* TextFile::GetLine(unsigned line) {
	if (line < 0 || line >= m_nCount) return 0;
	return (const char*)&m_Buffer[line * m_nLength];
}

unsigned TextFile::GetLineCount() {
	return m_nCount;
}
