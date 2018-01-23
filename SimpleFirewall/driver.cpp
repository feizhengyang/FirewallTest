#include "commonheader.h"

GLOBAL_VARIABLE global;

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

	global.pDevice = pDevice;

	// start monitor
	if (!NT_SUCCESS(ntStatus = FirewallRegisterCallouts()))
		goto ERROR_EXIT;


ERROR_EXIT:

	if (!NT_SUCCESS(ntStatus))
	{
		// delete filter unregister callout
		FirewallUnRegisterCallouts();

		// delete device object
		if (global.pDevice)
		{
			IoDeleteDevice(global.pDevice);
			global.pDevice = NULL;
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

	// delete filter unregister callout
	FirewallUnRegisterCallouts();

	if (global.pDevice)
	{
		IoDeleteDevice(global.pDevice);
		global.pDevice = NULL;
	}

	DbgPrint("fzy : <- DriverUnload\n");
	return;
}
