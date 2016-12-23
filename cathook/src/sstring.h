/*
 * sstring.h
 *
 *  Created on: Dec 23, 2016
 *      Author: nullifiedcat
 */

#ifndef SSTRING_H_
#define SSTRING_H_

class sstring {
public:
	inline sstring() { length = 0; p_String = 0; };
	inline sstring(int length) { this->length = length; p_String = new char[length]; }
	inline ~sstring() { delete [] p_String; }
	inline operator const char*() const { return p_String; };
	inline operator char*() const { return (char*)p_String; }
	const char* p_String;
	int length;
};

#endif /* SSTRING_H_ */
