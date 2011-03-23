/*
 * stars.h
 *
 *  Created on: Mar 5, 2011
 *      Author: hopwoocp
 */

#ifndef STARS_H_
#define STARS_H_


#endif /* STARS_H_ */

typedef struct{
	float x;
	float y;
	float z;
	int color;
} Star;

void initializeTextures();
void displayStars(int offsetX, int offsetY, Ship * ship);
void displayMenuStars(float x, float y);
void freeStarMemory();
