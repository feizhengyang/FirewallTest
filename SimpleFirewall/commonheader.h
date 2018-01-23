#pragma once
#pragma warning(push)
#pragma warning(disable: 4201) /// NAMELESS_STRUCT_UNION
#pragma warning(disable: 4324) /// STRUCTURE_PADDED
#define NDIS_SUPPORT_NDIS6			1
#define INIT_GUID
#include <initguid.h>
#include <fwpmk.h>                    /// Inc
#include <Fwpsk.h>
#include <stdlib.h>                   /// SDK\Inc\CRT
#pragma warning(pop)
#include "calloutguid.h"
#include "callout.h"

// Macro
#define DEVICE_NAME L"\\Device\\FirewallDevice"

// Structure
typedef struct _GLOBAL_VARIABLE {
	PDEVICE_OBJECT		pDevice;
	HANDLE				hEngineHandle;
	UINT32				n32ConnectCalloutId;
	UINT64				n64ConnectFilterId;
	UINT32				n32RecvAcceptCalloutId;
	UINT64				n64AcceptRecvFilterId;
}GLOBAL_VARIABLE;

// Global variable
extern GLOBAL_VARIABLE global;
