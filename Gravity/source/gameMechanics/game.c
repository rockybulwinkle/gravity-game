/*
 * game.c
 *
 *  Created on: Mar 5, 2011
 *      Author: hopwoocp
 */

#include "../definitions.h"
#include "grrlib.h"
#include "game.h"
#include <math.h>

extern GRRLIB_texImg *tex_Calibri;

//executes each game mechanic.  takes an array of ships and planets
void doMechanics(Ship * ships, Planet * planets) {
	dealDamage(ships, planets);
	takeOverPlanets(ships, planets);
	printPoints(ships);
}

//deals damage.  called from game.doMechanics.
void dealDamage(Ship * ships, Planet * planets) {
	bulletDamage(ships, planets);
	planetDamage(ships, planets);
}

//deals bullet damage.  called from game.dealDamage
void bulletDamage(Ship * ship, Planet * planets) {
	//does damage from other ship's bullets
	int i, j, k, relX, relY;
	Bullet * enemy; //used to set enemy's bullets to an array
	for (j = 0; j < NUM_SHIPS; j++) { // count through bullets attacking
		for (k = 0; k < NUM_SHIPS; k++) { // apply to each ship recieving
			enemy = ship[j].bullets; //set our bullets to an array, do each time because of pointer arithmetic
			for (i = j == k ? NUM_BULLETS : 0; i < NUM_BULLETS; i++) { // only do this loop if the bullets are not your own.
				if (enemy->drawn) {
					relX = ship[k].x - enemy->x; //calculate relative position
					relY = ship[k].y - enemy->y;
					if (hypot(relX, relY) < ship[k].r) { //if the bullet is inside the ships radius
						enemy->drawn = 0; //the bullet is no longer drawn
						ship[k].health -= hypot(enemy->vx, enemy->vy); //decrement the ship's health by the bullet's velocity
						if (ship[k].health < 0) { //if the ship's health dropped down below 0, kill it
							killShip(&ship[k], planets); //kills ship.  resides in game.c
							ship[j].points++; //deal points
							ship[k].points--;
						}
					}
				}
				enemy++;
			}
		}
	}
}
void planetDamage(Ship * ship, Planet * planets) {
	//does damage from other ship's bullets
	int i, j, k, relX, relY;
	Bullet * enemy; //used to set enemy's bullets to an array
	for (j = 0; j < NUM_SHIPS; j++) { // count through bullets attacking
		for (k = 0; k < NUM_PLANETS; k++) { // apply to each ship recieving
			enemy = ship[j].bullets; //set our bullets to an array, do each time because of pointer arithmetic
			for (i = j == planets[k].owner ? NUM_BULLETS : 0; i < NUM_BULLETS; i++) { // only do this loop if the bullets are not your own.
				if (enemy->drawn) { //if the bullet is active
					relX = planets[k].x - enemy->x; //calculate relative position
					relY = planets[k].y - enemy->y;
					if (hypot(relX, relY) < planets[k].r) { //if the planet's health drops below zero:
						enemy->drawn = 0; //deactivate the bullet
						planets[k].health -= hypot(enemy->vx, enemy->vy); //decrement the planet's health by the bullet's velocity
						if (planets[k].health < 0) // if the planet is dead:
							killPlanet(&planets[k]); //kill the planet.  function resides in game.c
					}
				}
				enemy++;
			}
		}
	}
}
// kills the ship. Called from various places
void killShip(Ship * ship, Planet * planets) {
	ship->health = SHIP_HEALTH; //restore health
	int i;
	int spawnPlanet = -1; //used to find a planet that the ship owns.
	for (i = 0; i < NUM_PLANETS; i++) {
		if (planets[i].owner == ship->shipNum) { // if the planet is owned by the ship, set spawnPlanet to that planet number
			spawnPlanet = i;
			i = NUM_PLANETS;
		}
	}
	ship->vx = 0; //reset velocity
	ship->vy = 0;
	if (spawnPlanet != -1) { //if we found a planet, land the ship on the planet
		ship->x = planets[spawnPlanet].x; //set the x to planet's x
		ship->y = planets[spawnPlanet].y - planets[spawnPlanet].r - ship->r; // set the Y to the planets Y
		ship->ang = -PI / 2; //reset the ship's angle so it is pointed up.
		ship->isLanded = 1; //the ship should be landed now
	} else { // if we did not find a planet, reset the position to the orign
		ship->x = 0;
		ship->y = 0;

		ship->isLanded = 0;
	}
}
//kills the planet.  Called from various places
void killPlanet(Planet * planet) {
	planet->owner = NO_OWNER;
	planet->health = 0;

}

//checks to see if any planets have been taken over.  called from game.doMechanics
void takeOverPlanets(Ship * ship, Planet * planet) {
	int i;
	for (i = 0; i < NUM_SHIPS; i++) {
		if (ship[i].landedPlanet != TAKEN_OVER) {
			if (ship[i].isLanded && planet[ship[i].landedPlanet].health == 0) {
				planet[ship[i].landedPlanet].owner = ship[i].shipNum;
				planet[ship[i].landedPlanet].health = PLANET_HEALTH;
				ship[i].points++;
				ship[i].landedPlanet = TAKEN_OVER;
			}
		}
	}
}

void printPoints(Ship * ship) {
	int i;
	int root;
	for (i = 0; i < NUM_SHIPS; i++) {
		root = ship[i].shipNum == 0 ? SHIP_HEALTH + 20 : SCREEN_X
				- (SHIP_HEALTH + 20);
		GRRLIB_Printf(root, 20, tex_Calibri, ship[i].color, 1, "%d",
				ship[i].points);
	}
}

void AI(Ship * ship, Planet * planet, int * turnLeft, int * turnRight,
		int * forward, int * backward, int * fire, int shipNum) {
	const float DEEP_SPACE = 100;
	//stores the 'intent' of the ship.
	//used to remember what it was doing.
	static int intent = 0;

	//find the nearest planet
	int i;
	float nearestPlanetDistance = hypot(planet[0].x - ship->x, planet[0].y
			- ship->y);
	int nearestPlanetNum = 0;
	for (i = 0; i < NUM_PLANETS; i++) {
		int distanceToPlanet = hypot(planet[i].x - ship->x, planet[i].y
				- ship->y);
		int collisionDistance = sqrtf(powf(-planet[i].y
				* ship->vx + ship->y * ship->vx + planet[i].x
				* ship->vy - ship->x * ship->vy, 2) / (powf(ship->vx, 2)
				+ powf(ship->vy, 2)));

		if (distanceToPlanet < nearestPlanetDistance && collisionDistance
				< planet[i].r + ship->r * 2) {
			nearestPlanetDistance = distanceToPlanet;
			nearestPlanetNum = i;
		}
	}
	GRRLIB_Printf(400, 20, tex_Calibri, 0xFFFFFFFF, 1, "%d, %f",
			nearestPlanetNum, nearestPlanetDistance);
	Vector shipSpeedDir; //direction that the ship's velocity is headed
	Vector planetVec; //direction to the planet
	Vector unitPlanetVec;

	// store the ship's unit vector for where it is pointing
	float speed = hypot(ship->vx, ship->vy);
	shipSpeedDir.x = ship->vx / speed;
	shipSpeedDir.y = ship->vy / speed;

	// stores the direction that the ship is facing
	Vector shipDirUnitVec;
	shipDirUnitVec.x = cos(ship->ang);
	shipDirUnitVec.y = sin(ship->ang);
	// store the vector that points from the ship to the planet
	planetVec.x = planet[nearestPlanetNum].x - ship->x;
	planetVec.y = planet[nearestPlanetNum].y - ship->y;

	// calulate unit vector for ship to planet
	float distanceToPlanet = hypot(planetVec.x, planetVec.y);
	unitPlanetVec.x = planetVec.x / distanceToPlanet;
	unitPlanetVec.y = planetVec.y / distanceToPlanet;

	// calculate the distance to surface
	float distanceToSurface = distanceToPlanet - planet[0].r;

	// calculate the angle between the ship's velocity and the planet
	float angleVelPlanet = compareAngles(shipSpeedDir, planetVec);

	if (speed != 0) {

		//calculates the smallest distance the the ship will be against the nearest planet in the future.
		int collisionDistance = sqrtf(powf(-planet[nearestPlanetNum].y
				* ship->vx + ship->y * ship->vx + planet[nearestPlanetNum].x
				* ship->vy - ship->x * ship->vy, 2) / (powf(ship->vx, 2)
				+ powf(ship->vy, 2)));

		if (collisionDistance < planet[nearestPlanetNum].r + ship->r * 2) {
			GRRLIB_Printf(400, 60, tex_Calibri, 0xFFFFFFFF, 1, "COLLISION");
			Vector perpToPlanetVec;
			perpToPlanetVec.x = -planetVec.y;
			perpToPlanetVec.y = planetVec.x;
			float angleToPoint = compareAngles(shipDirUnitVec, perpToPlanetVec);
			GRRLIB_Printf(400, 90, tex_Calibri, 0xFFFFFFFF, 1, "%f",
					angleToPoint);
			*forward = 1;
			if (angleToPoint > 0) {
				if (angleToPoint < 1.626 + .4) {
					*turnLeft = 1;
				} else if (angleToPoint > 1.515 + .4) {
					*turnRight = 1;
				}

			} else if (angleToPoint < 0) {
				if (angleToPoint < -1.626 - .4) {
					*turnLeft = 1;
				} else if (angleToPoint > -1.515 - .4) {
					*turnRight = 1;
				}

			}
		}

	}
}

//returns the angle between two vectors.  if V1 is clockwise on screen (counterclockwise on cartesian), angle is positive
//if V1 is counterclockwise to V2 on screen (clockwise on cartesian), then angle is negative
float compareAngles(Vector one, Vector two) {
	float cosine = (one.x * two.x + one.y * two.y) / (hypot(one.x, one.y)
			* hypot(two.x, two.y));
	float sine = (one.y * two.x - one.x * two.y) / (hypot(one.x, one.y)
			* hypot(two.x, two.y));
	float calculatedAngle = copysignf(acos(cosine), asin(sine));
	return calculatedAngle;
}

//for some reason this lands it perfectly:
/**
 * void AI(Ship * ship, Planet * planet, int * turnLeft, int * turnRight,
 int * forward, int * backward, int * fire, int shipNum) {
 const float DEEP_SPACE = 100;
 //stores the 'intent' of the ship.
 //used to remember what it was doing.
 static int intent = 0;

 //find the nearest planet
 int i;
 float nearestPlanetDistance = hypot(planet[0].x - ship->x, planet[0].y
 - ship->y);
 int nearestPlanetNum = 0;
 for (i = 0; i < NUM_PLANETS; i++) {
 int distanceToPlanet = hypot(planet[i].x - ship->x, planet[i].y
 - ship->y);

 if (distanceToPlanet < nearestPlanetDistance) {
 nearestPlanetDistance = distanceToPlanet;
 nearestPlanetNum = i;
 }
 }
 GRRLIB_Printf(400, 20, tex_Calibri, 0xFFFFFFFF, 1, "%d, %f",
 nearestPlanetNum, nearestPlanetDistance);
 Vector shipSpeedDir; //direction that the ship's velocity is headed
 Vector planetVec; //direction to the planet
 Vector unitPlanetVec;

 // store the ship's unit vector for where it is pointing
 float speed = hypot(ship->vx, ship->vy);
 shipSpeedDir.x = ship->vx / speed;
 shipSpeedDir.y = ship->vy / speed;

 // stores the direction that the ship is facing
 Vector shipDirUnitVec;
 shipDirUnitVec.x = cos(ship->ang);
 shipDirUnitVec.y = sin(ship->ang);
 // store the vector that points from the ship to the planet
 planetVec.x = planet[nearestPlanetNum].x - ship->x;
 planetVec.y = planet[nearestPlanetNum].y - ship->y;

 // calulate unit vector for ship to planet
 float distanceToPlanet = hypot(planetVec.x, planetVec.y);
 unitPlanetVec.x = planetVec.x / distanceToPlanet;
 unitPlanetVec.y = planetVec.y / distanceToPlanet;

 // calculate the distance to surface
 float distanceToSurface = distanceToPlanet - planet[0].r;

 // calculate the angle between the ship's velocity and the planet
 float angleVelPlanet = compareAngles(shipSpeedDir, planetVec);

 if (speed != 0) {

 //calculates the smallest distance the the ship will be against the nearest planet in the future.
 int collisionDistance = sqrtf(powf(-planet[nearestPlanetNum].y
 * ship->vx + ship->y * ship->vx + planet[nearestPlanetNum].x
 * ship->vy - ship->x * ship->vy, 2) / (powf(ship->vx, 2)
 + powf(ship->vy, 2)));

 if (collisionDistance < planet[nearestPlanetNum].r + ship->r) {
 GRRLIB_Printf(400, 60, tex_Calibri, 0xFFFFFFFF, 1, "COLLISION");
 Vector perpToPlanetVec;
 perpToPlanetVec.x = -planetVec.y;
 perpToPlanetVec.y = planetVec.x;
 float angleToPoint = compareAngles(shipDirUnitVec, perpToPlanetVec);
 GRRLIB_Printf(400, 90, tex_Calibri, 0xFFFFFFFF, 1, "%f",
 angleToPoint);
 //*forward = 1;
 if (angleToPoint > 0) {
 if (angleToPoint > 1.626) {
 *turnLeft = 1;
 } else if (angleToPoint < 1.515) {
 *turnRight = 1;
 }
 if (angleToPoint<1.6 && angleToPoint>1.2){
 *forward = 1;
 }

 } else if (angleToPoint < 0) {
 if (angleToPoint < -1.626) {
 *turnLeft = 1;
 } else if (angleToPoint > -1.515) {
 *turnRight = 1;
 }
 if (angleToPoint>-1.6 && angleToPoint<-1.2){
 *forward = 1;
 }

 }
 }

 }
 }
 */

