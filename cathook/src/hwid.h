/*
 * hwid.h
 *
 *  Created on: Jan 22, 2017
 *      Author: nullifiedcat
 */

#ifndef HWID_H_
#define HWID_H_

#include <asm/types.h>

#define USER_KEYS true

#define CHAR_TO_NIBBLE(x) \
	(x >= 'a' && x <= 'f') ? (x - 'a' + 0xA) : ((x >= '0' && x <= '9') ? (x - '0') : (0))

namespace hwid {

const __u32 machineid_key[] = {
	0x2ea03063, 0x20a52253,
	0x0e086f22, 0x3e3bc4db };

const __u32 fstab_key[] = {
	0xAC5D8791, 0x6EFD749C,
	0x5DEC7286, 0xE0A56284 };

const __u32 result_key[] = {
	0xC89207A6, 0x0A5F9418, 0x376B9DC8, 0xEC201BFD,
	0x5A937048, 0xC89A3DBE, 0xB570C6E8, 0xC314827F
};

#if USER_KEYS == true
const __u64 user_key[] = {
	0x5DA1379C9AFC762Ellu, 0xEB584AC0EAD17528llu,
	0x847A291317E5F2C8llu, 0x1EDA6F907ADC52B1llu };
#endif

const volatile unsigned char xorstring_machineid[] = { 0x2E, 0xAF, 0x67, 0x97, 0xE2, 0x40, 0xD1, 0xC4, 0xA5, 0x58, 0x9C, 0x45, 0x76, 0x48, 0x5F };
const volatile unsigned char xorstring_fstab[] = { 0x7C, 0xD4, 0x69, 0xFB, 0x15, 0xB6, 0x02, 0x31, 0xA1, 0x50 };
const volatile unsigned char xorkey_machineid[] = { 0x01, 0xca, 0x13, 0xf4, 0xcd, 0x2d, 0xb0, 0xa7, 0xcd, 0x31, 0xf2, 0x20, 0x5b, 0x21, 0x3b };
const volatile unsigned char xorkey_fstab[] = { 0x53, 0xb1, 0x1d, 0x98, 0x3a, 0xd0, 0x71, 0x45, 0xc0, 0x32 };

extern unsigned char up_machineid[16];
extern unsigned char up_fstab[11];

extern __u32 hwid_machine[4];
extern __u32 hwid_fstab[4];
extern __u32 hwid_result[8];

void unpack_strings();
void read_hwid_machineid();
void read_hwid_fstab();
void compute_result();
#if USER_KEYS == true
bool check_hwid(__u64*);
bool check_hwid(__u64, __u64, __u64, __u64);
void create_hwid(__u64*, __u32*);
#endif
}


#endif /* HWID_H_ */
