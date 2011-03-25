/*
 * definitions.h
 *
 *  Created on: Mar 1, 2011
 *      Author: hopwoocp
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_
#define SHIPFUNC_H_

/*
 * Width and height of the screen
 * Will be changed to GRRLIB's video mode later.
 */
#define SCREEN_X 640
#define SCREEN_Y 480
#define ORIGIN_X 0
#define ORIGIN_Y 0
#define BORDER_COLOR 0xFFFFFFFF
/*
 * Properties of ships and planets
 */
#define HEALTH_BAR_SCALE 1
#define DAMPING_CONSTANT .5 //used for bouncing off planets.  Could be changed to be a property of different planets.
#define NUM_BULLETS Max_Bullets    // Max bullets per player.  May be changed later to allow for 'upgrades'
#define MAX_SHIP_SPEED 20    // Max ship speed
#define MAX_BULLET_SPEED 5  // Max bullet speed
#define BULLET_SPEED 2      // Player's bullet speed.  may be changed to allow for 'upgrades'
#define RED_SHIFT_FACTOR    30// factor that determines how red or blue bullets become with relative speed
#define BLUE_SHIFT_FACTOR   30
#define GREEN_SHIFT_FACTOR  30
#define BURN_FACTOR         // factor that determines how red a bullet becomes with absolute speed
#define BULLET_LIFE   3000  // number of milli that the bullet will stay alive, also related to shift in alpha of bullets
#define BULLET_LENGTH 0     // Length of certain type of bullets. Deprecated for now.
#define SHIP_HEALTH 150
#define PLANET_HEALTH 100
#define SHIP_LAND_SPEED 2.3
#define SHIP_ANG_ACC  .11
#define SHIP_LIN_ACC  .15
#define TAKEN_OVER 100
#define NUM_PLANETS numPlanets      // Number of planets
#define NUM_SHIPS 2         // Number of Human ships
#define NUM_AI    0         // Number of AI players
#define NO_OWNER 1000       // value for planet owner member when it does not belong to any ship
#define LEVEL_WIDTH 2000    // Width of level
#define LEVEL_HEIGHT 2000   // Height of level
#define LEVEL_ORIGIN_X 0
#define LEVEL_ORIGIN_Y 0
#define MAP_SCALE 15
#define MAP_OFFSET_X 253
#define MAP_OFFSET_Y 347
#define NUM_STARS 2000
#define ENABLE_DEBUGGING 0
#define CRASH_DAMAGE_MULTIPLIER 15
#define PI 3.1415936535897  //Pi constant.
/*
 * Structure used for bullets
 */
typedef struct {
	float x; // X position
	float y; // Y position
	float vx; // X velocity
	float vy; // Y velocity
	int drawn; //indicate if the bullet is active or not
	unsigned long milli; // time in milliseconds that the bullet entered play
	long int baseColor; // base color of the bullet
	long int shiftedColor; // color of the bullet after shifting

} Bullet;

typedef struct {
	float x; // X position of ship
	float y; // Y position of ship
	float scrollX; // scrolling in X
	float scrollY; // scrolling in Y
	float vx; // X velocity of ship
	float vy; // Y velocity of ship
	int r; // radius of ship
	double ang; // rotation of ship in radians
	int color; // color of ship
	float acc; // linear acceleration of ship
	float angAcc; // angular acceleration of ship
	Bullet * bullets; // pointer to ship's bullet array.  Made as a pointer so that 'upgrades' can be made to ship.
	int numBullets; // determines which slot in the bullet array to fill in with a new bullet so that older bullets are replaced first.
	int isLanded; // true when landed on something, false otherwise.
	int isShooting; // used to stager bullet rapid fire
	int health;
	int userOveride;
	int landedPlanet;
	int shipNum;
	int points;
	int diedFromPlanet;
	int isAI;
} Ship;

typedef struct {
	float x; // X position of planet
	float y; // Y position of planet
	int m; // Mass of planet
	int r; // Radius of planet
	double color; // Color of planet
	int owner; // ship # that this planet belongs to.  equal to NO_OWNER (defined above) if it has no owner
	int health;
} Planet;

typedef struct {
	float x;
	float y;
} Vector;
extern int numPlanets;
extern int Max_Bullets;
#endif /* DEFINITIONS_H_ */


