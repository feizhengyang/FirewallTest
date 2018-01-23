#pragma once
#include "commonheader.h"

NTSTATUS
RegisterCalloutForLayer(
	IN HANDLE hEngine,	
	IN const GUID* layerKey,
	IN const GUID* calloutKey,
	IN FWPS_CALLOUT_CLASSIFY_FN classifyFn,
	IN FWPS_CALLOUT_NOTIFY_FN notifyFn,
	IN FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN flowDeleteNotifyFn,
	OUT UINT32* calloutId,
	OUT UINT64* filterId
)
{
	NTSTATUS        ntStatus = STATUS_SUCCESS;
	FWPS_CALLOUT    sCallout = { 0 };
	FWPM_FILTER     mFilter = { 0 };
	FWPM_FILTER_CONDITION mFilter_condition[1] = { 0 };
	FWPM_CALLOUT    mCallout = { 0 };
	FWPM_DISPLAY_DATA mDispData = { 0 };

	BOOLEAN         bCalloutRegistered = FALSE;

	sCallout.calloutKey = *calloutKey;
	sCallout.classifyFn = classifyFn;
	sCallout.flowDeleteFn = flowDeleteNotifyFn;
	sCallout.notifyFn = notifyFn;

	ntStatus = FwpsCalloutRegister(global.pDevice, &sCallout, calloutId);
	if (!NT_SUCCESS(ntStatus))
		goto exit;

	bCalloutRegistered = TRUE;

	mDispData.name = L"Firewall Callout";
	mDispData.description = L"Firewall Callout Description";
	mCallout.applicableLayer = *layerKey;
	mCallout.calloutKey = *calloutKey;
	mCallout.displayData = mDispData;

	ntStatus = FwpmCalloutAdd(hEngine, &mCallout, NULL, NULL);
	if (!NT_SUCCESS(ntStatus))
		goto exit;

	mFilter.action.calloutKey = *calloutKey;
	mFilter.action.type = FWP_ACTION_CALLOUT_TERMINATING;
	mFilter.displayData.name = L"Firewall Filter";
	mFilter.displayData.description = L"Firewall Filter that used to capture network event";
	mFilter.layerKey = *layerKey;
	mFilter.numFilterConditions = 0;
	mFilter.filterCondition = mFilter_condition;
	mFilter.subLayerKey = FWPM_SUBLAYER_UNIVERSAL;
	mFilter.weight.type = FWP_EMPTY;

	ntStatus = FwpmFilterAdd(hEngine, &mFilter, NULL, filterId);

	if (!NT_SUCCESS(ntStatus))
		goto exit;

exit:
	if (!NT_SUCCESS(ntStatus))
	{
		if (bCalloutRegistered)
		{
			FwpsCalloutUnregisterById(*calloutId);
		}

		DbgPrint("fzy : RegisterCalloutForLayer failed : 0x%08x\n", ntStatus);
	}
	return ntStatus;
}

NTSTATUS
FirewallRegisterCallouts() 
{
	NTSTATUS    ntStatus = STATUS_SUCCESS;

	// open engine handle
	FWPM_SESSION session = { 0 };
	session.flags = FWPM_SESSION_FLAG_DYNAMIC;
	ntStatus = FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, &session, &global.hEngineHandle);
	if (!NT_SUCCESS(ntStatus))
		goto ERROR_EXIT;

	// start transaction
	ntStatus = FwpmTransactionBegin(global.hEngineHandle, 0);
	if (!NT_SUCCESS(ntStatus))
		goto ERROR_EXIT;

	// register callout with ALE filter
	ntStatus = RegisterCalloutForLayer(
		global.hEngineHandle,
		&FWPM_LAYER_ALE_AUTH_CONNECT_V4,
		&FIREWALL_CALLOUT_ALE_AUTH_CONNECT_V4,
		FirewallALEConnectClassifyV1,
		FirewallALEConnectNotifyV1,
		FirewallALEFlowDeleteNotifyV1,
		&global.n32ConnectCalloutId,
		&global.n64ConnectFilterId
	);
	if (!NT_SUCCESS(ntStatus))
		goto ERROR_EXIT;

	ntStatus = RegisterCalloutForLayer(
		global.hEngineHandle,
		&FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4,
		&FIREWALL_CALLOUT_ALE_AUTH_RECV_ACCEPT_V4,
		FirewallALERecvAcceptClassifyV1,
		FirewallALERecvAcceptNotifyV1,
		FirewallALERecvAcceptNotifyV1,
		&global.n32RecvAcceptCalloutId,
		&global.n64AcceptRecvFilterId
	);
	if (!NT_SUCCESS(ntStatus))
		goto ERROR_EXIT;

	ntStatus = FwpmTransactionCommit(global.hEngineHandle);
	if (!NT_SUCCESS(ntStatus))
		goto ERROR_EXIT;

ERROR_EXIT:

	if (!NT_SUCCESS(ntStatus))
	{
		if (global.hEngineHandle)
		{
			FwpmTransactionAbort(global.hEngineHandle);
			FwpmEngineClose(global.hEngineHandle);
			global.hEngineHandle = NULL;
		}
		DbgPrint("fzy : FirewallRegisterCallouts failed : 0x%08x\n", ntStatus);
	} else {
		DbgPrint("fzy : FirewallRegisterCallouts exits with Code : 0x%08x\n", ntStatus);
	}


	return ntStatus;
}

NTSTATUS
FirewallUnRegisterCallouts() 
{
	if (global.hEngineHandle)
	{
		FwpmFilterDeleteById(global.hEngineHandle, global.n64AcceptRecvFilterId);
		FwpmFilterDeleteById(global.hEngineHandle, global.n64ConnectFilterId);
		FwpmCalloutDeleteById(global.hEngineHandle, global.n32RecvAcceptCalloutId);
		FwpmCalloutDeleteById(global.hEngineHandle, global.n32ConnectCalloutId);
		FwpsCalloutUnregisterById(global.n32RecvAcceptCalloutId);
		FwpsCalloutUnregisterById(global.n32ConnectCalloutId);
		global.n64AcceptRecvFilterId = 0;
		global.n64ConnectFilterId = 0;
		global.n32RecvAcceptCalloutId = 0;
		global.n32ConnectCalloutId = 0;
		global.hEngineHandle = 0;
	}

	return STATUS_SUCCESS;
}


// callbacks for filter FWPM_LAYER_ALE_AUTH_CONNECT_V4
void NTAPI
FirewallALEConnectClassifyV1(
	_In_ const FWPS_INCOMING_VALUES0* inFixedValues,
	_In_ const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
	_Inout_opt_ void* layerData,
	_In_opt_ const void* classifyContext,
	_In_ const FWPS_FILTER1* filter,
	_In_ UINT64 flowContext,
	_Inout_ FWPS_CLASSIFY_OUT0* classifyOut
) 
{
	UNREFERENCED_PARAMETER(inFixedValues);
	UNREFERENCED_PARAMETER(inMetaValues);
	UNREFERENCED_PARAMETER(layerData);
	UNREFERENCED_PARAMETER(classifyContext);
	UNREFERENCED_PARAMETER(filter);
	UNREFERENCED_PARAMETER(flowContext);
	UNREFERENCED_PARAMETER(classifyOut);
	DbgPrint("fzy : Connect classfy v1");
}

NTSTATUS NTAPI
FirewallALEConnectNotifyV1(
	FWPS_CALLOUT_NOTIFY_TYPE notifyType,
	const GUID *filterKey,
	FWPS_FILTER1 *filter
)
{
	UNREFERENCED_PARAMETER(notifyType);
	UNREFERENCED_PARAMETER(filterKey);
	UNREFERENCED_PARAMETER(filter);
	return STATUS_SUCCESS;
}

void NTAPI
FirewallALEFlowDeleteNotifyV1(
	UINT16 layerId,
	UINT32 calloutId,
	UINT64 flowContext
)
{
	UNREFERENCED_PARAMETER(layerId);
	UNREFERENCED_PARAMETER(calloutId);
	UNREFERENCED_PARAMETER(flowContext);
}

// callbacks for filter FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4
void NTAPI
FirewallALERecvAcceptClassifyV1(
	_In_ const FWPS_INCOMING_VALUES0* inFixedValues,
	_In_ const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
	_Inout_opt_ void* layerData,
	_In_opt_ const void* classifyContext,
	_In_ const FWPS_FILTER1* filter,
	_In_ UINT64 flowContext,
	_Inout_ FWPS_CLASSIFY_OUT0* classifyOut
) 
{
	UNREFERENCED_PARAMETER(inFixedValues);
	UNREFERENCED_PARAMETER(inMetaValues);
	UNREFERENCED_PARAMETER(layerData);
	UNREFERENCED_PARAMETER(classifyContext);
	UNREFERENCED_PARAMETER(filter);
	UNREFERENCED_PARAMETER(flowContext);
	UNREFERENCED_PARAMETER(classifyOut);

	DbgPrint("fzy : Recv accept classfy v1");

}

NTSTATUS NTAPI
FirewallALERecvAcceptNotifyV1(
	FWPS_CALLOUT_NOTIFY_TYPE notifyType,
	const GUID *filterKey,
	FWPS_FILTER1 *filter
)
{
	UNREFERENCED_PARAMETER(notifyType);
	UNREFERENCED_PARAMETER(filterKey);
	UNREFERENCED_PARAMETER(filter);

	return STATUS_SUCCESS;
}

void NTAPI
FirewallALERecvAcceptNotifyV1(
	UINT16 layerId,
	UINT32 calloutId,
	UINT64 flowContext
)
{
	UNREFERENCED_PARAMETER(layerId);
	UNREFERENCED_PARAMETER(calloutId);
	UNREFERENCED_PARAMETER(flowContext);
}