/*
 * Spam.h
 *
 *  Created on: Jan 21, 2017
 *      Author: nullifiedcat
 */

#ifndef HACKS_SPAM_H_
#define HACKS_SPAM_H_

#include "IHack.h"

#define SPAM_MAX_AMOUNT 64
#define SPAM_MAX_LENGTH 256

class TextFile;

class Spam : public IHack {
public:
	DECLARE_HACK_METHODS();
	Spam();
	void Reload();
	CatVar* v_bSpam;
	CatVar* v_sSpamFile;
	CatVar* v_bSpamNewlines;
	ConCommand* c_Reload;
	int m_iCurrentIndex;
	TextFile* m_TextFile;
	float m_fLastSpam;
};

DECLARE_HACK_SINGLETON(Spam);

void CC_Spam_ReloadFile(const CCommand& args);

#endif /* HACKS_SPAM_H_ */
