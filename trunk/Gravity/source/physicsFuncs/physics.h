/*
 * physics.h
 *
 *  Created on: Mar 1, 2011
 *      Author: hopwoocp
 */

#ifndef PHYSICS_H_
#define PHYSICS_H_
#endif /* PHYSICS_H_ */

void gravityBullets(Ship * ship, Planet * GW);
void collide(Ship * ship, Planet * GW, int planetNum);
void gravityShip(Ship * ship, Planet * GW);
void checkLanding(Ship * ship, Planet * planet);
