/*
emu_mmu.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/
#include "emulator.h"

#undef min
#undef max
#define InRange(x, min, max) (x >= min && x <= max)

DWORD64 EmuPopStack(void) {
	EmuCtx->Stack = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Ctx->Sp);

	DWORD64 Return = EmuCtx->Stack[0];
	if ((Ctx->Sp - 8) > Ctx->System.StackPointerLowerBound)
		Ctx->Sp -= 8;

	return Return;
}

void EmuPushStack(DWORD64 Value) {
	EmuCtx->Stack = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Ctx->Sp);
	EmuCtx->Stack[1] = Value;

	if ((Ctx->Sp + 8) < Ctx->System.StackPointerUpperBound)
		Ctx->Sp += 8;

	return;
}

DWORD64 EmuVirtualTranslate(DWORD64 Value) {
	if (!(Ctx->Flags & 0x20))
		return Value; // no virtual memory
	
	EmuCtx->ActivePageTable = EmuCtx->PhysicalMemory + Ctx->System.PageStart;
	DWORD PageCount = (Ctx->System.PageEnd - Ctx->System.PageStart) / sizeof(PL2_PAGE);

	for (DWORD i = 0; i < PageCount; i++) {
		PPL2_PAGE ThisPage = &EmuCtx->ActivePageTable[i];

		if (!ThisPage->Active)
			continue;
		
		if (!InRange(Value, ThisPage->Virtual, ThisPage->Virtual + ThisPage->Size))
			continue;

		DWORD64 VirtualOffset = Value - ThisPage->Virtual;
		return ThisPage->Physical + VirtualOffset;
	}

	return 0;
}