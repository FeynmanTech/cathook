/*
 * vtable.h
 *
 *  Created on: Aug 13, 2016
 *      Author: nullifiedcat
 */

#ifndef VTABLE_H_
#define VTABLE_H

#define GUARD 0xD34D0C47;

namespace uran {

class VMTHook {
public:
	void Init(void* addr) {
		base = addr;
		vmt_old = addr;;
	}
	void Hook(void* hook, int index);
	template<typename T>
	T Method(int offset);
	void* base;
	void* vmt_old;
	void* vmt_new;
};

}

#endif /* VTABLE_H_ */
