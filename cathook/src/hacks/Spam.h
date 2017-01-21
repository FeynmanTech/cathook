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

class Spam : public IHack {
public:
	DECLARE_HACK_METHODS();
	Spam();
	void LoadFile();
	CatVar* v_bSpam;
	CatVar* v_sSpamFile;
	CatVar* v_bSpamNewlines;
	ConCommand* c_Reload;
	char m_Spam[SPAM_MAX_AMOUNT][SPAM_MAX_LENGTH];
	int m_nSpam;
	int m_iCurrentIndex;
	float m_fLastSpam;
};

DECLARE_HACK_SINGLETON(Spam);

void CC_Spam_ReloadFile(const CCommand& args);

#endif /* HACKS_SPAM_H_ */
