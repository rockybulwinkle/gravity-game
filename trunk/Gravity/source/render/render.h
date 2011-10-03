/*
 * render.h
 *
 *  Created on: Mar 2, 2011
 *      Author: hopwoocp
 */

#ifndef RENDER_H_
#define RENDER_H_

#endif /* RENDER_H_ */

void renderShip(Ship * ship, int offsetX, int offsetY, int shipNum);
void render(Ship * ship, Planet *planet);
void renderPlanets(Planet *planet, int offsetX, int offsetY, float shipX, float shipY, Ship * ship);
void colorBullets(struct Bullet * bullet, Ship * ship);
void renderHealth(Ship * ship, int offsetX, int shipNum);
void drawPlanetBullets(Planet* planets, Ship * ships, int offsetX, int offsetY, int shipNum);
