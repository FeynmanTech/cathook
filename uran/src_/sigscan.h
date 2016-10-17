/*
 * sigscan.h
 *
 *  Created on: Aug 13, 2016
 *      Author: nullifiedcat
 */

#ifndef SIGSCAN_H_
#define SIGSCAN_H_

// Shamelessly copied code
#define INRANGE( x, a, b ) (x >= a && x <= b)
#define GETBITS( x ) (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define GETBYTE( x ) (GETBITS(x[0]) << 4 | GETBITS(x[1]))

namespace uran {

uintptr_t SearchMemory(uintptr_t start, uintptr_t size, const char* pattern) {
	const char* current = pattern;
	uintptr_t match = 0;
	for (uintptr_t addr = start; addr < start + size; addr++) {
		if (!*current) return match;
		if (*(uintptr_t*) current == '?' || *(uintptr_t*) addr == GETBYTE(current)) {
			if (!match) match = addr;
			if (!current[2]) return match;
			if (*(uintptr_t*) current == '? ') current += 2;
			else current += 3;
		} else {
			current = pattern;
			match = 0;
		}
	}
	return 0;
}

}

#endif /* SIGSCAN_H_ */
