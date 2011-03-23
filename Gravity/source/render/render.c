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
#include "../FPS/fps.h"
#include "../VectorCalculus/vector.h"
#include <ogc/lwp_watchdog.h>
#include "../staryBackground/stars.h"
//#include "../fonts/Calibri_18.h"
extern GRRLIB_texImg * tex_Calibri;
void renderShip(Ship * ship, int offsetX, int offsetY, int shipNum) {
	//my ships position - object position + offset
	//this code is very sloppy, needs revision to be more concise
	int jj, kk;
	register int p1x;
	register int p1y;
	register int p2x;
	register int p2y;
	register int p3x;
	register int p3y;
	register float ang = ship[shipNum].ang;
	register int x = ship[shipNum].x;
	register int y = ship[shipNum].y;
	register int r = ship[shipNum].r;
	register float ang2;
	register int x2;
	register int y2;
	register int r2;
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
	Bullet * bullet;
	int count = 0;
	for (j = 0; j < NUM_SHIPS; j++) {
		bullet = ship[j].bullets;
		profiler(1);
		for (i = 0; i < NUM_BULLETS; i++) {
			if (bullet->drawn == 1) {
				/*
				 v = sqrt(pow((bullet+i)->vx, 2) + pow(
				 (bullet+i)->vy, 2));
				 color = 0xFFFFFFFF - (v + 1) * 0x00222200;
				 */
				//GRRLIB_Plot(ship->bullets[i].x, ship->bullets[i].y, color);

				//GRRLIB_Plot(bullet->x - x + offsetX, bullet->y - y + offsetY,
				//bullet->shiftedColor);
				/////

				for (jj = -1; jj <= 1; jj++) {
					x3 = bullet->x + LEVEL_WIDTH * jj;
					for (kk = -1; kk <= 1; kk++) {
						y3 = bullet->y + LEVEL_HEIGHT * kk;
						if (GRRLIB_PtInRect(ship[shipNum].x - SCREEN_X / 4,
								ship[shipNum].y - SCREEN_Y / 2, SCREEN_X / 2,
								SCREEN_Y, x3, y3) && bullet->drawn) {
							GRRLIB_Rectangle(x3 - x + offsetX,
									y3 - y + offsetY, 2, 2,
									bullet->shiftedColor, 1);
							count++;
							jj = 2;
							kk = 2;
						}

					}
				}

				/////
				//GRRLIB_Line(ship->bullets[i].x, ship->bullets[i].y, ship->bullets[i].vx*BULLET_LENGTH+ship->bullets[i].x, ship->bullets[i].vy*BULLET_LENGTH+ship->bullets[i].y, color);
			}
			bullet++;
		}
		GRRLIB_Printf(20, 140 + j * 20, tex_Calibri, 0xFFFFFFFF
				* ENABLE_DEBUGGING, .7, "bullet Render Time: %d", profiler(0));
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
			GRRLIB_Line(p1x, p1y, p2x, p2y, ship[j].color);
			//GRRLIB_Line(p2x, p2y, p3x, p3y, ship->color);
			GRRLIB_Line(p3x, p3y, p1x, p1y, ship[j].color);
		}
	}
	GRRLIB_Printf(20, 200, tex_Calibri, 0xFFFFFFFF * ENABLE_DEBUGGING, .5,
			"%d", count);
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
					GRRLIB_Circle(x + offsetX, y + offsetY, r, color, 0);
					count++;
					j = 2;
					k = 2;
				}

			}
		}
	}
	GRRLIB_Printf(20 + blarg * SCREEN_X / 2, 180, tex_Calibri, 0xFFFFFFFF
			* ENABLE_DEBUGGING, .5, "%d", count);
	blarg++;
	blarg = blarg == 2 ? 0 : blarg;
}
void render(Ship*ship, Planet * planet) {
	int ii = 0;
	int jj = 0;
	int offsetX = 0;
	int offsetY = 0;

	GRRLIB_ClipReset();
	GRRLIB_Line(SCREEN_X / 2, ORIGIN_Y, SCREEN_X / 2, SCREEN_Y, BORDER_COLOR);
	for (ii = 0; ii < NUM_SHIPS; ii++) {
		GRRLIB_Settings.antialias = ii;

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
		renderPlanets(planet, offsetX, offsetY, ship[ii].x, ship[ii].y, ship);
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
	GRRLIB_Rectangle(MAP_OFFSET_X, MAP_OFFSET_Y, LEVEL_WIDTH/MAP_SCALE, LEVEL_HEIGHT/MAP_SCALE, 0x000000EE, 1);
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
		GRRLIB_Circle(x, y, r, color, 0);
	}
	//draws border around the mini-map
	GRRLIB_Line(MAP_OFFSET_X, MAP_OFFSET_Y, LEVEL_WIDTH/MAP_SCALE+MAP_OFFSET_X, MAP_OFFSET_Y, 0xFFFFFFFF);
	GRRLIB_Line(MAP_OFFSET_X, MAP_OFFSET_Y, MAP_OFFSET_X, LEVEL_HEIGHT/MAP_SCALE+MAP_OFFSET_Y, 0xFFFFFFFF);
	GRRLIB_Line(LEVEL_WIDTH/MAP_SCALE+MAP_OFFSET_X, MAP_OFFSET_Y, LEVEL_WIDTH/MAP_SCALE+MAP_OFFSET_X, LEVEL_HEIGHT/MAP_SCALE+MAP_OFFSET_Y, 0xFFFFFFFF);
	GRRLIB_Line(MAP_OFFSET_X, LEVEL_HEIGHT/MAP_SCALE+MAP_OFFSET_Y, LEVEL_WIDTH/MAP_SCALE+MAP_OFFSET_X, LEVEL_HEIGHT/MAP_SCALE+MAP_OFFSET_Y, 0xFFFFFFFF);
}

// changes the color of bullets based its relative speed to a ship.
void colorBullets(Bullet * bullet, Ship * ship) { // arguments refer to a set of bullets and the ship we are coloring them for
	int i = 0;
	int red;
	int green;
	int blue;
	int alpha;
	float dDist;
	int relVX;
	int relVY;
	int relX;
	int relY;
	u64 time = ticks_to_millisecs(gettime());
	for (i = 0; i < NUM_BULLETS; i++) {
		red = R(bullet->baseColor);
		green = G(bullet->baseColor);
		blue = B(bullet->baseColor);
		alpha = A(bullet->baseColor);
		relVX = bullet->vx - ship->vx;
		relVY = bullet->vy - ship->vy;
		relX = bullet->x - ship->x;
		relY = bullet->y - ship->y;
		dDist = (relX * relVX + relY * relVY) / sqrt(pow(relX, 2)
				+ pow(relY, 2)); // rate of change of the distance between the two things
		red = red + dDist * RED_SHIFT_FACTOR;
		green = green - 2 * dDist * GREEN_SHIFT_FACTOR;
		blue = blue - dDist * BLUE_SHIFT_FACTOR;
		red = red > 0xFF ? 0xFF : red < 0 ? 0 : red;
		green = green > 0xFF ? 0xFF : green < 0 ? 0 : green;
		blue = blue > 0xFF ? 0xFF : blue < 0 ? 0 : blue;

		alpha = alpha - (time - bullet->milli) * 255 / BULLET_LIFE;
		alpha = alpha > 0xFF ? 0xFF : alpha < 0 ? 0 : alpha;
		if (alpha == 0)
			bullet->drawn = 0;
		bullet->shiftedColor = RGBA(red, green, blue, alpha);
		bullet++;
	}
}

void renderHealth(Ship * ship, int offsetX, int shipNum) {
	int root = shipNum == 0? 20:SCREEN_X-20;
	int sign = shipNum == 0? 1:-1;
	GRRLIB_Rectangle(root, 450, sign*ship->health * HEALTH_BAR_SCALE, 20,
			ship->color, 1);
}


