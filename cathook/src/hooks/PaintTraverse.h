/*
 * PaintTraverse.h
 *
 *  Created on: Jan 8, 2017
 *      Author: nullifiedcat
 */

#ifndef PAINTTRAVERSE_H_
#define PAINTTRAVERSE_H_

typedef void(PaintTraverse_t)(void*, unsigned int, bool, bool);
void PaintTraverse_hook(void*, unsigned int, bool, bool);

#endif /* PAINTTRAVERSE_H_ */
