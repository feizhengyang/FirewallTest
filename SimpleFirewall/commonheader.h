#pragma once
#if 1
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

#include "callout.h"
#else
#include <wdm.h>
#endif

extern PDEVICE_OBJECT		g_pDevice;
extern HANDLE				g_EngineHandle;
