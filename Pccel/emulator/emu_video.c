/*
emu_video.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#include "emulator.h"

void EmudiLine(DWORD64 Data, DWORD32 Color);
void EmudiRect(DWORD64 Data, DWORD32 Color);
void EmudiFill(DWORD64 Data, DWORD32 Color);
void EmudiCopy(DWORD64 Data, DWORD64 Location);

DWORD64 EmudVideoStatusQuery(DWORD Device) {
	return DEVSTATUS_GOOD;
}

void EmudVideoSendCommand(DWORD Device, DWORD64 Command) {
	EmuCtx->VideoData.Command = Command;
	switch (Command) {
	case 0x00:
		EmuCtx->VideoData.Outbuf = 0;
		break;
	case 0x06:
		EmuCtx->VideoData.Outbuf = (EmuCtx->ScreenSizeX << 16) | EmuCtx->ScreenSizeY;
		break;
	}

	return;
}

void EmudVideoSendData(DWORD Device, DWORD64 Command) {
	switch (EmuCtx->VideoData.Command) {
	case 0x01: // deprecated, get text buffer
		break;
	case 0x02: // line
		EmudiLine(Command, EmuPopStack());
		break;
	case 0x03: // outline rectangle
		EmudiRect(Command, EmuPopStack());
		break;
	case 0x04: // filled rect
		EmudiFill(Command, EmuPopStack());
		break;
	case 0x05: // copy rect
		EmudiCopy(Command, EmuPopStack());
		break;
	}

	return;
}

DWORD64	EmudVideoGetData(DWORD Device) {
	return EmuCtx->VideoData.Outbuf;
}

void EmudVideoReset(DWORD Device) {
	return;
}

void EmudVideoOff(DWORD Device) {
	PPL2_DEVICE VideoDevice = &EmuCtx->DeviceList[0];
	VideoDevice->Flags.On = 0;
	return;
}

void EmudVideoOn(DWORD Device) {
	PPL2_DEVICE VideoDevice = &EmuCtx->DeviceList[0];
	VideoDevice->Flags.On = 1;
	return;
}

#define GET16_HIHI(x) (DWORD64)((x & 0xFFFF000000000000) >> 48)
#define GET16_HILO(x) (DWORD64)((x & 0x0000FFFF00000000) >> 32)
#define GET16_LOHI(x) (DWORD64)((x & 0x00000000FFFF0000) >> 16)
#define GET16_LOLO(x) (DWORD64)((x & 0x000000000000FFFF) >> 00)
#define R(x) (Uint8)((DWORD32)(x & 0xFF000000) >> 24)
#define G(x) (Uint8)((DWORD32)(x & 0x00FF0000) >> 16)
#define B(x) (Uint8)((DWORD32)(x & 0x0000FF00) >> 8)
#define A(x) (Uint8)((DWORD32)(x & 0x000000FF) >> 0)

void EmudiLine(DWORD64 Data, DWORD32 Color) {
	WORD X = GET16_HIHI(Data);
	WORD Y = GET16_HILO(Data);
	WORD W = GET16_LOHI(Data);
	WORD H = GET16_LOLO(Data);

	EmuCtx->EmuPauseDrawing = 1;
	SDL_SetRenderDrawColor(EmuCtx->EmuRenderer, R(Color), G(Color), B(Color), A(Color));
	SDL_RenderDrawLine(EmuCtx->EmuRenderer, X, Y, W, H);
	EmuCtx->EmuPauseDrawing = 0;

	return;
}

void EmudiRect(DWORD64 Data, DWORD32 Color) {
	WORD X = GET16_HIHI(Data);
	WORD Y = GET16_HILO(Data);
	WORD W = GET16_LOHI(Data);
	WORD H = GET16_LOLO(Data);

	SDL_Rect Rect;
	Rect.x = X;
	Rect.y = Y;
	Rect.w = W;
	Rect.h = H;

	EmuCtx->EmuPauseDrawing = 1;
	SDL_SetRenderDrawColor(EmuCtx->EmuRenderer, R(Color), G(Color), B(Color), A(Color));
	SDL_RenderDrawRect(EmuCtx->EmuRenderer, &Rect);
	EmuCtx->EmuPauseDrawing = 0;

	return;
}

void EmudiFill(DWORD64 Data, DWORD32 Color) {
	WORD X = GET16_HIHI(Data);
	WORD Y = GET16_HILO(Data);
	WORD W = GET16_LOHI(Data);
	WORD H = GET16_LOLO(Data);

	SDL_Rect Rect;
	Rect.x = X;
	Rect.y = Y;
	Rect.w = W;
	Rect.h = H;

	EmuCtx->EmuPauseDrawing = 1;
	SDL_SetRenderDrawColor(EmuCtx->EmuRenderer, R(Color), G(Color), B(Color), A(Color));
	SDL_RenderFillRect(EmuCtx->EmuRenderer, &Rect);
	EmuCtx->EmuPauseDrawing = 0;

	return;
}

#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff

void EmudiCopy(DWORD64 Data, DWORD64 Location) {
	WORD X = GET16_HIHI(Data);
	WORD Y = GET16_HILO(Data);
	WORD W = GET16_LOHI(Data);
	WORD H = GET16_LOLO(Data);

	SDL_Rect Rect;
	Rect.x = X;
	Rect.y = Y;
	Rect.w = W;
	Rect.h = H;

	DWORD64 VirtAddr = EmuVirtualTranslate(Location);
	SDL_Surface* NewSurface = SDL_CreateRGBSurfaceFrom(EmuCtx->PhysicalMemory + VirtAddr, W, H, 32,
		32 * W, RMASK, GMASK, BMASK, AMASK);

	EmuCtx->EmuPauseDrawing = 1;
	SDL_Texture* TransTexture = SDL_CreateTextureFromSurface(EmuCtx->EmuRenderer, NewSurface);
	SDL_FreeSurface(NewSurface);
	SDL_RenderCopy(EmuCtx->EmuRenderer, TransTexture, NULL, &Rect);
	SDL_DestroyTexture(TransTexture);
	EmuCtx->EmuPauseDrawing = 0;

	return;
}