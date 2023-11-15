/*
emu_fdisk.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/
#include "emulator.h"

DWORD64 EmudFdiskStatusQuery(DWORD Device) {
	return DEVSTATUS_GOOD;
}

void EmudFdiskSendCommand(DWORD Device, DWORD64 Command) {
	EmuCtx->FdiskData.Command = Command;

	switch (Command) {
	case 0x00: // set active drive
		break;
	case 0x01: // get total number of drives
		EmuCtx->FdiskData.Outbuf = 1;
		break;
	case 0x02: // puts a drive to sleep
 		break; 
	case 0x03: // wakes a drive up
		break;
	case 0x04: // get drive size
		fseek(EmuCtx->FdiskData.PhysicalFile, 0, SEEK_END);
		EmuCtx->FdiskData.Outbuf = ftell(EmuCtx->FdiskData.PhysicalFile);
		break;
	case 0x06: // drive read
		fseek(EmuCtx->FdiskData.PhysicalFile, EmuCtx->FdiskData.FilePointer, SEEK_SET);
		fread(&EmuCtx->FdiskData.Outbuf, 8, 1, EmuCtx->FdiskData.PhysicalFile);
		if (!EmuCtx->FdiskData.DisableIncrement) {
			EmuCtx->FdiskData.FilePointer += 8;
		} else if (EmuCtx->FdiskData.SkipIncrement) {
			EmuCtx->FdiskData.DisableIncrement = 0;
		}
		break;
	case 0x07: // drive write
		break; // needs data
	case 0x08: // getdriveserial
		EmuCtx->FdiskData.Outbuf = 4280;
		break;
	case 0x09: // getdrivevendor
		EmuCtx->FdiskData.Outbuf = NULL;
		break;
	case 0x0A: // getdrivemodel
		EmuCtx->FdiskData.Outbuf = 2;
		break;
	case 0x0B: // getdrivevendorid
		EmuCtx->FdiskData.Outbuf = 1;
		break;
	case 0x0C: // farseek 
		break; // do nothing
	case 0x0D: // driveskipfpincrement
		EmuCtx->FdiskData.DisableIncrement = 1;
		EmuCtx->FdiskData.SkipIncrement = 0;
		break;
	case 0x0E: // drivedisablefpincrement
		EmuCtx->FdiskData.DisableIncrement = 1;
		break;
	case 0x0F: // driveenablefpincrement
		EmuCtx->FdiskData.DisableIncrement = 0;
		break;
	case 0x10: // driveseek
		break; // requires data
	case 0x11: // drivereadstack
		fseek(EmuCtx->FdiskData.PhysicalFile, EmuPopStack(), SEEK_SET);
		fread(&EmuCtx->FdiskData.Outbuf, 8, 1, EmuCtx->FdiskData.PhysicalFile);
		break; 
	case 0x12: // drivewritestack
		break; // requires data
	}

	return;
}

void EmudFdiskSendData(DWORD Device, DWORD64 Command) {
	switch (EmuCtx->FdiskData.Command) {
	case 0x07: // write
		fseek(EmuCtx->FdiskData.PhysicalFile, EmuCtx->FdiskData.FilePointer, SEEK_SET);
		fwrite(EmuCtx->PhysicalMemory + EmuVirtualTranslate(Command), 8, 1,
			EmuCtx->FdiskData.PhysicalFile);
		break;
	case 0x10: // seek
		EmuCtx->FdiskData.FilePointer = Command;
		break;
	case 0x12: // write stack
		fseek(EmuCtx->FdiskData.PhysicalFile, EmuPopStack(), SEEK_SET);
		fwrite(EmuCtx->PhysicalMemory + EmuVirtualTranslate(Command), 8, 1,
			EmuCtx->FdiskData.PhysicalFile);
		break;
	}

	return;
}

DWORD64	EmudFdiskGetData(DWORD Device) {
	return EmuCtx->FdiskData.Outbuf;
}

void EmudFdiskReset(DWORD Device) {
	return;
}

void EmudFdiskOff(DWORD Device) {
	PPL2_DEVICE	FdiskDevice = &EmuCtx->DeviceList[2];
	FdiskDevice->Flags.On = 0;
	return;
}

void EmudFdiskOn(DWORD Device) {
	PPL2_DEVICE	FdiskDevice = &EmuCtx->DeviceList[2];
	FdiskDevice->Flags.On = 1;
	return;
}