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
#define NUM_WIDTH 40
#define NUM_HEIGHT 40
#define SPACING LEVEL_HEIGHT/NUM_WIDTH
#define NUM_POINTS NUM_WIDTH*NUM_HEIGHT
#define SPRING_CONSTANT 0
#define DAMPING_CONSTANT_SPRING 1
#define MAX_DEVIATION 20
#define GRID_MASS 10

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
			gridPoints[index].vx = 0;
			gridPoints[index].vy = 0;
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
							+ offsetY, 0xFFFFFFFF);
			GRRLIB_Line(gridPoints[indexGrid(x, y)].x + gridPoints[indexGrid(x,
					y)].dx + offsetX, gridPoints[indexGrid(x, y)].y
					+ gridPoints[indexGrid(x, y)].dy + offsetY,
					gridPoints[indexGrid(x, y + 1)].x + gridPoints[indexGrid(x,
							y + 1)].dx + offsetX,
					gridPoints[indexGrid(x, y + 1)].y + gridPoints[indexGrid(x,
							y + 1)].dy + offsetY, 0xFFFFFFFF);

		}
	}
}
double warpGrid(Ship * ships) {
	int x, y;
	int shipCount;
	for (x = 1; x < NUM_WIDTH - 1; x++) {
		for (y = 1; y < NUM_HEIGHT - 1; y++) {
			// distance to the point to the right
			double dist1 = hypot(gridPoints[indexGrid(x, y)].x
					+ gridPoints[indexGrid(x, y)].dx - gridPoints[indexGrid(x
					+ 1, y)].x - gridPoints[indexGrid(x + 1, y)].dx, /**/
			gridPoints[indexGrid(x, y)].y + gridPoints[indexGrid(x, y)].dy
					- gridPoints[indexGrid(x + 1, y)].y - gridPoints[indexGrid(
					x + 1, y)].dy) / SPACING - 1;
			// distance to the point below
			double dist2 = hypot(gridPoints[indexGrid(x, y)].x
					+ gridPoints[indexGrid(x, y)].dx - gridPoints[indexGrid(x,
					y + 1)].x - gridPoints[indexGrid(x, y + 1)].dx, /**/
			gridPoints[indexGrid(x, y)].y + gridPoints[indexGrid(x, y)].dy
					- gridPoints[indexGrid(x, y + 1)].y - gridPoints[indexGrid(
					x, y + 1)].dy) / SPACING - 1;
			// distance to the point to the left
			double dist3 = hypot(gridPoints[indexGrid(x, y)].x
					+ gridPoints[indexGrid(x, y)].dx - gridPoints[indexGrid(x
					- 1, y)].x - gridPoints[indexGrid(x - 1, y)].dx, /**/
			gridPoints[indexGrid(x, y)].y + gridPoints[indexGrid(x, y)].dy
					- gridPoints[indexGrid(x - 1, y)].y - gridPoints[indexGrid(
					x - 1, y)].dy) / SPACING - 1;
			// distance to the point above
			double dist4 = hypot(gridPoints[indexGrid(x, y)].x
					+ gridPoints[indexGrid(x, y)].dx - gridPoints[indexGrid(x,
					y - 1)].x - gridPoints[indexGrid(x, y - 1)].dx, /**/
			gridPoints[indexGrid(x, y)].y + gridPoints[indexGrid(x, y)].dy
					- gridPoints[indexGrid(x, y - 1)].y - gridPoints[indexGrid(
					x, y - 1)].dy) / SPACING - 1;

			for (shipCount = 0; shipCount < NUM_SHIPS; shipCount++) {
				float xx = gridPoints[indexGrid(x, y)].x
						+ gridPoints[indexGrid(x, y)].dx - ships[shipCount].x;
				float yy = gridPoints[indexGrid(x, y)].y
						+ gridPoints[indexGrid(x, y)].dy - ships[shipCount].y;
				float r = hypotf(xx, yy);
				float dv = GRID_MASS / pow(r, 2);
				gridPoints[indexGrid(x, y)].dx = GRID_MASS * dv * xx / r;
				;
				gridPoints[indexGrid(x, y)].dy = GRID_MASS * dv * yy / r;
				;
				if (hypot(xx, yy) < 1) {
					gridPoints[indexGrid(x, y)].vx = 0;
					gridPoints[indexGrid(x, y)].vy = 0;
				}
			}
			/*gridPoints[indexGrid(x, y)].vx = (gridPoints[indexGrid(x, y)].vx
					+ (dist1 - dist3) * SPRING_CONSTANT)
					* DAMPING_CONSTANT_SPRING;
			gridPoints[indexGrid(x, y)].vy = (gridPoints[indexGrid(x, y)].vy
					+ (dist2 - dist4) * SPRING_CONSTANT)
					* DAMPING_CONSTANT_SPRING;

			gridPoints[indexGrid(x, y)].dx += gridPoints[indexGrid(x, y)].vx;
			gridPoints[indexGrid(x, y)].dy += gridPoints[indexGrid(x, y)].vy;
			*//*
			if (hypot(gridPoints[indexGrid(x, y)].dx,
					gridPoints[indexGrid(x, y)].dy) > MAX_DEVIATION) {

				int r = hypot(gridPoints[indexGrid(x, y)].dx,
						gridPoints[indexGrid(x, y)].dy);
				gridPoints[indexGrid(x, y)].dx = gridPoints[indexGrid(x, y)].dx/r * MAX_DEVIATION;
				gridPoints[indexGrid(x, y)].dy = gridPoints[indexGrid(x, y)].dy/r * MAX_DEVIATION;
				gridPoints[indexGrid(x, y)].vx = 0;
				gridPoints[indexGrid(x, y)].vy = 0;
			}
			*/
		}
	}

}
