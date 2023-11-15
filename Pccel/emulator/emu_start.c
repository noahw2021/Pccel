/*
emu_start.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#include "emulator.h"
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

PPL2_CONTEXT Ctx;
PCCEL_EMUCTX EmuCtx;

void EmuInit(void) {
	Ctx = malloc(sizeof(PL2_CONTEXT));
	memset(Ctx, 0, sizeof(PL2_CONTEXT));

	EmuCtx = malloc(sizeof(CCEL_EMUCTX));
	memset(EmuCtx, 0, sizeof(CCEL_EMUCTX));

	EmuCtx->PhysicalMemory = malloc(EMUARG_RAMSIZE);
	memset(EmuCtx->PhysicalMemory, 0, EMUARG_RAMSIZE);

	EmuCtx->DeviceList = EmuCtx->PhysicalMemory + 0x00;

	// Create Video Device
	PPL2_DEVICE VideoDevice = &EmuCtx->DeviceList[0];
	VideoDevice->DeviceType = DEVTYPE_VIDEO;
	VideoDevice->DeviceModel = 1;
	strcpy(VideoDevice->DeviceName, "PCCEL Emulated Video Driver");
	strcpy(VideoDevice->DeviceVendor, "noahw2021/PCCEL");
	VideoDevice->DeviceSerial = 4270;
	VideoDevice->VendorId = 1;
	VideoDevice->Flags.Active = 1;
	VideoDevice->Flags.On = 1;
	VideoDevice->Callbacks[0] = EmudVideoStatusQuery;
	VideoDevice->Callbacks[1] = EmudVideoSendCommand;
	VideoDevice->Callbacks[2] = EmudVideoSendData;
	VideoDevice->Callbacks[3] = EmudVideoGetData;
	VideoDevice->Callbacks[4] = EmudVideoReset;
	VideoDevice->Callbacks[5] = EmudVideoOff;
	VideoDevice->Callbacks[6] = EmudVideoOn;

	// Create Keyboard Device
	PPL2_DEVICE KbDevice = &EmuCtx->DeviceList[1];
	KbDevice->DeviceType = DEVTYPE_KB;
	KbDevice->DeviceModel = 1;
	strcpy(KbDevice->DeviceName, "PCCEL Emulated I/O Keyboard");
	strcpy(KbDevice->DeviceVendor, "noahw2021/PCCEL");
	KbDevice->DeviceSerial = 4271;
	KbDevice->VendorId = 1;
	KbDevice->Flags.Active = 1;
	KbDevice->Flags.On = 1;
	KbDevice->Callbacks[0] = EmudKeyboardStatusQuery;
	KbDevice->Callbacks[1] = EmudKeyboardSendCommand;
	KbDevice->Callbacks[2] = EmudKeyboardSendData;
	KbDevice->Callbacks[3] = EmudKeyboardGetData;
	KbDevice->Callbacks[4] = EmudKeyboardReset;
	KbDevice->Callbacks[5] = EmudKeyboardOff;
	KbDevice->Callbacks[6] = EmudKeyboardOn;
	EmuCtx->EmuKeyState = EmuCtx->PhysicalMemory + 0x23F0;

	// Create Fixed Disk Controller Device
	PPL2_DEVICE FdiskDevice = &EmuCtx->DeviceList[2];
	FdiskDevice->DeviceType = DEVTYPE_KB;
	FdiskDevice->DeviceModel = 1;
	strcpy(FdiskDevice->DeviceName, "PCCEL Fixed Disk Controller");
	strcpy(FdiskDevice->DeviceVendor, "noahw2021/PCCEL");
	FdiskDevice->DeviceSerial = 4272;
	FdiskDevice->VendorId = 1;
	FdiskDevice->Flags.Active = 1;
	FdiskDevice->Flags.On = 1;
	FdiskDevice->Callbacks[0] = EmudFdiskStatusQuery;
	FdiskDevice->Callbacks[1] = EmudFdiskSendCommand;
	FdiskDevice->Callbacks[2] = EmudFdiskSendData;
	FdiskDevice->Callbacks[3] = EmudFdiskGetData;
	FdiskDevice->Callbacks[4] = EmudFdiskReset;
	FdiskDevice->Callbacks[5] = EmudFdiskOff;
	FdiskDevice->Callbacks[6] = EmudFdiskOn;

	EmuCtx->DeviceCount = 3;
	EmuCtx->ScreenSizeX = 640;
	EmuCtx->ScreenSizeY = 480;

	EmuCtx->EmuWindow = SDL_CreateWindow("PCCEL Emulator Output", 20, 20, EmuCtx->ScreenSizeX,
		EmuCtx->ScreenSizeY, SDL_WINDOW_SHOWN);
	EmuCtx->EmuRenderer = SDL_CreateRenderer(EmuCtx->EmuWindow, -1, SDL_RENDERER_ACCELERATED);
	EmuCtx->EmuThread = SDL_CreateThread(EmuVideoThread, "PCCELVideoThread", NULL);

	LARGE_INTEGER Time;
	QueryPerformanceCounter(&Time);
	EmuCtx->CpuCurrentNs = Time.QuadPart;
	EmuCtx->EmuInterruptThread = SDL_CreateThread(EmuInterruptThread, "PCCELInterruptThread", NULL);
	EmuCtx->CpuPhysicalThread = GetCurrentThread();

	EmuCtx->CpuInterruptCountMax = 256;
	EmuCtx->CpuInterrupts = malloc(sizeof(PL2_INTERRUPT) * EmuCtx->CpuInterruptCountMax);

	EmuCtx->FdiskData.PhysicalFile = fopen(EMUARG_FDISKNM, "rb+");
	DWORD Attempt = 0;
	while (!EmuCtx->FdiskData.PhysicalFile) { // pretty good
		Attempt++;
		char AttemptStr[6] = "00000";
		char FullName[20] = EMUARG_FDISKNM;

		int Attempt5 = (Attempt / 10000) % 10;
		int Attempt4 = (Attempt / 1000) % 10;
		int Attempt3 = (Attempt / 100) % 10;
		int Attempt2 = (Attempt / 10) % 10;
		int Attempt1 = (Attempt / 1) % 10;
		AttemptStr[0] += Attempt5;
		AttemptStr[1] += Attempt4;
		AttemptStr[2] += Attempt3;
		AttemptStr[3] += Attempt2;
		AttemptStr[4] += Attempt1;
		memcpy(FullName + 11, AttemptStr, 5);
		EmuCtx->FdiskData.PhysicalFile = fopen(FullName, "rb+");
	}

	return;
}

void EmuShutdown(void) {
	if (Ctx)
		free(Ctx);

	if (EmuCtx) {
		if (EmuCtx->PhysicalMemory)
			free(EmuCtx->PhysicalMemory);

		SDL_DestroyRenderer(EmuCtx->EmuRenderer);
		SDL_DestroyWindow(EmuCtx->EmuWindow);
		SDL_DetachThread(EmuCtx->EmuThread);
		SDL_DetachThread(EmuCtx->EmuInterruptThread);
		fclose(EmuCtx->FdiskData.PhysicalFile);

		free(EmuCtx);
	}

	return;
}

void EmuVideoThread(void) {
	SDL_Event Event;
	BOOLEAN Quit = 0;
	while (!Quit) {
		while (SDL_PollEvent(&Event)) {
			if (EmuCtx->EmuPauseDrawing)
				break;

			if (Event.type == SDL_QUIT) {
				Quit = 1;
				break;
			}

			if (Event.type == SDL_KEYDOWN) {
				EmuCtx->EmuKeyState[Event.key.keysym.scancode] = 1;
				EmuCtx->EmuKeyStateChange[Event.key.keysym.scancode] = 1;
			}

			if (Event.type == SDL_KEYUP) {
				EmuCtx->EmuKeyState[Event.key.keysym.scancode] = 0;
				EmuCtx->EmuKeyStateChange[Event.key.keysym.scancode] = 1;
			}
		}

		if (!EmuCtx->EmuPauseDrawing)
			SDL_RenderPresent(EmuCtx->EmuRenderer);
		
		Sleep(10);
	}
}

void EmuInterruptThread(void) {
	while (!Ctx->System.PccelFlags.InTable);
	SuspendThread(EmuCtx->CpuPhysicalThread);
	Ctx->System.PccelFlags.InTable = 0;
	GetThreadContext(EmuCtx->CpuPhysicalThread, &EmuCtx->CpuIntContext);

	while (1) {
		if (Ctx->System.PccelFlags.InInterrupt) {
			if (Ctx->System.PccelFlags.InterruptReturn) {
				SuspendThread(EmuCtx->CpuPhysicalThread);
				SetThreadContext(EmuCtx->CpuPhysicalThread, &EmuCtx->CpuPreIntContext);
				Ctx->System.PccelFlags.InInterrupt = 0;
				Ctx->System.PccelFlags.InterruptReturn = 0;
			}
			continue;
		}

		if (EmuCtx->CpuInterruptCount) {
			for (int i = EmuCtx->CpuInterruptCount - 1; i >= 0; i--) {
				PPL2_INTERRUPT ThisInt = &EmuCtx->CpuInterrupts[i];

				SuspendThread(EmuCtx->CpuPhysicalThread);
				GetThreadContext(EmuCtx->CpuPhysicalThread, &EmuCtx->CpuPreIntContext);
#ifdef _WIN64
				EmuCtx->CpuIntContext.Rcx = ThisInt->Interrupt;
#else
				EmuCtx->CpuIntContext.Ecx = ThisInt->Interrupt;
#endif
				EmuPushStack(ThisInt->Argument);
				SetThreadContext(EmuCtx->CpuPhysicalThread, &EmuCtx->CpuIntContext);
				Ctx->System.PccelFlags.InInterrupt = 1;
			}
		}

		LARGE_INTEGER CurrentTime;
		QueryPerformanceCounter(&CurrentTime);

		if (CurrentTime.QuadPart < EmuCtx->CpuCurrentNs)
			continue;

		EmuCtx->CpuCurrentNs = CurrentTime.QuadPart + EMUARG_INTTMNS;

		SuspendThread(EmuCtx->CpuPhysicalThread);
		GetThreadContext(EmuCtx->CpuPhysicalThread, &EmuCtx->CpuPreIntContext);
		// Interrupt should be stored in ECX/RCX, and the handler should be __fastcall on x86
#ifdef _WIN64
		EmuCtx->CpuIntContext.Rcx = EMUARG_CLCKINT;
#else
		EmuCtx->CpuIntContext.Ecx = EMUARG_CLCKINT;
#endif
		SetThreadContext(EmuCtx->CpuPhysicalThread, &EmuCtx->CpuIntContext);
		Ctx->System.PccelFlags.InInterrupt = 1;
	}

	return;
}