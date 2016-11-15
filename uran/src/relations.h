/*
 * relations.h
 *
 *  Created on: Nov 13, 2016
 *      Author: nullifiedcat
 */

#ifndef RELATIONS_H_
#define RELATIONS_H_

typedef unsigned long uint32;

enum relation {
	NONE = 0,
	FRIEND,   // PURPLE color
	FRIENDLY, // Ignored by aimbot, GREEN color
	RAGE      // Max target priority, YELLOW color
};

#define MAX_FRIENDS 256

class CRelations {
public:
	void Add(uint32 id, relation rel);
	void Remove(uint32 id);
	void WriteConfig();
	void ShiftArray(uint32* array, int idx, int max);

	uint32 m_Friends[MAX_FRIENDS];
	uint32 m_Friendlies[MAX_FRIENDS];
	uint32 m_Rage[MAX_FRIENDS];

	int m_nFriends;
	int m_nFriendlies;
	int m_nRage;
};

extern char* RelStr[4];
extern CRelations* g_pRelations;

#endif /* RELATIONS_H_ */
