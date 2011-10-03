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
#include "../staryBackground/stars.h"
#include <math.h>

extern GRRLIB_texImg * tex_Calibri;
int mainMenu() {
	GRRLIB_Rectangle(0, 0, SCREEN_X, SCREEN_Y, 0x000000FF, 1);
	GRRLIB_Printf(20, 20, tex_Calibri, 0xFFFFFFFF, 1, "press start");
	int buttons;
	int selection = 0;
	freeStarMemory();
	initializeTextures();
	double x = 0;
	double y = 0;
	while (selection == 0) {
		GRRLIB_3dMode(0.1, 1000,45, 0,1);
		//GRRLIB_Printf(20, 20, tex_Calibri, 0xFFFFFFFF, 1, "press start");
		WPAD_ScanPads();
		buttons = WPAD_ButtonsDown(0);
		if (buttons & WPAD_BUTTON_PLUS) {
			selection = 1;
		}
		GRRLIB_Camera3dSettings(0.0f,0.0f,320, 0,1,0, 0,0,0);
		GRRLIB_ObjectView(100.0f, 0.0f, 0, 0,0,0,1,1,1);
		GRRLIB_SetLightDiff(0, (guVector){0,0,0},20.0f, 1.0f, 0xFF0000FF);
		GRRLIB_SetLightAmbient(0x777777FF);
		GRRLIB_DrawSphere(30.0f, 20,20, 1, 0xFFFFFFFF);

		//displayMenuStars(x, y);
		GRRLIB_Render();
	}
	freeStarMemory();
	GRRLIB_2dMode();
	return selection;
}
void displayMiniMap(FILE*file) {
}
