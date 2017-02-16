/*
 * userlist.h
 *
 *  Created on: Feb 15, 2017
 *      Author: nullifiedcat
 */

#ifndef USERLIST_H_
#define USERLIST_H_

namespace userlist {

namespace bits {
const unsigned int
	ignore = (1 << 0),
	force_attack = (1 << 1),
	aimbot_priority = (1 << 2),
	higher_healing_priority = (1 << 3),
	exclusive_healing_priority = (1 << 4);
extern const std::vector<std::string> names;
}

class Data {
public:
	size_t role;
	unsigned int bits_added;
	unsigned int bits_removed;
	int force_color;
	void OnDataChanged();
	unsigned int bits;
	int color;
};

class Role {
public:
	unsigned int bits;
	unsigned int color;
};

extern const Role roles[4];
extern std::unordered_map<unsigned int, Data> userdata;

unsigned int UserBits(CachedEntity& entity);
void AddUserBits(CachedEntity& entity, unsigned int bit);
void RemoveUserBits(CachedEntity& entity, unsigned int bit);

void SetUserRole(unsigned int steamid, size_t role);
void SetUserColor(unsigned int steamid, int color);

void Save();
void Load();

CatCommand user_set_role;
CatCommand user_set_color;
CatCommand user_add_bit;
CatCommand user_remove_bit;
CatCommand user_list;
CatCommand user_data;

}

#endif /* USERLIST_H_ */
