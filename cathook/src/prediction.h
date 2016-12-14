/*
 * prediction.h
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#ifndef PREDICTION_H_
#define PREDICTION_H_

#include "enums.h"

class IClientEntity;
class Vector;

Vector SimpleLatencyPrediction(IClientEntity* ent, int hb);
Vector ProjectilePrediction(IClientEntity* ent, int hb, float speed, float gravitymod);

float DistanceToGround(Vector origin);

#endif /* PREDICTION_H_ */
