/*
exp_ints.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#include "../exports.h"
#include "../emulator/emulator.h"

void CciInterruptCall(__int64 InterruptId) {
	PPL2_INTERRUPT Interrupt = NULL;

	if (EmuCtx->CpuInterruptCount == EmuCtx->CpuInterruptCountMax) {
		EmuCtx->CpuInterruptCountMax += 64;
		EmuCtx->CpuInterrupts = realloc(EmuCtx->CpuInterrupts, sizeof(PPL2_INTERRUPT) *
			EmuCtx->CpuInterruptCountMax);
	}

	Interrupt = &EmuCtx->CpuInterrupts[EmuCtx->CpuInterruptCount++];
	Interrupt->Argument = 0x00;
	Interrupt->Interrupt = InterruptId;
	return;
}

void CciInterruptHandle(__int64 InterruptId, __int64 VirtualAddress, __int8 SecurityLevel) {
	PDWORD64 Handler = EmuCtx->PhysicalMemory + Ctx->System.InterruptTable;

	DWORD64 VirtualAddr = VirtualAddress & 0x00FFFFFFFFFFFFFF;
	VirtualAddress |= (SecurityLevel << 56);

	Handler[InterruptId] = VirtualAddr;
	return;
}

void CciInterruptEnable(void) {
	Ctx->Flags |= 0x10;
	return;
}

void CciInterruptDisable(void) {
	Ctx->Flags &= ~0x10;
	return;
}

void CciInterruptSetTable(__int64 InterruptTable) {
	Ctx->System.InterruptTable = InterruptTable;
	return;
}

void CciInterruptSetCSM(__int64 CSMHandler) {
	Ctx->System.CSMHandler = CSMHandler;
	return;
}