/*
 * sdcardMount.c
 *
 *  Created on: Apr 1, 2011
 *      Author: Arikado
 */
#include <sdcard/wiisd_io.h>
#include <fat.h>
#include "sdcardMount.h"

void InitSD() {

	fatUnmount("sd:/");

	__io_wiisd.shutdown();

	fatMountSimple("sd", &__io_wiisd);

}

void DeInitSD() {

	fatUnmount("sd:/");
	__io_wiisd.shutdown();

}
