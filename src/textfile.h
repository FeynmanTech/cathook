/*
 * textfile.h
 *
 *  Created on: Jan 24, 2017
 *      Author: nullifiedcat
 */

#ifndef TEXTFILE_H_
#define TEXTFILE_H_

class TextFile {
public:
	TextFile(unsigned length, unsigned lines);
	~TextFile();
	void LoadFile(const char* name);
	unsigned GetLineCount();
	const char* GetLine(unsigned line);

	unsigned m_nLength;
	unsigned m_nLines;
	unsigned m_nCount;
	const char* m_pszFilename;
	char* m_Buffer;
};

#endif /* TEXTFILE_H_ */
