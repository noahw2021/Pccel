/*
exp_devices.c
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#include "../exports.h"
#include "../emulator/emulator.h"

__int64 CciDeviceStatusQuery(__int64 DeviceId) {
	__int64 DevId = DeviceId % EmuCtx->DeviceCount;
	PPL2_DEVICE ThisDevice = &EmuCtx->DeviceList[DevId];

	return ThisDevice->Callbacks[0](DevId, 0);
}

void CciDeviceSendCommand(__int64 DeviceId, __int64 Command) {
	__int64 DevId = DeviceId % EmuCtx->DeviceCount;
	PPL2_DEVICE ThisDevice = &EmuCtx->DeviceList[DevId];
 
	ThisDevice->Callbacks[1](DevId, Command);
	return;
}

void CciDeviceSendData(__int64 DeviceId, __int64 Data) {
	__int64 DevId = DeviceId % EmuCtx->DeviceCount;
	PPL2_DEVICE ThisDevice = &EmuCtx->DeviceList[DevId];

	ThisDevice->Callbacks[2](DevId, Data);
	return;
}

__int64 CciDeviceGetData(__int64 DeviceId) {
	__int64 DevId = DeviceId % EmuCtx->DeviceCount;
	PPL2_DEVICE ThisDevice = &EmuCtx->DeviceList[DevId];

	return ThisDevice->Callbacks[3](DevId, 0);
}

void CciDeviceSetOn(__int64 DeviceId) {
	__int64 DevId = DeviceId % EmuCtx->DeviceCount;
	PPL2_DEVICE ThisDevice = &EmuCtx->DeviceList[DevId];

	ThisDevice->Callbacks[4](DevId, 0);
	return;
}

void CciDeviceSetOff(__int64 DeviceId) {
	__int64 DevId = DeviceId % EmuCtx->DeviceCount;
	PPL2_DEVICE ThisDevice = &EmuCtx->DeviceList[DevId];

	ThisDevice->Callbacks[5](DevId, 0);
	return;
}

void CciDeviceSetReset(__int64 DeviceId) {
	__int64 DevId = DeviceId % EmuCtx->DeviceCount;
	PPL2_DEVICE ThisDevice = &EmuCtx->DeviceList[DevId];

	ThisDevice->Callbacks[6](DevId, 0);
	return;
}

__int64 CciDeviceGetCount(void) {
	return EmuCtx->DeviceCount;
}