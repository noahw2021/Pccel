#pragma once
/*
exports.h
Pccel

PLASM2 Compiled C Emulation Layer
(c) Noah Wooten 2023, All Rights Reserved
*/

#define CCEL_API __declspec(dllexport)

CCEL_API void CcInit(void);
CCEL_API void CcShutdown(void);

CCEL_API void CciSetHaltFlag(void);

CCEL_API __int64 CciRead64(__int64 Address);
CCEL_API __int32 CciRead32(__int64 Address);
CCEL_API __int16 CciRead16(__int64 Address);
CCEL_API __int8  CciRead08(__int64 Address);
CCEL_API   void CciWrite64(__int64 Address, __int64 Value);
CCEL_API   void CciWrite32(__int64 Address, __int32 Value);
CCEL_API   void CciWrite16(__int64 Address, __int16 Value);
CCEL_API   void CciWrite08(__int64 Address, __int8  Value);

typedef struct _PL2_CONTEXT {
	union {
		union {
			__int64 Registers64[32];
			struct {
				__int64 GPRs[16];
				__int64 Special[16];
			};
		}AllRegisters;
		struct {
			__int64 GPRs[16];
			__int64 Ip;
			__int64 Sp;
			__int64 Flags;
			__int64 Security;

			struct {
				__int64 PageStart;
				__int64 PageEnd;
				__int64 ReturnAddressLocation;
				__int64 InterruptTable;
				__int64 VirtualStackPointer;
				__int64 CSMHandler;
				__int64 DevicePointer;
				__int64 StackPointerUpperBound;
				__int64 StackPointerLowerBound;
				__int64 PageMaxLocation;

				union {
					__int64 Reserved0;
					struct {
						__int64 InterruptReturn : 1;
						__int64 InInterrupt : 1;
						__int64 InTable : 1;

						__int64 Reserved : 61;
					};
				}PccelFlags;

				__int64 Reserved[1];
			}System;
		};
	};
}PL2_CONTEXT, *PPL2_CONTEXT;

CCEL_API PL2_CONTEXT CciGetContext(void);
CCEL_API        void CciSetContext(PL2_CONTEXT Context);

CCEL_API void    CciVirtualMemoryEnable(void);
CCEL_API void    CciVirtualMemoryDisable(void);
CCEL_API __int64 CciVirtualMemoryCreate(__int64 PhysicalAddress, __int64 Size, __int8 Permissions);
CCEL_API void    CciVirtualMemoryDelete(__int64 VirtualAddress);
CCEL_API void    CciVirtualSetTableLow(__int64 TableLow);
CCEL_API void    CciVirtualSetTableHigh(__int64 TableHigh);

CCEL_API void CciVirtualSecurityIncrement(void);
CCEL_API void CciVirtualSecurityDecrement(void);

CCEL_API __int64 CciStackPtrGet(void);
CCEL_API void    CciStackPtrSet(__int64 NewSp);

CCEL_API __int64 CciDeviceStatusQuery(__int64 DeviceId);
CCEL_API    void CciDeviceSendCommand(__int64 DeviceId, __int64 Command);
CCEL_API    void CciDeviceSendData(__int64 DeviceId, __int64 Data);
CCEL_API __int64 CciDeviceGetData(__int64 DeviceId);
CCEL_API    void CciDeviceSetOn(__int64 DeviceId);
CCEL_API    void CciDeviceSetOff(__int64 DeviceId);
CCEL_API    void CciDeviceSetReset(__int64 DeviceId);
CCEL_API __int64 CciDeviceGetCount(void);

CCEL_API void CciInterruptCall(__int64 InterruptId);
CCEL_API void CciInterruptHandle(__int64 InterruptId, __int64 VirtualAddress, __int8 SecurityLevel);
CCEL_API void CciInterruptEnable(void);
CCEL_API void CciInterruptDisable(void);
CCEL_API void CciInterruptSetTable(__int64 InterruptTable);
CCEL_API void CciInterruptSetCSM(__int64 CSMHandler);