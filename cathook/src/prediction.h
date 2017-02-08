/*
 * prediction.h
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#ifndef PREDICTION_H_
#define PREDICTION_H_

#include "enums.h"

class CachedEntity;
class Vector;

Vector SimpleLatencyPrediction(CachedEntity* ent, int hb);

bool PerformProjectilePrediction(CachedEntity* target, int hitbox);

Vector ProjectilePrediction(CachedEntity* ent, int hb, float speed, float gravitymod, float entgmod);

float PlayerGravityMod(CachedEntity* player);

float DistanceToGround(CachedEntity* ent);
float DistanceToGround(Vector origin);

#endif /* PREDICTION_H_ */
