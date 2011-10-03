/*
 * fps.c
 *
 *  Created on: Mar 3, 2011
 *      Author: hopwoocp
 */
#include <ogc/lwp_watchdog.h>
#include <time.h>
u8 calculateFPS() {
	static u8 frameCount = 0;
	static int lastTime = 0;
	static u8 FPS = 0;
	int currentTime = time(NULL);

	frameCount++;
	if (currentTime != lastTime) {
		lastTime = currentTime;
		FPS = frameCount;
		frameCount = 0;
	}
	return FPS;
}

u64 profiler(int start) {
	static u64 ticks = 0; // initialize to zero at compile time
	int deltaTicks = 0;
	if (start == 1) {
		ticks = gettime();
	} else {
		deltaTicks = gettime() - ticks;
	}
	return ticks_to_cycles(deltaTicks);
}
