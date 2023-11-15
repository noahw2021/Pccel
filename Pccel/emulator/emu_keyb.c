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
	switch (EmuCtx->KeybData.Command) {
	case 0x00: // Set key down interrupt
		EmuCtx->KeybData.InterruptKeyDown = Command;
		break;
	case 0x01: // Set key up interrupt
		EmuCtx->KeybData.InterruptKeyUp = Command;
		break;
	}

	return;
}

DWORD64	EmudKeyboardGetData(DWORD Device) {
	return EmuCtx->KeybData.Outbuf;
}

void EmudKeyboardReset(DWORD Device) {
	return;
}

void EmudKeyboardOff(DWORD Device) {
	PPL2_DEVICE KeybDevice = &EmuCtx->DeviceList[1];
	KeybDevice->Flags.On = 0;
	return;
}

void EmudKeyboardOn(DWORD Device) {
	PPL2_DEVICE KeybDevice = &EmuCtx->DeviceList[1];
	KeybDevice->Flags.On = 1;
	return;
}