#pragma once
/*
emulator.h
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#include "../exports.h"
#include <SDL.h>
#include <Windows.h>

#define EMUARG_RAMSIZE 1048576
#define EMUARG_INTTMNS 10000
#define EMUARG_CLCKINT 0x00

#define DEVTYPE_KB			0x10000001
#define DEVTYPE_MOUSE		0x10000002
#define DEVTYPE_CPU			0x10000003
#define DEVTYPE_TERMINAL	0x10000004
#define DEVTYPE_VIDEO       0x10000005
#define DEVTYPE_FDISKC      0x10000006
#define DEVTYPE_RDISKC      0x10000007
#define DEVTYPE_NETWORK     0x10000008

#define DEVSTATUS_GOOD       0x1100100A
#define DEVSTATUS_FAIL       0x1100100B
#define DEVSTATUS_WARN       0x1100100C
#define DEVSTAUTS_INVL       0x11002000

#define DEVSTATUS_RSN_FIRST  0x00000000
#define DEVSTATUS_RSN_INVCMD 0x00010000
#define DEVSTATUS_RSN_CLEAR  0x00FF0000

typedef struct _PL2_PAGE {
	DWORD64 Physical;
	DWORD64 Virtual;
	DWORD64 Size;

	union {
		BYTE Permissions;
		struct {
			BYTE Execute : 1;
			BYTE Read : 1;
			BYTE Write : 1;
			BYTE Active : 1;
		};
	};
}PL2_PAGE, *PPL2_PAGE;

typedef struct _PL2_DEVICE {
	int DeviceType;
	char DeviceName[32];
	char DeviceVendor[32];
	DWORD64 VendorId;
	DWORD64 DeviceModel;
	DWORD64 DeviceSerial;
	DWORD64(*Callbacks[7])(DWORD DeviceId, DWORD64 Argument);
	union {
		DWORD FlagsRaw;
		struct {
			DWORD Active : 1;
			DWORD On : 1;
			DWORD Sleep : 1;
			DWORD Hotswappable : 1;
		};
	}Flags;
}PL2_DEVICE, *PPL2_DEVICE;

typedef struct _PL2_INTERRUPT {
	BYTE Interrupt;
	DWORD64 Argument;
	CONTEXT OldThreadCtx;
}PL2_INTERRUPT, *PPL2_INTERRUPT;

typedef struct _CCEL_EMUCTX {
	SDL_Window* EmuWindow;
	SDL_Renderer* EmuRenderer;
	SDL_Thread* EmuThread;
	SDL_Thread* EmuInterruptThread;
	BOOLEAN EmuPauseDrawing;
	PBOOLEAN EmuKeyState;
	BOOLEAN EmuKeyStateChange[256];
	DWORD ScreenSizeX, ScreenSizeY;

	DWORD64 CpuCurrentNs;
	HANDLE CpuPhysicalThread;
	CONTEXT CpuPreIntContext;
	CONTEXT CpuIntContext;
	DWORD32 CpuInterruptCount;
	DWORD32	CpuInterruptCountMax;
	PPL2_INTERRUPT CpuInterrupts;

	PBYTE PhysicalMemory;
	PPL2_PAGE ActivePageTable;
	DWORD ActivePTEntryCount;
	PDWORD64 Stack;

	DWORD DeviceCount;
	PPL2_DEVICE DeviceList;

	struct {
		DWORD64 Command;
		DWORD64 Data;
		DWORD64 Outbuf;
	}VideoData;

	struct {
		DWORD64 Command;
		DWORD64 Data;
		DWORD64 Outbuf;

		BYTE InterruptKeyDown;
		BYTE InterruptKeyUp;
	}KeybData;
}CCEL_EMUCTX, *PCCEL_EMUCTX;

extern PPL2_CONTEXT Ctx;
extern PCCEL_EMUCTX EmuCtx;

void    EmuInit(void);
void    EmuShutdown(void);
void    EmuVideoThread(void);
void    EmuInterruptThread(void);
DWORD64 EmuPopStack(void);
void    EmuPushStack(DWORD64 Value);
DWORD64 EmuVirtualTranslate(DWORD64 Value);

DWORD64 EmudVideoStatusQuery(DWORD Device);
void	EmudVideoSendCommand(DWORD Device, DWORD64 Command);
void	EmudVideoSendData(DWORD Device, DWORD64 Command);
DWORD64	EmudVideoGetData(DWORD Device);
void	EmudVideoReset(DWORD Device);
void	EmudVideoOff(DWORD Device);
void	EmudVideoOn(DWORD Device);

DWORD64 EmudKeyboardStatusQuery(DWORD Device);
void	EmudKeyboardSendCommand(DWORD Device, DWORD64 Command);
void	EmudKeyboardSendData(DWORD Device, DWORD64 Command);
DWORD64	EmudKeyboardGetData(DWORD Device);
void	EmudKeyboardReset(DWORD Device);
void	EmudKeyboardOff(DWORD Device);
void	EmudKeyboardOn(DWORD Device);

DWORD64 EmudFdiskStatusQuery(DWORD Device);
void	EmudFdiskSendCommand(DWORD Device, DWORD64 Command);
void	EmudFdiskSendData(DWORD Device, DWORD64 Command);
DWORD64	EmudFdiskGetData(DWORD Device);
void	EmudFdiskReset(DWORD Device);
void	EmudFdiskOff(DWORD Device);
void	EmudFdiskOn(DWORD Device);
DWORD64 EmudFdiskDevCount(void);

