/*
 * stars.c
 *
 *  Created on: Mar 5, 2011
 *      Author: hopwoocp
 */
#include "../definitions.h"
#include "stars.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <grrlib.h>

Star * fieldLayerOne = NULL;
//static Star fieldLayerTwo[NUM_STARS];

void initializeTextures() {
	srand(time(NULL));

	int i;
	fieldLayerOne = (Star*) malloc(sizeof(Star) * NUM_STARS);
	if (fieldLayerOne == NULL)
		exit(0);
	for (i = 0; i < NUM_STARS; i++) {
		fieldLayerOne[i].x = rand();
		fieldLayerOne[i].x *= LEVEL_WIDTH;
		fieldLayerOne[i].x /= RAND_MAX;
		fieldLayerOne[i].y = rand();
		fieldLayerOne[i].y *= LEVEL_HEIGHT;
		fieldLayerOne[i].y /= RAND_MAX;
		fieldLayerOne[i].z = rand();
		fieldLayerOne[i].z = fieldLayerOne[i].z / RAND_MAX;
		fieldLayerOne[i].color
				= RGBA(0xFF, 0xFF, 0xFF, fieldLayerOne[i].z*0xFF);
	}

}

void displayStars(int offsetX, int offsetY, Ship * ship) {
	int i;
	float x2;
	float y2;
	int jj, kk;
	float x = ship->x;
	float y = ship->y;
	/*for (i = 0; i < NUM_STARS; i++) {
	 GRRLIB_Rectangle((fieldLayerOne[i].x + offsetX) * fieldLayerOne[i].z,
	 (fieldLayerOne[i].y + offsetY) * fieldLayerOne[i].z,
	 fieldLayerOne[i].z * 2, fieldLayerOne[i].z * 2,
	 fieldLayerOne[i].color, 1);
	 }*/
	for (i = 0; i < NUM_STARS; i++) {
		for (jj = -1; jj <= 1; jj++) {
			x2 = (fieldLayerOne[i].x + offsetX + LEVEL_WIDTH * jj)
					* fieldLayerOne[i].z;
			for (kk = -1; kk <= 1; kk++) {
				y2 = (fieldLayerOne[i].y + offsetY + LEVEL_HEIGHT * kk)
						* fieldLayerOne[i].z;
				if (GRRLIB_PtInRect(x - SCREEN_X / 4, y - SCREEN_Y / 2, SCREEN_X,SCREEN_Y, x2-offsetX, y2-offsetY)) {
					//GRRLIB_Rectangle(x3 - x + offsetX, y3 - y + offsetY, 1, 1,
					//bullet->shiftedColor, 1);
					GRRLIB_Rectangle(x2, y2, fieldLayerOne[i].z * 2,
							fieldLayerOne[i].z * 2, fieldLayerOne[i].color, 1);
					//count++;
					jj = 2;
					kk = 2;
				}

			}
		}
	}

}


void displayMenuStars(float x, float y) {
	int i;
	float offsetX = 0;
	float offsetY = 0;
	float x2;
	float y2;
	int jj, kk;
	/*for (i = 0; i < NUM_STARS; i++) {
	 GRRLIB_Rectangle((fieldLayerOne[i].x + offsetX) * fieldLayerOne[i].z,
	 (fieldLayerOne[i].y + offsetY) * fieldLayerOne[i].z,
	 fieldLayerOne[i].z * 2, fieldLayerOne[i].z * 2,
	 fieldLayerOne[i].color, 1);
	 }*/
	for (i = 0; i < NUM_STARS; i++) {
		for (jj = -1; jj <= 1; jj++) {
			x2 = (fieldLayerOne[i].x + offsetX + LEVEL_WIDTH * jj)
					* fieldLayerOne[i].z;
			for (kk = -1; kk <= 1; kk++) {
				y2 = (fieldLayerOne[i].y + offsetY + LEVEL_HEIGHT * kk)
						* fieldLayerOne[i].z;
				if (GRRLIB_PtInRect(0,0,SCREEN_X, SCREEN_Y, x2,y2)) {
					GRRLIB_Rectangle(x2, y2, fieldLayerOne[i].z * 2,
							fieldLayerOne[i].z * 2, fieldLayerOne[i].color, 1);
					//count++;
					jj = 2;
					kk = 2;
				}

			}
		}
	}

}


void freeStarMemory(){
	free(fieldLayerOne);
}
