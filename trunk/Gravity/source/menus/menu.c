/*
 * menu.c
 *
 *  Created on: Mar 7, 2011
 *      Author: hopwoocp
 */

#include <grrlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include "../definitions.h"
//#include <fat.h>
#include "../staryBackground/stars.h"
#include <math.h>

extern GRRLIB_texImg * tex_Calibri;
int mainMenu(){
	GRRLIB_Rectangle(0,0,SCREEN_X, SCREEN_Y, 0x000000FF, 1);
	GRRLIB_Printf(20,20,tex_Calibri, 0xFFFFFFFF, 1, "press start");
	int buttons;
	int selection = 0;
	freeStarMemory();
	initializeTextures();
	double x=0;
	double y=0;
	while(selection == 0){
		GRRLIB_Printf(20,20,tex_Calibri, 0xFFFFFFFF, 1, "press start");
		WPAD_ScanPads();
		buttons = WPAD_ButtonsDown(0);
		if (buttons & WPAD_BUTTON_PLUS){
			selection = 1;
		}
		displayMenuStars(x, y);

		GRRLIB_Render();
	}

	return selection;
}
void displayMiniMap(FILE*file){
}
