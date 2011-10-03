#include "definitions.h"
#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include <math.h>
#include <ogc/lwp_watchdog.h>
#include "physicsFuncs/physics.h"
#include "render/render.h"
#include "../fonts/Calibri_18.h"
#include "FPS/fps.h"
#include "gameMechanics/game.h"
#include <stdio.h>
#include "staryBackground/stars.h"
#include "render/grid.h"
#include "menus/menu.h"
#include "shipFuncs/shipFuncs.h"
#include "sdcardMount.h"
#include <time.h>
#include <stdlib.h>

int numPlanets = 5;
int Max_Bullets = 50;
long frameCount = 0;
GRRLIB_texImg *tex_Calibri;

int main(int argc, char **argv) {
	InitSD();
	FILE * file = fopen("sd:/hello.txt", "w");
	if (file == NULL)
		exit(0);
	fprintf(file, "hello, there!\n");
	fclose(file);

	// initialize the sound system.
	//ASND_Init();

	// Initialise the Graphics & Video subsystem
	GRRLIB_Init();
	GRRLIB_Settings.antialias = true;

	tex_Calibri = GRRLIB_LoadTexture(Calibri_18);

	GRRLIB_InitTileSet(tex_Calibri, 27, 37, 32);

	// Initialize the Wiimotes
	WPAD_Init();

	mainMenu();

	Ship *ship;
	ship = (Ship*) malloc(NUM_SHIPS * sizeof(Ship));
	ship = initializeShips(NUM_SHIPS, ship);
	Planet* planet;
	planet = (Planet*) malloc(NUM_PLANETS * sizeof(Planet));

	planet[0].x = 200;
	planet[0].y = 200;
	planet[0].m = 700;
	planet[0].r = 60;
	planet[0].color = 0xFFFFFFFF;
	planet[0].owner = NO_OWNER;
	planet[0].health = PLANET_HEALTH;
	planet[0].currentUpgrade = 0;
	planet[1].x = 400;
	planet[1].y = 400;
	planet[1].m = 300;
	planet[1].r = 30;
	planet[1].color = 0x00FFFFFF;
	planet[1].owner = NO_OWNER;
	planet[1].health = PLANET_HEALTH;
	planet[1].currentUpgrade = 0;
	planet[2].x = 1000;
	planet[2].y = 400;
	planet[2].m = 1000;
	planet[2].r = 200;
	planet[2].color = 0xFF2200FF;
	planet[2].owner = NO_OWNER;
	planet[2].health = PLANET_HEALTH;
	planet[2].currentUpgrade = 0;
	planet[3].x = 1400;
	planet[3].y = 1200;
	planet[3].m = 700;
	planet[3].r = 51;
	planet[3].color = 0x29AF1BFF;
	planet[3].owner = NO_OWNER;
	planet[3].health = PLANET_HEALTH;
	planet[3].currentUpgrade = 0;
	planet[4].x = 1850;
	planet[4].y = 1850;
	planet[4].m = 100;
	planet[4].r = 20;
	planet[4].color = 0xFF7777FF;
	planet[4].owner = NO_OWNER;
	planet[4].health = PLANET_HEALTH;
	planet[4].currentUpgrade = 0;
	// initialize planet's bullet arrays
	int i = 0;
	for (i = 0; i < NUM_PLANETS; i++) {
		planet[i].bullets = (struct Bullet *) malloc(sizeof(Bullet)
				* PLANET_BULLET_NUM);
		if (planet[i].bullets == NULL) {
			exit(0);
		}
		planet[i].numBullets = 0;
	}

	ship = initializeShips(NUM_SHIPS, ship);

	u8 frame_rate;

	initializeTextures();
	int buttons;
	initializeGrid();

	//ASND_Init();
	//ASND_SetVoice(5,VOICE_STEREO_16BIT,8000, 0, &betamaster_raw, betamaster_raw_size, 255, 255, NULL);
	/*
	 MODPlay_Init(&play);
	 MODPlay_SetMOD(&play, paradox_mod);
	 MODPlay_SetVolume(&play, 63, 63);
	 MODPlay_Start(&play);
	 */
	while (1) {

		profiler(1);
		WPAD_ScanPads(); // Scan the Wiimotes
		buttons = WPAD_ButtonsDown(0);
		// If [HOME] was pressed on the first Wiimote, break out of the loop
		if (buttons & WPAD_BUTTON_HOME)
			break;

		update(ship, planet);
		doMechanics(ship, planet);
		render(ship, planet);

		frame_rate = calculateFPS();

		GRRLIB_Printf(20, 20, tex_Calibri, 0xFFFFFFFF, .5, "FPS: %d",
				frame_rate);
		GRRLIB_Printf(20, 40, tex_Calibri, 0xFFFFFFFF, .5, "%d",
		                                ship[0].bullets[0].exploded);
		//int endProfile = profiler(0);
		GRRLIB_Render();
		frameCount++;

	}

	GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
	for (i = 0; i < NUM_SHIPS; i++) {
		free(ship[i].bullets);
	}
	free(ship);
	GRRLIB_FreeTexture(tex_Calibri);
	freeStarMemory();
	exit(0); // Use exit() to exit a program, do not use 'return' from main()
}

