/*
 * criticals.h
 *
 *  Created on: Feb 15, 2017
 *      Author: nullifiedcat
 */

#ifndef HACKS_CRITICALS_H_
#define HACKS_CRITICALS_H_

namespace hacks { namespace tf { namespace criticals {

extern CatVar forced;
extern CatVar suppress;
extern CatVar show_info;

bool CurrentShotACrit();
bool AllowAttacking();

}}}

#endif /* HACKS_CRITICALS_H_ */
