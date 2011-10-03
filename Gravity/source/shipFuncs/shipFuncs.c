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

extern long frameCount;
extern GRRLIB_texImg * tex_Calibri;
Ship* initializeShips(int numShips, Ship * ship) {
	int i;
	int j;
	u32 color;
	for (i = 0; i < numShips; i++) {
		ship[i].bullets = (struct Bullet*) malloc(NUM_BULLETS
				* sizeof(struct Bullet));
		if (ship[i].bullets == NULL) {
			exit(0);
		}
		ship[i].acc = SHIP_LIN_ACC;
		ship[i].ang = 0;
		ship[i].angAcc = SHIP_ANG_ACC;
		ship[i].color = i == 0 ? 0x7777FFFF : i == 1 ? 0xFF7777FF : 0xFFFFFFFF;
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
		ship[i].currentWeapon = 0;
		for (j = 0; j < NUM_BULLETS; j++) {

			ship[i].bullets[j].vx = 0;
			ship[i].bullets[j].vy = 0;
			ship[i].bullets[j].x = 0;
			ship[i].bullets[j].y = 0;
			ship[i].bullets[j].drawn = 0;
			ship[i].bullets[j].baseColor = color;
			ship[i].bullets[j].weaponType = 0;
			ship[i].bullets[j].subBullets = (struct Bullet *) malloc(
					sizeof(struct Bullet) * NUM_EXPLODER);
			if (ship[i].bullets[j].subBullets == NULL) {
				exit(0);
			}
		}
	}
	return ship;
}

/*Planet* initializePlanets(){

 }*/

void checkControllers(int chan, Ship * ship, Planet * planet) {
	int buttons = WPAD_ButtonsHeld(chan);
	ship->userOveride = 0;
	//	if (ship->isAI) {
	//		AI(ship, planet, &turnLeft, &turnRight, &forward, &backward, &fire, 1);
	//	}
	if (buttons & WPAD_BUTTON_RIGHT) {
		ship->vy += sin(ship->ang) * ship->acc;
		ship->vx += cos(ship->ang) * ship->acc;
		ship->userOveride = 1;
		ship->isLanded = 0;
	}
	if (buttons & WPAD_BUTTON_LEFT) {
		ship->vy -= sin(ship->ang) * ship->acc;
		ship->vx -= cos(ship->ang) * ship->acc;
		ship->userOveride = 1;
		ship->isLanded = 0;
	}
	if (buttons & WPAD_BUTTON_UP) {
		ship->ang -= ship->angAcc;
	}
	if (buttons & WPAD_BUTTON_DOWN) {
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
		struct Bullet * bullet = &ship->bullets[ship->numBullets];
		bullet->vx = ship->vx + cos(ship->ang) * BULLET_SPEED;
		bullet->vy = ship->vy + sin(ship->ang) * BULLET_SPEED;
		bullet->x = cos(ship->ang) * ship->r + ship->x;
		bullet->y = sin(ship->ang) * ship->r + ship->y;
		bullet->drawn = 1;
		bullet->shiftedColor = ship->color;
		bullet->weaponType = ship->currentWeapon;
		bullet->milli = frameCount;
		bullet->state = 0;
		bullet->exploded = 0;
		ship->numBullets++;
		ship->numBullets = ship->numBullets % NUM_BULLETS;
	}

}

void update(Ship *ship, Planet * planet) {
	int i = 0;
	int j = 0;
	for (j = 0; j < NUM_SHIPS; j++) {
		for (i = 0; i < NUM_PLANETS; i++) {
			collide(&ship[j], &planet[i], i);
			gravityShip(&ship[j], &planet[i]);
			gravityBullets(&ship[j], &planet[i]);
			if (ship[j].diedFromPlanet) {
				ship[j].points--;
				killShip(&ship[j], planet);
				ship[j].diedFromPlanet = 0;
				i = NUM_PLANETS;
			}
		}
		checkControllers(j, &ship[j], planet);
		updateShipSpeeds(&ship[j]);
	}

	updatePlanetBullets(planet);
}

void updateShipSpeeds(Ship * ship) {
	int i;
	float speed = hypot(ship->vx, ship->vy);
	if (speed > MAX_SHIP_SPEED) {
		ship->vx = ship->vx / speed * MAX_SHIP_SPEED;
		ship->vy = ship->vy / speed * MAX_SHIP_SPEED;
	}
	ship->x += ship->vx;
	ship->y += ship->vy;
	struct Bullet * bullet = ship->bullets;

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
			if (ship->bullets[i].weaponType == 2 & ship->bullets[i].exploded
					== 1) {
				int r;
				for (r = 0; r < NUM_EXPLODER; r++) {
					bullet->subBullets[r].x += bullet->subBullets[r].vx;
					bullet->subBullets[r].y += bullet->subBullets[r].vy;
				}
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

void updatePlanetBullets(Planet * planets) {
	int i, j;
	for (i = 0; i < NUM_PLANETS; i++) {
		for (j = 0; j < PLANET_BULLET_NUM; j++) {
			struct Bullet * bullet = &planets[i].bullets[j];
			bullet->x += bullet->vx;
			bullet->y += bullet->vy;
		}
	}
}
