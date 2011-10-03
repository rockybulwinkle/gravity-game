/*
 * game.c
 *
 *  Created on: Mar 5, 2011
 *      Author: hopwoocp
 */

#include "../definitions.h"
#include "grrlib.h"
#include "game.h"
#include <time.h>
#include <math.h>

extern GRRLIB_texImg *tex_Calibri;
extern long frameCount;

//executes each game mechanic.  takes an array of ships and planets
void doMechanics(Ship * ships, Planet * planets) {
	dealDamage(ships, planets);
	takeOverPlanets(ships, planets);
	shootEnemyShips(ships, planets);
	printPoints(ships);
	upgradeAvailable(planets);
	giveUpgrades(ships, planets);
	handleSpecialBullets(ships, planets);
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
	struct Bullet * enemy; //used to set enemy's bullets to an array
	for (j = 0; j < NUM_SHIPS; j++) { // count through bullets attacking
		for (k = 0; k < NUM_SHIPS; k++) { // apply to each ship receiving
			enemy = ship[j].bullets; //set our bullets to an array, do each time because of pointer arithmetic
			for (i = (j == k) ? NUM_BULLETS : 0; i < NUM_BULLETS; i++) { // only do this loop if the bullets are not your own.
				if (enemy[i].drawn) {
					relX = ship[k].x - enemy[i].x; //calculate relative position
					relY = ship[k].y - enemy[i].y;
					if (hypot(relX, relY) < ship[k].r) { //if the bullet is inside the ships radius
						enemy[i].drawn = 0; //the bullet is no longer drawn
						ship[k].health -= hypot(enemy[i].vx, enemy[i].vy); //decrement the ship's health by the bullet's velocity
						if (ship[k].health < 0) { //if the ship's health dropped down below 0, kill it
							killShip(&ship[k], planets); //kills ship.  resides in game.c
							ship[j].points++; //deal points
							ship[k].points--;
						}
					}
				}
			}
		}
	}
}
void planetDamage(Ship * ship, Planet * planets) {
	//does damage from other ship's bullets
	int i, j, k, relX, relY;
	struct Bullet * enemy; //used to set enemy's bullets to an array
	for (j = 0; j < NUM_SHIPS; j++) { // count through bullets attacking
		for (k = 0; k < NUM_PLANETS; k++) { // apply to each ship recieving
			enemy = ship[j].bullets; //set our bullets to an array, do each time because of pointer arithmetic
			for (i = j == planets[k].owner ? NUM_BULLETS : 0; i < NUM_BULLETS; i++) { // only do this loop if the bullets are not your own.
				if (enemy[i].drawn) { //if the bullet is active
					relX = planets[k].x - enemy[i].x; //calculate relative position
					relY = planets[k].y - enemy[i].y;
					if (hypot(relX, relY) < planets[k].r) { //if the planet's health drops below zero:
						enemy[i].drawn = 0; //deactivate the bullet
						planets[k].health -= hypot(enemy[i].vx, enemy[i].vy); //decrement the planet's health by the bullet's velocity
						if (planets[k].health < 0) // if the planet is dead:
							killPlanet(&planets[k]); //kill the planet.  function resides in game.c
					}
				}
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
				planet[ship[i].landedPlanet].currentUpgrade = 0;
				planet[ship[i].landedPlanet].time_since_upgrade = time(NULL);
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

void shootEnemyShips(Ship * ships, Planet * planets) {
	int i, j;
	for (i = 0; i < NUM_SHIPS; i++) {
		for (j = 0; j < NUM_PLANETS; j++) {
			if (planets[j].owner != ships[i].shipNum && planets[j].owner
					!= NO_OWNER) {
				Planet * plan = &planets[j];
				Ship * ship = &ships[i];
				struct Bullet * bull =
						&planets[j].bullets[planets[j].numBullets];
				float r = sqrtf(pow((ship->x - plan->x), 2) + pow((ship->y
						- plan->y), 2));

				bull->x = (ship->x - plan->x) * plan->r / r;
				bull->y = (ship->y - plan->y) * plan->r / r;
				bull->vx = bull->x / plan->r * BULLET_SPEED;
				bull->vy = bull->y / plan->r * BULLET_SPEED;
				bull->x += plan->x;
				bull->y += plan->y;
				bull->drawn = 1;
				bull->shiftedColor = 0xFFFFFFFF;
				plan->numBullets++;
				if (plan->numBullets >= PLANET_BULLET_NUM) {
					plan->numBullets = 0;
				}
			}
		}
	}
}

void upgradeAvailable(Planet * planets) {
	int i;
	for (i = 0; i < NUM_PLANETS; i++) {
		if (time(NULL) - planets[i].time_since_upgrade > TIME_TO_UPGRADE
				&& planets[i].owner != NO_OWNER && planets[i].currentUpgrade
				== 0) {
			planets[i].currentUpgrade = 2;
			planets[i].time_since_upgrade = time(NULL);
		}
	}
}

void giveUpgrades(Ship * ships, Planet * planets) {
	int i;
	for (i = 0; i < NUM_SHIPS; i++) {
		if (ships[i].isLanded == 1 && planets[ships[i].landedPlanet].owner == i
				&& planets[ships[i].landedPlanet].currentUpgrade != 0) {
			ships[i].currentWeapon
					= planets[ships[i].landedPlanet].currentUpgrade;
			planets[ships[i].landedPlanet].currentUpgrade = 0;
			planets[ships[i].landedPlanet].time_since_upgrade = time(NULL);
		}
	}
}

void handleSpecialBullets(Ship * ships, Planet * planets) {
	int i;
	for (i = 0; i < NUM_SHIPS; i++) {
		struct Bullet * bullets = ships[i].bullets;
		int j;
		for (j = 0; j < NUM_BULLETS; j++) {
			if (bullets[j].weaponType == 2) {
				if (frameCount - bullets[j].milli >= 60 && bullets[j].exploded == 0) {
					bullets[j].exploded = 1;
					int r;
					for (r = 0; r < NUM_EXPLODER; r++) {
						bullets[j].subBullets[r].x = 0;
						bullets[j].subBullets[r].y = 0;
						bullets[j].subBullets[r].vx = cos(2*3.1415926*r/NUM_EXPLODER)*MAX_BULLET_SPEED;
						bullets[j].subBullets[r].vy = sin(2*3.1415926*r/NUM_EXPLODER)*MAX_BULLET_SPEED;
					}
				}

			}
		}
	}
}
