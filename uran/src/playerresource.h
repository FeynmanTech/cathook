/*
 * playerresource.h
 *
 *  Created on: Nov 13, 2016
 *      Author: nullifiedcat
 */

#ifndef PLAYERRESOURCE_H_
#define PLAYERRESOURCE_H_

class IClientEntity;

class TFPlayerResource {
public:
	void Update();
	int GetMaxHealth(IClientEntity* player);

	IClientEntity* m_pEntity;
};

extern TFPlayerResource* g_pPlayerResource;

#endif /* PLAYERRESOURCE_H_ */
