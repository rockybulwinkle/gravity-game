/*===========================================
 GRRLIB (GX Version)
 - Template Code -

 Minimum Code To Use GRRLIB
 ============================================*/

#include "definitions.h"
#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include <math.h>
#include "shipFuncs/shipFuncs.h"
#include "physicsFuncs/physics.h"
#include "render/render.h"
#include "../fonts/Calibri_18.h"
#include "FPS/fps.h"
#include "gameMechanics/game.h"
#include <stdio.h>
#include "staryBackground/stars.h"
#include "render/grid.h"
#include "menus/menu.h"

int numPlanets = 5;
int Max_Bullets = 500;
GRRLIB_texImg *tex_Calibri;
int main(int argc, char **argv) {
	// Initialise the Graphics & Video subsystem
	GRRLIB_Init();
	GRRLIB_Settings.antialias = true;

	tex_Calibri = GRRLIB_LoadTexture(Calibri_18);

	GRRLIB_InitTileSet(tex_Calibri, 27, 37, 32);

	// Initialise the Wiimotes
	WPAD_Init();

	mainMenu();

	Ship *ship;
	ship = (Ship*) malloc(NUM_SHIPS * sizeof(Ship));
	ship = initializeShips(NUM_SHIPS, ship);
	Planet* planet;
	planet = (Planet*) malloc(NUM_PLANETS * sizeof(Planet));

	planet[0].x = 200;
	planet[0].y = 200;
	planet[0].m = 300;
	planet[0].r = 40;
	planet[0].color = 0xFFFFFFFF;
	planet[0].owner = NO_OWNER;
	planet[0].health = PLANET_HEALTH;
	planet[1].x = 400;
	planet[1].y = 400;
	planet[1].m = 300;
	planet[1].r = 30;
	planet[1].color = 0x00FFFFFF;
	planet[1].owner = NO_OWNER;
	planet[1].health = PLANET_HEALTH;
	planet[2].x = 1000;
	planet[2].y = 400;
	planet[2].m = 1000;
	planet[2].r = 200;
	planet[2].color = 0xFF2200FF;
	planet[2].owner = NO_OWNER;
	planet[2].health = PLANET_HEALTH;
	planet[3].x = 1400;
	planet[3].y = 1200;
	planet[3].m = 700;
	planet[3].r = 51;
	planet[3].color = 0x29AF1BFF;
	planet[3].owner = NO_OWNER;
	planet[3].health = PLANET_HEALTH;
	planet[4].x = 1850;
	planet[4].y = 1850;
	planet[4].m = 100;
	planet[4].r = 20;
	planet[4].color = 0xFF7777FF;
	planet[4].owner = NO_OWNER;
	planet[4].health = PLANET_HEALTH;

	ship = initializeShips(NUM_SHIPS, ship);
	Max_Bullets = 1000;
	//saveLevel(planet, ship);
	u8 frame_rate;
	int i = 0;
	int profileUpdate[NUM_SHIPS];
	int profileRender;

	initializeTextures();
	int buttons;
	initializeGrid();
	while (1) {

		WPAD_ScanPads(); // Scan the Wiimotes
		buttons = WPAD_ButtonsDown(0);
		// If [HOME] was pressed on the first Wiimote, break out of the loop
		if (buttons & WPAD_BUTTON_HOME)
			break;
		if (buttons & WPAD_BUTTON_PLUS) {
			//saveLevel(planet, ship);
		}
		if (buttons & WPAD_BUTTON_MINUS) {
			//loadLevel(planet, ship);
		}
		GRRLIB_ClipReset();
		for (i = 0; i < NUM_SHIPS; i++) {
			profiler(1);
			update(&ship[i], planet, i);
			profileUpdate[i] = profiler(0);
			//GRRLIB_Printf(20, 20+i*20, tex_Calibri, 0xFFFFFFFF, 1, "isLanded: %d", ship[i].isLanded);
		}
		GRRLIB_ClipReset();
		doMechanics(ship, planet);
		//GRRLIB_Printf(20, 60, tex_Calibri, 0xFFFFFFFF, 1, "owner: %d", planet[0].owner);
		//GRRLIB_Printf(20, 80, tex_Calibri, 0xFFFFFFFF, 1, "owner: %d", planet[1].owner);

		profiler(1);

		render(ship, planet);

		profileRender = profiler(0);
		//renderPlanets(planet);
		frame_rate = calculateFPS();

		GRRLIB_Printf(20, 20, tex_Calibri, 0xFFFFFFFF, .5, "FPS: %d",
				frame_rate);
		for (i = 0; i < NUM_SHIPS; i++) {
			GRRLIB_Printf(20, 40 + i * 20, tex_Calibri, 0xFFFFFFFF
					* ENABLE_DEBUGGING, 1, "update: %d", profileUpdate[i]);
		}
		GRRLIB_Printf(20, 80, tex_Calibri, 0xFFFFFFFF * ENABLE_DEBUGGING, 1,
				"render: %d", profileRender);
		GRRLIB_Printf(20, 100, tex_Calibri, 0xFFFFFFFF * ENABLE_DEBUGGING, 1,
				"bullets: %d", ship[0].numBullets);
		GRRLIB_Printf(20, 120, tex_Calibri, 0xFFFFFFFF * ENABLE_DEBUGGING, 1,
				"bullets: %d", ship[1].numBullets);
		GRRLIB_Render();
	}

	GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
	free(ship);
	freeStarMemory();
	exit(0); // Use exit() to exit a program, do not use 'return' from main()
}

