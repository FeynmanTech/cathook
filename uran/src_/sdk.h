/*
 * sdk.h
 *
 *  Created on: Oct 3, 2016
 *      Author: nullifiedcat
 */

#ifndef SDK_H_
#define SDK_H_

class CUserCmd
{
public:
	virtual ~CUserCmd(){};
	int command_number;
	int tick_count;
	Vector viewangles;
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	uint8_t impulse;
	int weaponselect;
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
};



#endif /* SDK_H_ */
