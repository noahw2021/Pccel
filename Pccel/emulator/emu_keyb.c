/*
emu_keyb.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#include "emulator.h"

DWORD64 EmudKeyboardStatusQuery(DWORD Device) {
	return DEVSTATUS_GOOD;
}

void EmudKeyboardSendCommand(DWORD Device, DWORD64 Command) {
	EmuCtx->KeybData.Command = Command;

	switch (Command) {
	case 0x02:
		EmuCtx->KeybData.Outbuf = EmuCtx->EmuKeyState - EmuCtx->PhysicalMemory;
		break;
	}

	return;
}

void EmudKeyboardSendData(DWORD Device, DWORD64 Command) {

}

DWORD64	EmudKeyboardGetData(DWORD Device) {

}

void EmudKeyboardReset(DWORD Device) {

}

void EmudKeyboardOff(DWORD Device) {

}

void EmudKeyboardOn(DWORD Device) {

}