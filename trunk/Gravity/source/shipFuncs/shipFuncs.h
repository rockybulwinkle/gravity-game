/*
 * shipFunc.h
 *
 *  Created on: Feb 22, 2011
 *      Author: hopwoocp
 */
//#include "../definitions.h"
#ifndef SHIPFUNC_H_

#endif /* SHIPFUNC_H_ */

void checkControllers(int chan, Ship * ship, Planet * planet);
void update(Ship *ship, Planet *planet);
Ship* initializeShips(int numShips, Ship * ship);
void updateShipSpeeds(Ship * ship);
//Planet* initializePlanets();
void updatePlanetBullets(Planet * planets);
