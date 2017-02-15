/*
 * Spam.h
 *
 *  Created on: Jan 21, 2017
 *      Author: nullifiedcat
 */

#ifndef HACKS_SPAM_H_
#define HACKS_SPAM_H_

#include "../textfile.h"

namespace hacks { namespace shared { namespace spam {

extern CatVar enabled;
extern CatVar filename;
extern CatVar newlines;
extern CatCommand reload;

extern TextFile file;
extern size_t index;

void ProcessUserCmd(CUserCmd*);
void Reset();

}}}

#endif /* HACKS_SPAM_H_ */
