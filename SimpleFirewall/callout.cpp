#pragma once
#include "commonheader.h"

#if 0
NTSTATUS
RegisterCalloutForLayer(
	IN const GUID* layerKey,
	IN const GUID* calloutKey,
	IN FWPS_CALLOUT_CLASSIFY_FN classifyFn,
	IN FWPS_CALLOUT_NOTIFY_FN notifyFn,
	IN FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN flowDeleteNotifyFn,
	OUT UINT32* calloutId,
	OUT UINT64* filterId
)
{
	NTSTATUS        status = STATUS_SUCCESS;

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

	status = FwpsCalloutRegister(g_pDevice, &sCallout, calloutId);
	if (!NT_SUCCESS(status))
		goto exit;

	bCalloutRegistered = TRUE;

	mDispData.name = L"Wall ALE Callout";
	mDispData.description = L"Callout that capture the wall acquired event";

	mCallout.applicableLayer = *layerKey;
	mCallout.calloutKey = *calloutKey;
	mCallout.displayData = mDispData;

	status = FwpmCalloutAdd(g_EngineHandle, &mCallout, NULL, NULL);
	if (!NT_SUCCESS(status))
		goto exit;

	mFilter.action.calloutKey = *calloutKey;
	mFilter.action.type = FWP_ACTION_CALLOUT_TERMINATING;
	mFilter.displayData.name = L"Wall Filter";
	mFilter.displayData.description = L"filter that used to capture the wall needed event";
	mFilter.layerKey = *layerKey;
	mFilter.numFilterConditions = 0;
	mFilter.filterCondition = mFilter_condition;
	mFilter.subLayerKey = FWPM_SUBLAYER_UNIVERSAL;
	mFilter.weight.type = FWP_EMPTY;

	//initialize mFilter_condition
	//...........................

	status = FwpmFilterAdd(g_EngineHandle, &mFilter, NULL, filterId);

	if (!NT_SUCCESS(status))
		goto exit;

exit:
	if (!NT_SUCCESS(status))
	{
		if (bCalloutRegistered)
		{
			FwpsCalloutUnregisterById(*calloutId);
		}
	}
	return status;
}

#endif // 0

NTSTATUS
FirewallRegisterCallouts() 
{
	return STATUS_SUCCESS;
}

NTSTATUS
FirewallUnRegisterCallouts() 
{
	return STATUS_SUCCESS;
}