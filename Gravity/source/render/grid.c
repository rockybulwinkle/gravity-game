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
#define NUM_WIDTH 64.0
#define NUM_HEIGHT 48.0
#define NUM_POINTS NUM_WIDTH*NUM_HEIGHT

#define K 2

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
	float tempX, tempY;
	for (x = 0; x < NUM_WIDTH; x++) {
		for (y = 0; y < NUM_HEIGHT; y++) {
			index = indexGrid(x, y);
			gridPoints[index].x = 20 * x;
			gridPoints[index].y = 20 * y;
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
					+ gridPoints[indexGrid(x, y)].dy + offsetY, gridPoints[indexGrid(x + 1, y)].x
					+ gridPoints[indexGrid(x + 1, y)].dx+offsetX,
					gridPoints[indexGrid(x + 1, y)].y
							+ gridPoints[indexGrid(x + 1, y)].dy+offsetY, 0xFFFFFF11);
			GRRLIB_Line(gridPoints[indexGrid(x, y)].x + gridPoints[indexGrid(x,
					y)].dx+offsetX, gridPoints[indexGrid(x, y)].y
					+ gridPoints[indexGrid(x, y)].dy+offsetY, gridPoints[indexGrid(x, y
					+ 1)].x + gridPoints[indexGrid(x, y + 1)].dx+offsetX,
					gridPoints[indexGrid(x, y + 1)].y + gridPoints[indexGrid(x,
							y + 1)].dy+offsetY, 0xFFFFFF11);

		}
	}
}
void warpGrid(Planet * ships){};
