/*
 * physics.c
 *
 *  Created on: Mar 1, 2011
 *      Author: hopwoocp
 */
#include "../definitions.h"
#include "physics.h"
#include <stdlib.h>
#include <math.h>

void gravityShip(Ship * ship, Planet * GW) { //takes a ship and modifies its trajectory and bullets due to gravity
	// calculate the difference in x and y positions
	if (ship->isLanded != 1) {
		gravitate(ship->x, ship->y, &ship->vx, &ship->vy, GW->x, GW->y, GW->m);
	}
}

void gravityBullets(Ship * ship, Planet * GW) {
	int i;
	struct Bullet * bullets = ship->bullets;
	for (i = 0; i < NUM_BULLETS; i++) {
		// only apply gravity to bullets that are active
		if (bullets->drawn == 1) {
			gravitate(bullets->x, bullets->y, &bullets->vx, &bullets->vy,
					GW->x, GW->y, GW->m);
		}
		bullets++;
	}
}

void collide(Ship * ship, Planet * GW, int planetNum) {
	float r[2];
	//start making a vector pointing from GW to ship
	r[0] = GW->x - ship->x;
	r[1] = GW->y - ship->y;
	float mag = sqrt(r[0] * r[0] + r[1] * r[1]);
	float vr[2];
	float vt[2];
	float dotProductRadial;
	float dotProductTangential;

	if (mag < GW->r + ship->r) {//if the ship has collided with the planet and has not landed, continue
		r[0] = r[0] / mag;
		r[1] = r[1] / mag;
		dotProductRadial = ship->vx * r[0] + ship->vy * r[1];
		dotProductTangential = ship->vx * -r[1] + ship->vy * r[0];
		vr[0] = dotProductRadial * r[0];
		vr[1] = dotProductRadial * r[1];
		vt[0] = dotProductTangential * -r[1];
		vt[1] = dotProductTangential * r[0];
		if (ship->isLanded == 0) {
			ship->isLanded = 1;
			if (hypot(ship->vx, ship->vy) > SHIP_LAND_SPEED) {
				ship->vx = -vr[0] * DAMPING_CONSTANT + vt[0] * DAMPING_CONSTANT;
				ship->vy = -vr[1] * DAMPING_CONSTANT + vt[1] * DAMPING_CONSTANT;
				ship->isLanded = 0;
				ship->health -= hypot(ship->vx, ship->vy);
				if (ship->health <= 0) {
					ship->diedFromPlanet = 1;
				}
			} else if (!ship->userOveride) {
				ship->vx = 0;
				ship->vy = 0;
			} else {
				ship->isLanded = 0;
			}
		}
		ship->x = -(GW->r + ship->r) * r[0] + GW->x;
		ship->y = -(GW->r + ship->r) * r[1] + GW->y;
		ship->landedPlanet = planetNum;
	}

	int i;
	for (i = 0; i < NUM_BULLETS; i++) {
		if (ship->bullets[i].drawn == 1) {
			r[0] = GW->x - ship->bullets[i].x;
			r[1] = GW->y - ship->bullets[i].y;
			mag = sqrt(pow(r[0], 2) + pow(r[1], 2));
			if (mag < GW->r) { //if the ship has collided with the planet, continue
				r[0] = mag > .01 ? r[0] / mag : 0;
				r[1] = mag > .01 ? r[1] / mag : 0;
				dotProductRadial = ship->bullets[i].vx * r[0]
						+ ship->bullets[i].vy * r[1];
				dotProductTangential = ship->bullets[i].vx * -r[1]
						+ ship->bullets[i].vy * r[0];
				vr[0] = dotProductRadial * r[0];
				vr[1] = dotProductRadial * r[1];
				vt[0] = dotProductTangential * -r[1];
				vt[1] = dotProductTangential * r[0];
				ship->bullets[i].vx = 0;
				ship->bullets[i].vy = 0;
				ship->bullets[i].vx = -vr[0] + vt[0];
				ship->bullets[i].vy = -vr[1] + vt[1];
				ship->bullets[i].x = -GW->r * r[0] + GW->x;
				ship->bullets[i].y = -GW->r * r[1] + GW->y;
			}
		}
	}
}

void checkLanding(Ship * ship, Planet * GW) {
	float r[2];
	//start making a vector pointing from GW to ship
	r[0] = GW->x - ship->x;
	r[1] = GW->y - ship->y;
	float mag = hypot(r[0], r[1]);
	if (mag <= GW->r + ship->r) {
		ship->isLanded++;
	}
	ship->isLanded = ship->isLanded > 2 ? 2 : ship->isLanded;
}

inline void gravitate(float bX, float bY, float * bVX, float * bVY, float gwX,
		float gwY, int gwM) {
	int jj, kk;
	for (jj = -1; jj <= 1; jj++) {
		float x = gwX + LEVEL_WIDTH * jj;
		for (kk = -1; kk <= 1; kk++) {
			float y = gwY + LEVEL_HEIGHT * kk;
			float dx = bX - x;
			float dy = bY - y;
			//find the distance between the two objects
			float r = dx * dx + dy * dy;
			//calculate the change in velocity (i.e. acceleration)
			float dv = gwM / r;
			r = babylonianSqrt(r);
			//convert the dv (change in velocity) to change of velocity in x and y
			float dvx = dv * dx / r;
			float dvy = dv * dy / r;
			//add the change in velocity in x and y to the ships x and y
			*bVX -= dvx;
			*bVY -= dvy;
		}
	}
}

inline void gravitateGrid(float bX, float bY, float * bVX, float * bVY,
		float gwX, float gwY, int gwM) {
	int jj, kk;
	for (jj = -1; jj <= 1; jj++) {
		float x = gwX + LEVEL_WIDTH * jj;
		for (kk = -1; kk <= 1; kk++) {
			float y = gwY + LEVEL_HEIGHT * kk;
			float dx = bX - x;
			float dy = bY - y;
			//find the distance between the two objects
			float r = dx * dx + dy * dy;
			//calculate the change in velocity (i.e. acceleration)
			float dv = gwM / r;
			r = babylonianSqrt(r);
			if (dv > r) {
				*bVX = -dx;
				*bVY = -dy;
				return;
			}
			//convert the dv (change in velocity) to change of velocity in x and y
			float dvx = dv * dx / r;
			float dvy = dv * dy / r;
			//add the change in velocity in x and y to the ships x and y
			*bVX -= dvx;
			*bVY -= dvy;
		}
	}
}

inline float babylonianSqrt(float x) {
	float guess = 10;
	float div = x / guess;
	guess = (div + guess) / 2;
	div = x / guess;
	guess = (div + guess) / 2;
	div = x / guess;
	return (div + guess) / 2;
}

inline float babylonianHypot(float x, float y) {
	return babylonianSqrt(x * x + y * y);
}
