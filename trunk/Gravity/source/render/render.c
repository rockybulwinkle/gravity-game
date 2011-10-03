/*
 * render.c
 *
 *  Created on: Mar 2, 2011
 *      Author: hopwoocp
 */
#include "../definitions.h"
#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <grrlib.h>
#include <math.h>
#include "../VectorCalculus/vector.h"
#include <ogc/lwp_watchdog.h>
#include "../staryBackground/stars.h"
#include "grid.h"
#include "../physicsFuncs/physics.h"
//#include "../fonts/Calibri_18.h"
extern GRRLIB_texImg * tex_Calibri;
void renderShip(Ship * ship, int offsetX, int offsetY, int shipNum) {
	//my ships position - object position + offset
	//this code is very sloppy, needs revision to be more concise
	int jj, kk;
	int p1x;
	int p1y;
	int p2x;
	int p2y;
	int p3x;
	int p3y;
	float ang = ship[shipNum].ang;
	int x = ship[shipNum].x;
	int y = ship[shipNum].y;
	int r = ship[shipNum].r;
	float ang2;
	int x2;
	int y2;
	int r2;
	p1x = cos(ang) * r + offsetX;
	p1y = sin(ang) * r + offsetY;
	p2x = cos(ang + PI * 2 / 3) * r + offsetX;
	p2y = sin(ang + PI * 2 / 3) * r + offsetY;
	p3x = cos(ang - PI * 3 / 4) * r + offsetX;
	p3y = sin(ang - PI * 3 / 4) * r + offsetY;
	GRRLIB_Line(p1x, p1y, p2x, p2y, ship[shipNum].color);
	//GRRLIB_Line(p2x, p2y, p3x, p3y, ship->color);
	GRRLIB_Line(p3x, p3y, p1x, p1y, ship[shipNum].color);

	int i;
	int j;
	int x3, y3;
	struct Bullet * bullet;
	int count = 0;
	for (j = 0; j < NUM_SHIPS; j++) {
		bullet = ship[j].bullets;
		for (i = 0; i < NUM_BULLETS; i++) {
			if (bullet->drawn == 1) {
				for (jj = -1; jj <= 1; jj++) {
					x3 = bullet->x + LEVEL_WIDTH * jj;
					for (kk = -1; kk <= 1; kk++) {
						y3 = bullet->y + LEVEL_HEIGHT * kk;
						if ((GRRLIB_PtInRect(ship[shipNum].x - SCREEN_X / 4,
								ship[shipNum].y - SCREEN_Y / 2, SCREEN_X / 2,
								SCREEN_Y, x3, y3) && bullet->drawn)) {
							if (bullet->weaponType == 1) {
								GRRLIB_Line(x3 - x + offsetX, y3 - y + offsetY,
										x3 - x + offsetX + bullet->vx
												- ship[shipNum].vx, y3 - y
												+ offsetY + bullet->vy
												- ship[shipNum].vy,
										bullet->shiftedColor);
							}
							if (bullet->weaponType == 0) {
								GRRLIB_Rectangle(x3 - x + offsetX, y3 - y
										+ offsetY, 2, 2, bullet->shiftedColor,
										1);
							}
							if (bullet->weaponType == 2) {
								if (bullet->exploded == 0) {
									u32 color = bullet->state == 0 ? 0xFF0000FF
											: 0xFFFF00FF;
									bullet->state++;
									bullet->state %= 2;

									GRRLIB_Circle(x3 - x + offsetX, y3 - y
											+ offsetY, EXPLODER_RADIUS, color,
											1);
								} else if (bullet->exploded == 1) {
									int r;
									for (r = 0; r < NUM_EXPLODER; r++) {
										GRRLIB_Rectangle(x3 - x + offsetX
												+ bullet->subBullets[r].x, y3
												- y + offsetY
												+ bullet->subBullets[r].y, 3, 3,
												0xFFFFFFFF,1);
									}
								}
							}
							count++;
							jj = 2;
							kk = 2;
						}

					}
				}
			}
			bullet++;
		}
		if (j != shipNum) {
			ang2 = ship[j].ang;
			x2 = ship[j].x;
			y2 = ship[j].y;
			r2 = ship[j].r;
			p1x = cos(ang2) * r2 + x2 - x + offsetX;
			p1y = sin(ang2) * r2 + y2 - y + offsetY;
			p2x = cos(ang2 + PI * 2 / 3) * r2 + x2 - x + offsetX;
			p2y = sin(ang2 + PI * 2 / 3) * r2 + y2 - y + offsetY;
			p3x = cos(ang2 - PI * 3 / 4) * r2 + x2 - x + offsetX;
			p3y = sin(ang2 - PI * 3 / 4) * r2 + y2 - y + offsetY;
			for (kk = -1; kk <= 1; kk++) {
				for (jj = -1; jj <= 1; jj++) {
					GRRLIB_Line(p1x + kk * LEVEL_WIDTH,
							p1y + jj * LEVEL_HEIGHT, p2x + kk * LEVEL_WIDTH,
							p2y + jj * LEVEL_HEIGHT, ship[j].color);
					//GRRLIB_Line(p2x, p2y, p3x, p3y, ship->color);
					GRRLIB_Line(p3x + kk * LEVEL_WIDTH,
							p3y + jj * LEVEL_HEIGHT, p1x + kk * LEVEL_WIDTH,
							p1y + jj * LEVEL_HEIGHT, ship[j].color);
				}
			}
		}
	}
}

void renderPlanets(Planet *planet, int offsetX, int offsetY, float shipX,
		float shipY, Ship * ship) {
	int i, j, k, x, y, r, count = 0;
	double color;
	static int blarg = 0;
	for (i = 0; i < NUM_PLANETS; i++) {
		r = planet[i].r;
		for (j = -1; j <= 1; j++) {
			x = planet[i].x + LEVEL_WIDTH * j;
			for (k = -1; k <= 1; k++) {
				y = planet[i].y + LEVEL_HEIGHT * k;
				if (GRRLIB_PtInRect(shipX - SCREEN_X / 4 - r, shipY - SCREEN_Y
						/ 2 - r, SCREEN_X / 2 + 2 * r, SCREEN_Y + 2 * r, x, y)) {
					color = planet[i].owner == NO_OWNER ? planet[i].color
							: ship[planet[i].owner].color;
					GRRLIB_Circle(x + offsetX, y + offsetY, r, color, 1);
					if (planet[i].currentUpgrade != 0) {
						GRRLIB_Circle(x + offsetX, y + offsetY, r / 4,
								0xFFFFFFFF, 1);
					}
					count++;
					j = 2;
					k = 2;
				}

			}
		}
	}
}
void render(Ship*ship, Planet * planet) {
	int ii = 0;
	int jj = 0;
	int offsetX = 0;
	int offsetY = 0;
	warpGrid(ship, planet);
	GRRLIB_ClipReset();
	GRRLIB_Line(SCREEN_X / 2, ORIGIN_Y, SCREEN_X / 2, SCREEN_Y, BORDER_COLOR);
	for (ii = 0; ii < NUM_SHIPS; ii++) {

		if (ii == 0) {
			offsetX = ORIGIN_X;
			offsetY = ORIGIN_Y;

			GRRLIB_ClipDrawing(offsetX, offsetY, SCREEN_X / 2, SCREEN_Y);
		} else if (ii == 1) {
			offsetX = ORIGIN_X + SCREEN_X / 2;
			offsetY = ORIGIN_Y;
			GRRLIB_ClipDrawing(offsetX, offsetY, SCREEN_X, SCREEN_Y);
		}
		offsetX += ship[ii].scrollX;
		offsetY += ship[ii].scrollY;
		for (jj = 0; jj < NUM_SHIPS; jj++) {
			colorBullets(ship[jj].bullets, &ship[ii]);
		}
		renderShip(ship, offsetX, offsetY, ii);
		renderHealth(&ship[ii], ii * SCREEN_X / 2, ii);
		offsetX -= ship[ii].x;
		offsetY -= ship[ii].y;
		displayStars(offsetX, offsetY, &ship[ii]);
		drawGrid(offsetX, offsetY, ii);
		renderPlanets(planet, offsetX, offsetY, ship[ii].x, ship[ii].y, ship);
		drawPlanetBullets(planet, ship, offsetX, offsetY, ii);
	}
	GRRLIB_ClipReset();
	register int p1x;
	register int p1y;
	register int p2x;
	register int p2y;
	register int p3x;
	register int p3y;
	register float ang;
	register float x;
	register float y;
	int color;
	//make the map
	GRRLIB_Rectangle(MAP_OFFSET_X, MAP_OFFSET_Y, LEVEL_WIDTH / MAP_SCALE,
			LEVEL_HEIGHT / MAP_SCALE, 0x000000EE, 1);
	static int r = 2;
	for (ii = 0; ii < NUM_SHIPS; ii++) {
		ang = ship[ii].ang;
		x = ship[ii].x / MAP_SCALE + MAP_OFFSET_X;
		y = ship[ii].y / MAP_SCALE + MAP_OFFSET_Y;

		p1x = cos(ang) * r + x;
		p1y = sin(ang) * r + y;
		p2x = cos(ang + PI * 2 / 3) * r + x;
		p2y = sin(ang + PI * 2 / 3) * r + y;
		p3x = cos(ang - PI * 3 / 4) * r + x;
		p3y = sin(ang - PI * 3 / 4) * r + y;
		GRRLIB_Line(p1x, p1y, p2x, p2y, ship[ii].color);
		//GRRLIB_Line(p2x, p2y, p3x, p3y, ship->color);
		GRRLIB_Line(p3x, p3y, p1x, p1y, ship[ii].color);
	}
	for (ii = 0; ii < NUM_PLANETS; ii++) {
		x = planet[ii].x / MAP_SCALE + MAP_OFFSET_X;
		y = planet[ii].y / MAP_SCALE + MAP_OFFSET_Y;
		r = planet[ii].r / MAP_SCALE;
		color = planet[ii].owner == NO_OWNER ? planet[ii].color
				: ship[planet[ii].owner].color;
		GRRLIB_Circle(x, y, r, color, 1);
	}
	//draws border around the mini-map
	GRRLIB_Line(MAP_OFFSET_X, MAP_OFFSET_Y, LEVEL_WIDTH / MAP_SCALE
			+ MAP_OFFSET_X, MAP_OFFSET_Y, 0xFFFFFFFF);
	GRRLIB_Line(MAP_OFFSET_X, MAP_OFFSET_Y, MAP_OFFSET_X, LEVEL_HEIGHT
			/ MAP_SCALE + MAP_OFFSET_Y, 0xFFFFFFFF);
	GRRLIB_Line(LEVEL_WIDTH / MAP_SCALE + MAP_OFFSET_X, MAP_OFFSET_Y,
			LEVEL_WIDTH / MAP_SCALE + MAP_OFFSET_X, LEVEL_HEIGHT / MAP_SCALE
					+ MAP_OFFSET_Y, 0xFFFFFFFF);
	GRRLIB_Line(MAP_OFFSET_X, LEVEL_HEIGHT / MAP_SCALE + MAP_OFFSET_Y,
			LEVEL_WIDTH / MAP_SCALE + MAP_OFFSET_X, LEVEL_HEIGHT / MAP_SCALE
					+ MAP_OFFSET_Y, 0xFFFFFFFF);
}

// changes the color of bullets based its relative speed to a ship.
void colorBullets(struct Bullet * bullet, Ship * ship) { // arguments refer to a set of bullets and the ship we are coloring them for
	int i = 0;
	//u64 time = ticks_to_millisecs(gettime());
	int red;
	int green;
	int blue;
	//int alpha;
	float dDist;
	int relVX;
	int relVY;
	int relX;
	int relY;
	for (i = 0; i < NUM_BULLETS; i++) {
		if (GRRLIB_PtInRect(ship->x - SCREEN_X / 4, ship->y - SCREEN_Y / 2,
				SCREEN_X / 2, SCREEN_Y, bullet->x, bullet->y)) {
			red = R(bullet->baseColor);
			green = G(bullet->baseColor);
			blue = B(bullet->baseColor);
			//alpha = A(bullet->baseColor);
			relVX = bullet->vx - ship->vx;
			relVY = bullet->vy - ship->vy;
			relX = bullet->x - ship->x;
			relY = bullet->y - ship->y;
			//		dDist = (relX * relVX + relY * relVY) / sqrt(pow(relX, 2)
			//				+ pow(relY, 2)); // rate of change of the distance between the two things
			dDist = (relX * relVX + relY * relVY) / babylonianSqrt(relX * relX
					+ relY * relY); // rate of change of the distance between the two things
			red = red + dDist * RED_SHIFT_FACTOR;
			green = green - 2 * dDist * GREEN_SHIFT_FACTOR;
			blue = blue - dDist * BLUE_SHIFT_FACTOR;
			red = red > 0xFF ? 0xFF : red < 0 ? 0 : red;
			green = green > 0xFF ? 0xFF : green < 0 ? 0 : green;
			blue = blue > 0xFF ? 0xFF : blue < 0 ? 0 : blue;

			//alpha = alpha - (time - bullet->milli) * 255 / BULLET_LIFE;
			//alpha = alpha > 0xFF ? 0xFF : alpha < 0 ? 0 : alpha;
			//if (alpha == 0)
			//bullet->drawn = 0;
			bullet->shiftedColor = RGBA(red, green, blue, 0xFF);
			bullet++;
		}
	}
}

void renderHealth(Ship * ship, int offsetX, int shipNum) {
	int root = shipNum == 0 ? 20 : SCREEN_X - 20;
	int sign = shipNum == 0 ? 1 : -1;
	GRRLIB_Rectangle(root, 450, sign * ship->health * HEALTH_BAR_SCALE, 20,
			ship->color, 1);
}

void drawPlanetBullets(Planet* planets, Ship * ships, int offsetX, int offsetY,
		int shipNum) {
	int i, j, jj, kk;
	float x3, y3;

	for (j = 0; j < NUM_PLANETS; j++) {
		struct Bullet * bullet = planets[j].bullets;
		for (i = 0; i < PLANET_BULLET_NUM; i++) {
			if (bullet->drawn == 1) {
				for (jj = -1; jj <= 1; jj++) {
					x3 = bullet->x + LEVEL_WIDTH * jj;
					for (kk = -1; kk <= 1; kk++) {
						y3 = bullet->y + LEVEL_HEIGHT * kk;
						if (GRRLIB_PtInRect(ships[shipNum].x - SCREEN_X / 4,
								ships[shipNum].y - SCREEN_Y / 2, SCREEN_X / 2,
								SCREEN_Y, x3, y3) && bullet->drawn) {
							GRRLIB_Rectangle(x3 + offsetX, y3 + offsetY, 2, 2,
									0xFFFFFFFF, 1);
							jj = 2;
							kk = 2;
						}

					}
				}

			}
			bullet++;
		}
	}
}
