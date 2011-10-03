/*
 * grid.c
 *
 *  Created on: Mar 23, 2011
 *      Author: hopwoocp
 */
#include <grrlib.h>
#include <math.h>
#include "../definitions.h"
#include "grid.h"

#include <stdlib.h>
#include "../physicsFuncs/physics.h"

extern GRRLIB_texImg *tex_Calibri;

#define NUM_WIDTH 100
#define NUM_HEIGHT 100
#define X_SPACING (LEVEL_WIDTH/NUM_WIDTH)
#define Y_SPACING (LEVEL_HEIGHT/NUM_HEIGHT)
#define NUM_POINTS (NUM_WIDTH*NUM_HEIGHT)
#define GRID_MASS 1000
#define GRID_COLOR 0xFFFFFF50//0xFFFFFF33
#define LARGE_NUMBER  326973 // a very large number given to me by my roommate.  Really just needs to be bigger than the level's width and height... not sure why I didn't set it to LEVEL_WIDTH + 1, really.  W/e, it is doing fine for now.
GridPoint *gridPoints;

int inline indexGrid(int x, int y) {
	return ((x + 2 * NUM_WIDTH) % NUM_WIDTH + ((y + 2 * NUM_HEIGHT)
			% NUM_HEIGHT) * NUM_WIDTH);
}

void initializeGrid() {
	gridPoints = (GridPoint*) malloc(NUM_POINTS * sizeof(GridPoint));

	int x, y;

	if (gridPoints == NULL) {
		exit(0);
	}

	int count = 0;
	int index;
	for (x = 0; x < NUM_WIDTH; x++) {
		for (y = 0; y < NUM_HEIGHT; y++) {
			index = indexGrid(x, y);
			gridPoints[index].x = X_SPACING * x;
			gridPoints[index].y = X_SPACING * y;
			gridPoints[index].dx = 0;
			gridPoints[index].dy = 0;
			count++;
		}
	}
}

void drawGrid(int offsetX, int offsetY, int shipNum) {
	int x, y;
	int minX = (-offsetX + shipNum * SCREEN_X / 2) / X_SPACING - 2;
	int maxX = minX + (SCREEN_X / 2) / X_SPACING + 4;

	int minY = (-offsetY) / Y_SPACING - 2;
	int maxY = minY + SCREEN_Y / Y_SPACING + 4;
	for (x = minX; x < maxX; x++) {
		for (y = minY; y < maxY; y++) {
			float p1x = gridPoints[indexGrid(x, y)].x + gridPoints[indexGrid(x,
					y)].dx + offsetX;
			float p1y = gridPoints[indexGrid(x, y)].y + gridPoints[indexGrid(x,
					y)].dy + offsetY;
			p1x = (x < 0) ? p1x - LEVEL_WIDTH : (x > NUM_WIDTH - 1) ? p1x
					+ LEVEL_WIDTH : p1x;
			p1y = (y < 0) ? p1y - LEVEL_HEIGHT : (y > NUM_HEIGHT - 1) ? p1y
					+ LEVEL_HEIGHT : p1y;
			float p2x = gridPoints[indexGrid(x + 1, y)].x
					+ gridPoints[indexGrid(x + 1, y)].dx + offsetX;
			float p2y = gridPoints[indexGrid(x + 1, y)].y
					+ gridPoints[indexGrid(x + 1, y)].dy + offsetY;
			p2x = (x + 1 < 0) ? p2x - LEVEL_WIDTH : (x > NUM_WIDTH - 2) ? p2x
					+ LEVEL_WIDTH : p2x;
			p2y = (y < 0) ? p2y - LEVEL_HEIGHT : (y > NUM_HEIGHT - 1) ? p2y
					+ LEVEL_HEIGHT : p2y;

			float p3x = gridPoints[indexGrid(x, y + 1)].x
					+ gridPoints[indexGrid(x, y + 1)].dx + offsetX;
			float p3y = gridPoints[indexGrid(x, y + 1)].y
					+ gridPoints[indexGrid(x, y + 1)].dy + offsetY;
			p3x = (x < 0) ? p3x - LEVEL_WIDTH : (x > NUM_WIDTH - 1) ? p3x
					+ LEVEL_WIDTH : p3x;
			p3y = (y + 1 < 0) ? p3y - LEVEL_HEIGHT
					: (y + 1 > NUM_HEIGHT - 1) ? p3y + LEVEL_HEIGHT : p3y;
			GRRLIB_Line(p1x, p1y, p2x, p2y, GRID_COLOR);
			GRRLIB_Line(p1x, p1y, p3x, p3y, GRID_COLOR);
		}

	}
}
inline int sign(int x) {
	return (x > 0) ? 1 : (x == 0) ? 0 : -1;
}
void warpGrid(Ship * ships, Planet * planets) {
	int x, y;
	int shipCount;
	int viewportNum;
	for (viewportNum = 0; viewportNum < NUM_SHIPS; viewportNum++) {
		int minX = (ships[viewportNum].x - SCREEN_X / 4) / X_SPACING - 1;
		int maxX = minX + (SCREEN_X / 2) / X_SPACING + 2;
		int minY = (ships[viewportNum].y - SCREEN_Y / 2) / Y_SPACING - 1;
		int maxY = minY + SCREEN_Y / Y_SPACING + 2;

		for (x = minX; x < maxX; x++) {
			for (y = minY; y < maxY; y++) {

				gridPoints[indexGrid(x, y)].dx = 0;
				gridPoints[indexGrid(x, y)].dy = 0;

				int planetCount;
				// distance to the point to the right
				for (shipCount = 0; shipCount < NUM_SHIPS; shipCount++) {
					gravitateGrid(gridPoints[indexGrid(x, y)].x,
							gridPoints[indexGrid(x, y)].y,
							&gridPoints[indexGrid(x, y)].dx,
							&gridPoints[indexGrid(x, y)].dy,
							ships[shipCount].x, ships[shipCount].y, GRID_MASS
									* 50);
				}
				for (planetCount = 0; planetCount < NUM_PLANETS; planetCount++) {

					gravitateGrid(gridPoints[indexGrid(x, y)].x,
							gridPoints[indexGrid(x, y)].y,
							&gridPoints[indexGrid(x, y)].dx,
							&gridPoints[indexGrid(x, y)].dy,
							planets[planetCount].x, planets[planetCount].y,
							planets[planetCount].m * GRID_MASS);
				}
			}
		}
	}

}

void closeGrid() {
	free(gridPoints);
}
//void warpGrid(Ship * ships, Planet * planets) {
//	int x, y;
//	int shipCount;
//	for (x = 0; x < NUM_WIDTH; x++) {
//		for (y = 0; y < NUM_HEIGHT; y++) {
//			if (GRRLIB_PtInRect(ships[0].x - SCREEN_X / 4, ships[0].y
//					- SCREEN_Y / 2, SCREEN_X / 2, SCREEN_Y,
//					gridPoints[indexGrid(x, y)].x
//							+ gridPoints[indexGrid(x, y)].dx,
//					gridPoints[indexGrid(x, y)].y
//							+ gridPoints[indexGrid(x, y)].dy)
//					|| GRRLIB_PtInRect(ships[1].x - SCREEN_X / 4, ships[1].y
//							- SCREEN_Y / 2, SCREEN_X / 2, SCREEN_Y,
//							gridPoints[indexGrid(x, y)].x
//									+ gridPoints[indexGrid(x, y)].dx,
//							gridPoints[indexGrid(x, y)].y
//									+ gridPoints[indexGrid(x, y)].dy)) {
//
//				gridPoints[indexGrid(x, y)].dx = 0;
//				gridPoints[indexGrid(x, y)].dy = 0;
//				float dx1, dy1, r;
//				float dvx = 0;
//				float dvy = 0;
//				int planetCount;
//				// distance to the point to the right
//				for (shipCount = 0; shipCount < NUM_SHIPS; shipCount++) {
//					float dv;
//					dx1 = gridPoints[indexGrid(x, y)].x + gridPoints[indexGrid(
//							x, y)].dx - ships[shipCount].x;
//					dy1 = gridPoints[indexGrid(x, y)].y + gridPoints[indexGrid(
//							x, y)].dy - ships[shipCount].y;
//					r = hypotf(dx1, dy1);
//					dv = GRID_MASS / pow(r, 2);
//					dvx += dv * dx1 / r;
//					dvy += dv * dy1 / r;
//
//				}
//				for (planetCount = 0; planetCount < NUM_PLANETS; planetCount++) {
//					float dv;
//					dx1 = gridPoints[indexGrid(x, y)].x + gridPoints[indexGrid(
//							x, y)].dx - planets[planetCount].x;
//					dy1 = gridPoints[indexGrid(x, y)].y + gridPoints[indexGrid(
//							x, y)].dy - planets[planetCount].y;
//					r = hypotf(dx1, dy1);
//					dv = planets[planetCount].m * GRID_MASS / pow(r, 2) / 50;
//					if (r != 0) {
//						dvx += dv * dx1 / r;
//						dvy += dv * dy1 / r;
//					}
//				}
//
//				gridPoints[indexGrid(x, y)].dx -= dvx;
//
//				gridPoints[indexGrid(x, y)].dy -= dvy;
//
//				int closestPlanet = NUM_PLANETS;
//				int closestPlanetDistance = LARGE_NUMBER; // A very large number given to me by my roommate
//
//				for (planetCount = 0; planetCount < NUM_PLANETS; planetCount++) {
//					/*float xx = gridPoints[indexGrid(x, y)].x
//					 + gridPoints[indexGrid(x, y)].dx
//					 - planets[planetCount].x;
//					 float yy = gridPoints[indexGrid(x, y)].y
//					 + gridPoints[indexGrid(x, y)].dy
//					 - planets[planetCount].y;*/
//					float xx2 = gridPoints[indexGrid(x, y)].x
//							- planets[planetCount].x;
//					float yy2 = gridPoints[indexGrid(x, y)].y
//							- planets[planetCount].y;
//
//					float r2 = hypotf(xx2, yy2); // distance from absolute grid to ship
//					// ammount of deviation:
//					float r3 = hypotf(gridPoints[indexGrid(x, y)].dx,
//							gridPoints[indexGrid(x, y)].dy);
//
//					if (r3 > r2 && closestPlanetDistance > r2) {
//						closestPlanetDistance = r2;
//						closestPlanet = planetCount;
//					}
//
//				}
//
//				if (closestPlanet < NUM_PLANETS && closestPlanetDistance
//						!= LARGE_NUMBER) {
//					gridPoints[indexGrid(x, y)].dx = planets[closestPlanet].x
//							- gridPoints[indexGrid(x, y)].x;
//					gridPoints[indexGrid(x, y)].dy = planets[closestPlanet].y
//							- gridPoints[indexGrid(x, y)].y;
//				}
//
//				int closestShip = NUM_SHIPS;
//				int closestDistance = LARGE_NUMBER; // A very large number given to me by my roommate
//				for (shipCount = 0; shipCount < NUM_SHIPS; shipCount++) {
//					/*float xx = gridPoints[indexGrid(x, y)].x
//					 + gridPoints[indexGrid(x, y)].dx
//					 - ships[shipCount].x;
//					 float yy = gridPoints[indexGrid(x, y)].y
//					 + gridPoints[indexGrid(x, y)].dy
//					 - ships[shipCount].y;*/
//					float xx2 = gridPoints[indexGrid(x, y)].x
//							- ships[shipCount].x;
//					float yy2 = gridPoints[indexGrid(x, y)].y
//							- ships[shipCount].y;
//
//					//float r1 = hypotf(xx, yy); // distance from deviated grid to ship
//					float r2 = hypotf(xx2, yy2); // distance from absolute grid to ship
//					// ammount of deviation:
//					float r3 = hypotf(gridPoints[indexGrid(x, y)].dx,
//							gridPoints[indexGrid(x, y)].dy);
//
//					if (r3 > r2 && closestDistance > r2) {
//						closestDistance = r2;
//						closestShip = shipCount;
//					}
//
//				}
//
//				if (closestShip < NUM_SHIPS && closestDistance != LARGE_NUMBER) {
//					gridPoints[indexGrid(x, y)].dx = ships[closestShip].x
//							- gridPoints[indexGrid(x, y)].x;
//					gridPoints[indexGrid(x, y)].dy = ships[closestShip].y
//							- gridPoints[indexGrid(x, y)].y;
//				}
//			}
//		}
//	}
//
//}
