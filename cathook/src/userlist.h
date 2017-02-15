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
	has_color = (1 << 1),
	always_visible = (1 << 2),
	highest_priority = (1 << 3);
}

struct user_mode_s {
	unsigned int bits;
	int color;
};

struct user_role_s {
	user_mode_s mode;
	std::string name;
};

struct result_user_data_s {
	size_t role;
	user_mode_s special_mode;
	user_mode_s result_mode;
	void ComputeResult();
};

extern const user_mode_s usermode_default;

extern std::vector<std::pair<std::string, user_mode_s>> roles;
extern std::unordered_map<int, std::pair<size_t, user_mode_s>> userlist;

// ReadOnly usermode for checks and stuff, returns mode_default if user has no special role
const user_mode_s& GetUserMode(CachedEntity& entity);
// ReadWrite usermode, creates a new one if user has no special role
user_mode_s& ModUserMode(CachedEntity& entity);

void Save();
void Load();

}

#endif /* USERLIST_H_ */
