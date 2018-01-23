#pragma once

NTSTATUS 
FirewallRegisterCallouts();

NTSTATUS
FirewallUnRegisterCallouts();

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
);

NTSTATUS NTAPI
FirewallALEConnectNotifyV1(
	FWPS_CALLOUT_NOTIFY_TYPE notifyType,
	const GUID *filterKey,
	FWPS_FILTER1 *filter
);

void NTAPI
FirewallALEFlowDeleteNotifyV1(
	UINT16 layerId,
	UINT32 calloutId,
	UINT64 flowContext
);

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
);

NTSTATUS NTAPI
FirewallALERecvAcceptNotifyV1(
	FWPS_CALLOUT_NOTIFY_TYPE notifyType,
	const GUID *filterKey,
	FWPS_FILTER1 *filter
);

void NTAPI
FirewallALERecvAcceptNotifyV1(
	UINT16 layerId,
	UINT32 calloutId,
	UINT64 flowContext
);
