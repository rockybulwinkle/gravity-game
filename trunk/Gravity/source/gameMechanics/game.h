/*
 * game.h
 *
 *  Created on: Mar 5, 2011
 *      Author: hopwoocp
 */

#ifndef GAME_H_
#define GAME_H_


#endif /* GAME_H_ */

void dealDamage(Ship * ships, Planet * planets);
void bulletDamage(Ship * ship, Planet * planets);
void killShip(Ship * ship, Planet * planets);
void takeOverPlanets(Ship * ship, Planet * planet);
void doMechanics(Ship * ships, Planet * planets);
void planetDamage(Ship * ship, Planet * planets);
void killPlanet(Planet * planet);
void printPoints(Ship * ship);
float compareAngles(Vector one, Vector two);
void AI(Ship * ship, Planet * planet, int * turnLeft, int * turnRight, int * forward, int * backward, int * fire, int shipNum);
void shootEnemyShips(Ship * ships, Planet * planets);
void upgradeAvailable(Planet * planets);
void giveUpgrades(Ship * ships, Planet * planets);
void handleSpecialBullets(Ship * ships, Planet * planets);
