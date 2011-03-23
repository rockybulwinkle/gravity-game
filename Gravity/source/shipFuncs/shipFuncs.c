/*
 * shipFuncs.c
 *
 *  Created on: Feb 22, 2011
 *      Author: hopwoocp
 */
#include "../definitions.h"
#include "shipFuncs.h"
#include "../physicsFuncs/physics.h"
#include <stdlib.h>
#include <math.h>
#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>
#include "../staryBackground/stars.h"
#include "../gameMechanics/game.h"
Ship* initializeShips(int numShips, Ship ship[]) {
	int i;
	int j;
	u32 color;
	for (i = 0; i < numShips; i++) {
		ship[i].bullets = (Bullet*) malloc(NUM_BULLETS * sizeof(Bullet));
		if (ship[i].bullets == NULL) {
			exit(0);
		}
		ship[i].acc = SHIP_LIN_ACC;
		ship[i].ang = 0;
		ship[i].angAcc = SHIP_ANG_ACC;
		ship[i].color = i == 0 ? 0x7777FFFF : i == 1 ? 0xFF4444FF : 0xFFFFFFFF;
		ship[i].r = 10;
		ship[i].vx = 0;
		ship[i].vy = 0;
		ship[i].x = 0;
		ship[i].y = 0;
		ship[i].numBullets = 0;
		ship[i].isLanded = 0;
		ship[i].scrollX = SCREEN_X / 4;
		ship[i].scrollY = SCREEN_Y / 2;
		color = ship[i].color;
		ship[i].health = SHIP_HEALTH;
		ship[i].userOveride = 0;
		ship[i].landedPlanet = TAKEN_OVER;
		ship[i].shipNum = i;
		ship[i].points = 0;
		ship[i].diedFromPlanet = 0;
		ship[i].isAI = 0;
		for (j = 0; j < NUM_BULLETS; j++) {
			ship[i].bullets[j].vx = 0;
			ship[i].bullets[j].vy = 0;
			ship[i].bullets[j].x = 0;
			ship[i].bullets[j].y = 0;
			ship[i].bullets[j].drawn = 0;
			ship[i].bullets[j].baseColor = color;
		}
	}
	return ship;
}

/*Planet* initializePlanets(){

 }*/

void checkControllers(int chan, Ship * ship, Planet * planet) {
	int buttons = WPAD_ButtonsHeld(chan);
	ship->userOveride = 0;
	int turnLeft = 0, turnRight = 0, forward = 0, backward = 0, fire = 0;
	if (ship->isAI) {
		AI(ship, planet, &turnLeft, &turnRight, &forward, &backward, &fire, 1);
	}
	if (buttons & WPAD_BUTTON_RIGHT || forward == 1) {
		ship->vy += sin(ship->ang) * ship->acc;
		ship->vx += cos(ship->ang) * ship->acc;
		ship->userOveride = 1;
		ship->isLanded = 0;
	}
	if (buttons & WPAD_BUTTON_LEFT || backward == 1) {
		ship->vy -= sin(ship->ang) * ship->acc;
		ship->vx -= cos(ship->ang) * ship->acc;
		ship->userOveride = 1;
		ship->isLanded = 0;
	}
	if (buttons & WPAD_BUTTON_UP || turnLeft == 1) {
		ship->ang -= ship->angAcc;
	}
	if (buttons & WPAD_BUTTON_DOWN || turnRight == 1) {
		ship->ang += ship->angAcc;
	}
	if (buttons & WPAD_BUTTON_A) {
		ship->x = 0;
		ship->y = 0;
		ship->vx = 0;
		ship->vy = 0;
		ship->isLanded = 0;
		ship->userOveride = 0;
	}
	if (buttons & WPAD_BUTTON_1) {
		ship->bullets[ship->numBullets].vx = ship->vx + cos(ship->ang)
				* BULLET_SPEED;
		ship->bullets[ship->numBullets].vy = ship->vy + sin(ship->ang)
				* BULLET_SPEED;
		ship->bullets[ship->numBullets].x = cos(ship->ang) * ship->r + ship->x;
		ship->bullets[ship->numBullets].y = sin(ship->ang) * ship->r + ship->y;
		ship->bullets[ship->numBullets].drawn = 1;
		ship->bullets[ship->numBullets].milli = ticks_to_millisecs(gettime());

		ship->numBullets++;
		ship->numBullets = ship->numBullets % NUM_BULLETS;
	}

}

void update(Ship *ship, Planet * planet, int chan) {
	int i = 0;
	for (i = 0; i < NUM_PLANETS; i++) {
		collide(ship, &planet[i], i);
		gravityShip(ship, &planet[i]);
		gravityBullets(ship, &planet[i]);
		if (ship->diedFromPlanet) {
			ship->points--;
			killShip(ship, planet);
			ship->diedFromPlanet = 0;
			i = NUM_PLANETS;
		}
	}
	checkControllers(chan, ship, planet);
	updateSpeeds(ship, planet);
}

void updateSpeeds(Ship * ship, Planet * planet) {
	int i;
	float speed = hypot(ship->vx, ship->vy);
	if (speed > MAX_SHIP_SPEED) {
		ship->vx = ship->vx / speed * MAX_SHIP_SPEED;
		ship->vy = ship->vy / speed * MAX_SHIP_SPEED;
	}
	ship->x += ship->vx;
	ship->y += ship->vy;
	Bullet * bullet = ship->bullets;

	for (i = 0; i < NUM_BULLETS; i++) {
		if (bullet->drawn == 1) {
			bullet->x += (bullet->vx);
			bullet->y += (bullet->vy);
			if (bullet->x <= LEVEL_ORIGIN_X) {
				bullet->x += LEVEL_WIDTH;
			} else {
				bullet->x = fmodf(bullet->x, LEVEL_WIDTH);
			}
			if (bullet->y <= LEVEL_ORIGIN_Y) {
				bullet->y += LEVEL_HEIGHT;
			} else {
				bullet->y = fmodf(bullet->y, LEVEL_HEIGHT);
			}

		}
		bullet++;
	}
	if (ship->x <= LEVEL_ORIGIN_X) {
		ship->x += LEVEL_WIDTH;
		return;
	}
	if (ship->y <= LEVEL_ORIGIN_Y) {
		ship->y += LEVEL_HEIGHT;
		return;
	}
	ship->x = fmodf(ship->x, LEVEL_WIDTH);
	ship->y = fmodf(ship->y, LEVEL_HEIGHT);
}
