/*
 * grid.h
 *
 *  Created on: Mar 23, 2011
 *      Author: hopwoocp
 */

#ifndef GRID_H_
#define GRID_H_


#endif /* GRID_H_ */
int indexGrid(int x, int y);
void drawGrid(int offsetX, int offsetY, int shipNum);
void initializeGrid();
void warpGrid(Ship * ships, Planet * planets);
void closeGrid();
typedef struct{
	float x;  // point's original X position
	float y;  // point's original Y position
	float dx; // point's deviation from its original position in x and y
	float dy;
	float dv;
	int color;
} GridPoint;
