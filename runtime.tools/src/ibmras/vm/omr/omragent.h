/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#if !defined(OMRAGENT_H_INCLUDED)
#define OMRAGENT_H_INCLUDED

#include "j9comp.h"
#include "omr.h"
#include "ute_core.h"

/*
 * This header file should be used by agent writers.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define OMR_TI_VERSION_0 0

typedef struct OMR_TI {
	I_32 version;
	omr_error_t (*AttachCurrentThread)(OMR_VM *vm, OMR_VMThread **vmThread);
	omr_error_t (*DetachCurrentThread)(OMR_VMThread *vmThread);

	omr_error_t	(*RegisterRecordSubscriber)(OMR_VMThread *vmThread, char const *description,
		utsSubscriberCallback subscriberFunc, utsSubscriberAlarmCallback alarmFunc, void *userData,	UtSubscription **subscriptionID);
	omr_error_t (*DeregisterRecordSubscriber)(OMR_VMThread *vmThread, UtSubscription *subscriptionID, utsSubscriberAlarmCallback alarmFunc);
	omr_error_t (*GetTraceMetadata)(OMR_VMThread *vmThread, void **data, I_32 *length);
	omr_error_t (*SetTraceOptions)(OMR_VMThread *vmThread, char const *opts[]);
} OMR_TI;

/*
 * Required agent entry points:
 */
#if defined(_MSC_VER)
I_32 __cdecl OMRAgent_OnLoad(OMR_TI const *ti, OMR_VM *vm, char const *options);
void __cdecl OMRAgent_OnUnload(OMR_TI const *ti, OMR_VM *vm);
#else
/**
 * Invoked when an agent is loaded.
 *
 * Don't assume any VM threads exist when OnLoad is invoked.
 *
 * @param[in] ti The OMR tooling interface.
 * @param[in] vm The OMR vm.
 * @param[in] options A read-only string. Don't free or modify it.
 * @return OMR_ERROR_NONE for success, an OMR error code otherwise.
 */
omr_error_t OMRAgent_OnLoad(OMR_TI const *ti, OMR_VM *vm, char const *options);

/**
 * Invoked when an agent is unloaded.
 *
 * Don't assume any VM threads exist when OnUnload is invoked.
 * Don't assume that OnUnload is invoked from the same thread that invoked OnLoad.
 *
 * @param[in] ti The OMR tooling interface.
 * @param[in] vm The OMR vm.
 */
void OMRAgent_OnUnload(OMR_TI const *ti, OMR_VM *vm);
#endif

#ifdef __cplusplus
}
#endif

#endif /* OMRAGENT_H_INCLUDED */
