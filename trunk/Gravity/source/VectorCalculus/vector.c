/*
 * vector.c
 *
 *  Created on: Mar 4, 2011
 *      Author: hopwoocp
 */

#include <math.h>
#include "../definitions.h"
float dotProductVelocities(Ship * ship, struct Bullet * bullet) {
	float product;
	product = ship->vx * bullet->vx + ship->vy * bullet->vy;
	return product;
}
