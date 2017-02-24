/*
 * hwid.cpp
 *
 *  Created on: Jan 22, 2017
 *      Author: nullifiedcat
 */

#include "hwid.h"
#include <stdio.h>

unsigned char hwid::up_machineid[16] = { 0 };
unsigned char hwid::up_fstab[11] = { 0 };
__u32 hwid::hwid_machine[4] = { 0 };
__u32 hwid::hwid_fstab[4] = { 0 };
__u32 hwid::hwid_result[8] = { 0 };

void hwid::unpack_strings() {
	for (int i = 0; i < 15; i++) hwid::up_machineid[i] = hwid::xorstring_machineid[i] ^ hwid::xorkey_machineid[i];
	for (int i = 0; i < 10; i++) hwid::up_fstab[i] = hwid::xorstring_fstab[i] ^ hwid::xorkey_fstab[i];
}

void hwid::read_hwid_machineid() {
	using namespace hwid;
	FILE* handle = fopen((const char*)up_machineid, "r");
	unsigned char nibbles[32];
	for (int i = 0; i < 32; i++) {
		char a = fgetc(handle);
		nibbles[i] = CHAR_TO_NIBBLE(a);
	}
	unsigned char bytes[16];
	for (int i = 0; i < 16; i++) {
		bytes[i] = (nibbles[i * 2] << 4 | nibbles[i * 2 + 1]);
	}
	for (int i = 0; i < 4; i++) {
		hwid_machine[i] = machineid_key[i] ^ (
			((bytes[i * 4] << 24)) |
			((bytes[i * 4 + 1] << 16)) |
			((bytes[i * 4 + 2] << 8)) |
			((bytes[i * 4 + 3])));
	}
	fclose(handle);
}

void hwid::read_hwid_fstab() {
	FILE* handle = fopen((const char*)up_fstab, "r");
	char buf[256];
	while (fgets(buf, 256, handle)) {
		if (buf[0] == 'U') {
			unsigned char nibbles[32] = { 0 };
			int nnibbles = 0;
			for (int i = 0; i < 32; i++) {
				char a = buf[5 + i];
				if (a == '-') continue;
				if (a == ' ') break;
				nibbles[nnibbles++] = CHAR_TO_NIBBLE(a);
			}
			unsigned char bytes[16];
			for (int i = 0; i < 16; i++) {
				bytes[i] = (nibbles[i * 2] << 4 | nibbles[i * 2 + 1]);
			}
			for (int i = 0; i < 4; i++) {
				hwid_fstab[i] = fstab_key[i] ^ (
					((bytes[i * 4] << 24)) |
					((bytes[i * 4 + 1] << 16)) |
					((bytes[i * 4 + 2] << 8)) |
					((bytes[i * 4 + 3])));
			}
			break;
		}
	}
}

void hwid::compute_result() {
	hwid_result[0] = (hwid_fstab[0] ^ hwid_machine[1]) - hwid_fstab[3];
	hwid_result[1] = hwid_fstab[3] % hwid_machine[3] + hwid_machine[1];
	hwid_result[2] = hwid_fstab[1] + hwid_machine[2];
	hwid_result[3] = hwid_fstab[2] ^ hwid_machine[1] * hwid_fstab[0];
	hwid_result[4] = hwid_fstab[2] ^ (hwid_machine[0] + hwid_fstab[2]);
	hwid_result[5] = hwid_fstab[3] % hwid_machine[0];
	hwid_result[6] = hwid_fstab[0] ^ (hwid_machine[2] + hwid_machine[0]);
	hwid_result[7] = hwid_fstab[1] * hwid_machine[1];
	for (int i = 0; i < 8; i++) hwid_result[i] ^= result_key[i];
}

#if USER_KEYS == true

bool hwid::check_hwid(__u64* user) {
	for (int i = 0; i < 4; i++) {
		if ((((__u64)hwid_result[i * 2] << 32 | (__u64)hwid_result[i * 2 + 1]) ^ user_key[i]) != user[i]) {
			return false;
		}
	}
	return true;
}

bool hwid::check_hwid(__u64 a, __u64 b, __u64 c, __u64 d) {
	__u64 user[4] = { a, b, c, d };
	return check_hwid(user);
}

void hwid::create_hwid(__u64* result, __u32* source) {
	for (int i = 0; i < 4; i++) {
		result[i] = (((__u64)source[i * 2] << 32 | (__u64)source[i * 2 + 1]) ^ user_key[i]);
	}
}

#endif
