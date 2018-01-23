#include "commonheader.h"

// Macro
#define DEVICE_NAME L"\\Device\\FirewallDevice"

// Global variable
PDEVICE_OBJECT		g_pDevice;
HANDLE				g_EngineHandle;

extern "C"
{
NTSTATUS
DriverEntry(
	IN  PDRIVER_OBJECT          driverObject,
	IN  PUNICODE_STRING         registryPath
);

VOID
DriverUnload(
	IN  PDRIVER_OBJECT          driverObject
);
}

NTSTATUS
DriverEntry(
	IN  PDRIVER_OBJECT          driverObject,
	IN  PUNICODE_STRING         registryPath
) 
{
	UNREFERENCED_PARAMETER(driverObject);
	UNREFERENCED_PARAMETER(registryPath);

	DbgPrint("fzy : -> DriverEntry\n");

	driverObject->DriverUnload = DriverUnload;

	// create device object
	NTSTATUS        ntStatus = STATUS_SUCCESS;
	UNICODE_STRING  deviceName = { 0 };
	PDEVICE_OBJECT	pDevice;

	ntStatus = IoCreateDevice(driverObject,
				0,
				&deviceName,
				FILE_DEVICE_NETWORK,
				0,
				FALSE,
				&pDevice);
	if (!NT_SUCCESS(ntStatus))
		goto ERROR_EXIT;

	g_pDevice = pDevice;

	// start monitor
	if (!NT_SUCCESS(ntStatus = FirewallRegisterCallouts()))
		goto ERROR_EXIT;


ERROR_EXIT:

	if (!NT_SUCCESS(ntStatus))
	{
		// unregister 
		FirewallUnRegisterCallouts();

		// delete device object
		if (g_pDevice)
		{
			IoDeleteDevice(g_pDevice);
			g_pDevice = NULL;
		}
	}

	DbgPrint("fzy : <- DriverEntry : 0x%08x\n", ntStatus);
	return ntStatus;
}

VOID
DriverUnload(
	IN  PDRIVER_OBJECT          driverObject
)
{
	UNREFERENCED_PARAMETER(driverObject);

	DbgPrint("fzy : -> DriverUnload\n");

	if (g_pDevice)
	{
		IoDeleteDevice(g_pDevice);
		g_pDevice = NULL;
	}

	DbgPrint("fzy : <- DriverUnload\n");
	return;
}
