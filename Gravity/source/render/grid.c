/*
 * grid.c
 *
 *  Created on: Mar 23, 2011
 *      Author: hopwoocp
 */
#include <grrlib.h>
#include <math.h>
#include "grid.h"
#include "../definitions.h"
#include <stdlib.h>
#define NUM_WIDTH 30
#define NUM_HEIGHT 30
#define SPACING 60
#define NUM_POINTS NUM_WIDTH*NUM_HEIGHT
#define SPRING_CONSTANT 0
#define DAMPING_CONSTANT_SPRING 1
#define MAX_DEVIATION 20
#define GRID_MASS 1000000

#define LARGE_NUMBER  326973 // a very large number given to me by my roommate.
GridPoint *gridPoints;
int indexGrid(int x, int y) {
	return (x + y * NUM_WIDTH);
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
			gridPoints[index].x = SPACING * x;
			gridPoints[index].y = SPACING * y;
			gridPoints[index].dx = 0;
			gridPoints[index].dy = 0;
			count++;
		}
	}
}

void drawGrid(int offsetX, int offsetY) {
	int x, y;
	for (x = 0; x < NUM_WIDTH - 1; x++) {
		for (y = 0; y < NUM_HEIGHT - 1; y++) {
			GRRLIB_Line(gridPoints[indexGrid(x, y)].x + gridPoints[indexGrid(x,
					y)].dx + offsetX, gridPoints[indexGrid(x, y)].y
					+ gridPoints[indexGrid(x, y)].dy + offsetY,
					gridPoints[indexGrid(x + 1, y)].x + gridPoints[indexGrid(x
							+ 1, y)].dx + offsetX, gridPoints[indexGrid(x + 1,
							y)].y + gridPoints[indexGrid(x + 1, y)].dy
							+ offsetY, 0xFFFFFF44);
			GRRLIB_Line(gridPoints[indexGrid(x, y)].x + gridPoints[indexGrid(x,
					y)].dx + offsetX, gridPoints[indexGrid(x, y)].y
					+ gridPoints[indexGrid(x, y)].dy + offsetY,
					gridPoints[indexGrid(x, y + 1)].x + gridPoints[indexGrid(x,
							y + 1)].dx + offsetX,
					gridPoints[indexGrid(x, y + 1)].y + gridPoints[indexGrid(x,
							y + 1)].dy + offsetY, 0xFFFFFF44);

		}
	}
}
double warpGrid(Ship * ships, Planet * planets) {
	int x, y;
	int shipCount;
	for (x = 1; x < NUM_WIDTH - 1; x++) {
		for (y = 1; y < NUM_HEIGHT - 1; y++) {
			gridPoints[indexGrid(x, y)].dx = 0;
			gridPoints[indexGrid(x, y)].dy = 0;
			float dx1, dy1, r;
			float dvx = 0;
			float dvy = 0;
			// distance to the point to the right
			for (shipCount = 0; shipCount < NUM_SHIPS; shipCount++) {
				float dv;
				dx1 = gridPoints[indexGrid(x, y)].x
						+ gridPoints[indexGrid(x, y)].dx - ships[shipCount].x;
				dy1 = gridPoints[indexGrid(x, y)].y
						+ gridPoints[indexGrid(x, y)].dy - ships[shipCount].y;
				r = hypotf(dx1, dy1);
				dv = GRID_MASS / pow(r, 2);
				dvx += dv * dx1 / r;
				dvy += dv * dy1 / r;

			}
			//dv = hypot(dvx,dvy);
			gridPoints[indexGrid(x, y)].dx -= dvx;

			gridPoints[indexGrid(x, y)].dy -= dvy;

			int closestShip = NUM_SHIPS;
			int closestDistance = LARGE_NUMBER; // A very large number given to me by my roommate
			for (shipCount = 0; shipCount < NUM_SHIPS; shipCount++) {
				float xx = gridPoints[indexGrid(x, y)].x
						+ gridPoints[indexGrid(x, y)].dx - ships[shipCount].x;
				float yy = gridPoints[indexGrid(x, y)].y
						+ gridPoints[indexGrid(x, y)].dy - ships[shipCount].y;
				float xx2 = gridPoints[indexGrid(x, y)].x - ships[shipCount].x;
				float yy2 = gridPoints[indexGrid(x, y)].y - ships[shipCount].y;

				//float r1 = hypotf(xx, yy); // distance from deviated grid to ship
				float r2 = hypotf(xx2, yy2); // distance from absolute grid to ship
				// ammount of deviation:
				float r3 = hypotf(gridPoints[indexGrid(x, y)].dx,
						gridPoints[indexGrid(x, y)].dy);

				if (r3 > r2 && closestDistance > r2) {
					closestDistance = r2;
					closestShip = shipCount;
				}

			}

			if (closestShip < NUM_SHIPS && closestDistance != LARGE_NUMBER) {
				gridPoints[indexGrid(x, y)].dx = ships[closestShip].x
						- gridPoints[indexGrid(x, y)].x;
				gridPoints[indexGrid(x, y)].dy = ships[closestShip].y
						- gridPoints[indexGrid(x, y)].y;
			}
		}
	}

}
