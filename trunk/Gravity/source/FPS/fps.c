/*
 * fps.c
 *
 *  Created on: Mar 3, 2011
 *      Author: hopwoocp
 */
#include <ogc/lwp_watchdog.h>

u8 calculateFPS() {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    u32 currentTime = ticks_to_millisecs(gettime());

    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
}

u64 profiler(int start) {
	static u64 ticks = 0; // initialize to zero at compile time
	int deltaTicks = 0;
	if(start == 1){
		ticks = gettime();
	}
	else{
		deltaTicks = gettime()-ticks;
	}
	return ticks_to_cycles(deltaTicks);
}
