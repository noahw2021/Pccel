/*
exp_init.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#include "../exports.h"
#include "../emulator/emulator.h"

void CcInit(void) {
	EmuInit();
}

void CcShutdown(void) {
	EmuShutdown();
}