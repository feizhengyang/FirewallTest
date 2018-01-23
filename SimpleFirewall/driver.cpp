#include <wdm.h>

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

	DbgPrint("fzy : <- DriverEntry\n");

	return STATUS_SUCCESS;
}

VOID
DriverUnload(
	IN  PDRIVER_OBJECT          driverObject
)
{
	UNREFERENCED_PARAMETER(driverObject);


	DbgPrint("fzy : -> DriverUnload\n");
	DbgPrint("fzy : <- DriverUnload\n");
	return;
}
