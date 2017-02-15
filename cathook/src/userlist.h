/*
 * userlist.h
 *
 *  Created on: Feb 15, 2017
 *      Author: nullifiedcat
 */

#ifndef USERLIST_H_
#define USERLIST_H_

namespace userlist {

class Role {
public:
	unsigned int bits;
	unsigned int color;
};

std::vector<int> roles;
std::unordered_map<unsigned int, int> userdata;

void Save();
void Load();

}

#endif /* USERLIST_H_ */
