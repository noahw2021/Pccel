/*
exp_memory.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#include "../exports.h"
#include "../emulator/emulator.h"

__int64 CciRead64(__int64 Address) {
	__int64* Physical = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Address);
	return *Physical;
}

__int32 CciRead32(__int64 Address) {
	__int32* Physical = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Address);
	return *Physical;
}

__int16 CciRead16(__int64 Address) {
	__int16* Physical = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Address);
	return *Physical;
}

__int8  CciRead08(__int64 Address) {
	__int8* Physical = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Address);
	return *Physical;
}

void CciWrite64(__int64 Address, __int64 Value) {
	__int64* Physical = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Address);
	*Physical = Value;
	return;
}

void CciWrite32(__int64 Address, __int32 Value) {
	__int32* Physical = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Address);
	*Physical = Value;
	return;
}

void CciWrite16(__int64 Address, __int16 Value) {
	__int16* Physical = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Address);
	*Physical = Value;
	return;
}

void CciWrite08(__int64 Address, __int8  Value) {
	__int8* Physical = EmuCtx->PhysicalMemory + EmuVirtualTranslate(Address);
	*Physical = Value;
	return;
}

PL2_CONTEXT CciGetContext(void) {
	PL2_CONTEXT Return;
	memcpy(&Return, Ctx, sizeof(PL2_CONTEXT));
	return Return;
}

void CciSetContext(PL2_CONTEXT Context) {
	memcpy(Ctx, &Context, sizeof(PL2_CONTEXT));
	return;
}

void CciVirtualMemoryEnable(void) {
	Ctx->Flags |= 0x20; // VF
	return;
}

void CciVirtualMemoryDisable(void) {
	Ctx->Flags &= ~0x20;
	return;
}

__int64 CciVirtualMemoryCreate(__int64 PhysicalAddress, __int64 Size, __int8 Permissions) {
	PPL2_PAGE PageTable = EmuCtx->PhysicalMemory + Ctx->System.PageStart;
	DWORD PageCount = (Ctx->System.PageEnd - Ctx->System.PageStart) / sizeof(PL2_PAGE);

	for (DWORD i = 0; i < PageCount; i++) {
		PPL2_PAGE ThisPage = &PageTable[i];

		if (!ThisPage->Active) {
			ThisPage->Permissions = Permissions;
			ThisPage->Active = 1;

			ThisPage->Physical = PhysicalAddress;
			ThisPage->Size = Size;
			ThisPage->Virtual = Ctx->System.PageMaxLocation;
			
			Ctx->System.PageMaxLocation += Size * 2;
			return ThisPage->Virtual;
		}
	}

	return NULL;
}

#undef min
#undef max
#define InRange(x, min, max) (x >= min && x <= max)

void CciVirtualMemoryDelete(__int64 VirtualAddress) {
	PPL2_PAGE PageTable = EmuCtx->PhysicalMemory + Ctx->System.PageStart;
	DWORD PageCount = (Ctx->System.PageEnd - Ctx->System.PageStart) / sizeof(PL2_PAGE);

	for (DWORD i = 0; i < PageCount; i++) {
		PPL2_PAGE ThisPage = &PageTable[i];

		if (InRange(VirtualAddress, ThisPage->Virtual, ThisPage->Virtual + ThisPage->Size))
			ThisPage->Active = 0;
	}

	return;
}

void CciVirtualSetTableLow(__int64 TableLow) {
	Ctx->System.PageStart = TableLow;
	return;
}
	
void CciVirtualSetTableHigh(__int64 TableHigh) {
	Ctx->System.PageEnd = TableHigh;
	return;
}

void CciVirtualSecurityIncrement(void) {
	Ctx->Security++;
	Ctx->Security %= 32;
	return;
}

void CciVirtualSecurityDecrement(void) {
	Ctx->Security--;
	Ctx->Security %= 32;
	return;
}

__int64 CciStackPtrGet(void) {
	return Ctx->Sp;
}

void CciStackPtrSet(__int64 NewSp) {
	Ctx->Sp = NewSp;
	return;
}