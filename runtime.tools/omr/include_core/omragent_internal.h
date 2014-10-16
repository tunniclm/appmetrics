/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef OMRAGENT_INTERNAL_H
#define OMRAGENT_INTERNAL_H
/**
 * This header file is used internally by OMR library. OMR agents should not use this header file.
 * The structures defined in this file may be changed without notice.
 */

#if defined(WIN32)
/* pdh.h include windows.h which defined UDATA.  Ignore its definition */
#define UDATA UDATA_win32_
#include <pdh.h>
#undef UDATA	/* this is safe because our UDATA is a typedef, not a macro */
#endif /* defined(WIN32) */

#include "j9comp.h"
#include "omragent.h"
#include "thread_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OMR_TI_ENTER_FROM_VM_THREAD(_omrVMThread) \
		BOOLEAN _omrTIAccessMutex_taken = FALSE; \
		if (NULL != (_omrVMThread)) { \
			j9thread_monitor_enter((_omrVMThread)->_vm->_omrTIAccessMutex); \
			_omrTIAccessMutex_taken = TRUE; \
		}

#define OMR_TI_RETURN(_omrVMThread, _rc) \
		if (TRUE == _omrTIAccessMutex_taken) { \
			j9thread_monitor_exit((_omrVMThread)->_vm->_omrTIAccessMutex); \
			return (_rc); \
		} else { \
			return (_rc); \
		}

typedef struct OMRSysInfoProcessCpuTime {
	I_64 timestampNS; 			/* time in nanoseconds from a fixed but arbitrary point in time */
	I_64 systemCpuTimeNS;		/* system time, in nanoseconds, consumed by this process on all CPUs. */
	I_64 userCpuTimeNS;			/* user time, in nanoseconds, consumed by this process on all CPUs. */
	I_32 numberOfCpus;			/* number of CPUs */
} OMRSysInfoProcessCpuTime;

typedef enum OMRSysInfoCpuLoadCallStatus {
	NO_HISTORY,
	SUPPORTED,
	CPU_LOAD_ERROR_VALUE,
	INSUFFICIENT_PRIVILEGE,
	UNSUPPORTED,
	OMRSysInfoCpuLoadCallStatus_EnsureWideEnum = 0x1000000	/* force 4-byte enum */
} OMRSysInfoCpuLoadCallStatus;

#if defined(WIN32)
typedef struct OMR_PdhCounterPath {
	char counterPath[PDH_MAX_COUNTER_PATH];		/* counter path */
	UDATA counterPathStatus;					/* If counter path has been initialized or PDH query has failed */
	j9thread_rwmutex_t counterPathLock;
} OMR_PdhCounterPath;
#endif /* defined(WIN32) */

typedef struct OMR_SysInfo {
	UDATA systemCpuTimeNegativeElapsedTimeCount;		/* consecutive negative elapsed time count for system CPU load calculation */
	UDATA processCpuTimeNegativeElapsedTimeCount;		/* consecutive negative elapsed time count for process CPU load calculation */
	OMRSysInfoCpuLoadCallStatus systemCpuLoadCallStatus;		/* whether GetSystemCpuLoad is supported on this platform or user have sufficient rights */
	OMRSysInfoCpuLoadCallStatus processCpuLoadCallStatus;		/* whether GetProcessCpuLoad is supported on this platform or user have sufficient rights */

	J9SysinfoCPUTime oldestSystemCpuTime;			/* the oldest GetSystemCpuLoad() call record */
	J9SysinfoCPUTime interimSystemCpuTime; 			/* the 2nd oldest GetSystemCpuLoad() call record */
	OMRSysInfoProcessCpuTime oldestProcessCpuTime;	/* the oldest GetProcessCpuLoad() call record */
	OMRSysInfoProcessCpuTime interimProcessCpuTime;	/* the 2nd oldest GetProcessCpuLoad() call record */

	j9thread_monitor_t syncSystemCpuLoad;
	j9thread_monitor_t syncProcessCpuLoad;
#if defined(WIN32)
	OMR_PdhCounterPath counterPath;
#endif /* defined(WIN32) */
} OMR_SysInfo;

omr_error_t omrtiBindCurrentThread(OMR_VM *vm, const char *threadName, OMR_VMThread **vmThread);
omr_error_t omrtiUnbindCurrentThread(OMR_VMThread *vmThread);
omr_error_t omrtiRegisterRecordSubscriber(OMR_VMThread *vmThread, char const *description,
	utsSubscriberCallback subscriberFunc, utsSubscriberAlarmCallback alarmFunc, void *userData, UtSubscription **subscriptionID);
omr_error_t omrtiDeregisterRecordSubscriber(OMR_VMThread *vmThread, UtSubscription *subscriptionID);
omr_error_t omrtiFlushTraceData(OMR_VMThread *vmThread);
omr_error_t omrtiGetTraceMetadata(OMR_VMThread *vmThread, void **data, I_32 *length);
omr_error_t omrtiSetTraceOptions(OMR_VMThread *vmThread, char const *opts[]);

omr_error_t omrtiGetSystemCpuLoad(OMR_VMThread *vmThread, double *systemCpuLoad);
omr_error_t omrtiGetProcessCpuLoad(OMR_VMThread *vmThread, double *processCpuLoad);
omr_error_t omrtiGetMemoryCategories(OMR_VMThread *vmThread, I_32 max_categories, OMR_TI_MemoryCategory *categories_buffer, I_32 *written_count_ptr, I_32 *total_categories_ptr);
omr_error_t omrtiGetFreePhysicalMemorySize(OMR_VMThread *vmThread, U_64 *freePhysicalMemorySize);
omr_error_t omrtiGetProcessVirtualMemorySize(OMR_VMThread *vmThread, U_64 *processVirtualMemorySize);
omr_error_t omrtiGetProcessPhysicalMemorySize(OMR_VMThread *vmThread, U_64 *processPhysicalMemorySize);
omr_error_t omrtiGetProcessPrivateMemorySize(OMR_VMThread *vmThread, U_64 *processPrivateMemorySize);

omr_error_t omrtiGetMethodDescriptions(OMR_VMThread *vmThread, void **methodArray, size_t methodArrayCount,
	OMR_SampledMethodDescription *methodDescriptions, char *nameBuffer, size_t nameBytes,
	size_t *firstRetryMethod, size_t *nameBytesRemaining);
omr_error_t omrtiGetMethodProperties(OMR_VMThread *vmThread, size_t *numProperties, const char ***propertyNames, size_t *sizeofSampledMethodDesc);

/* This is an internal API which is subject to change without notice. Agents must not use this API. */
typedef struct OMR_ThreadAPI {
    IDATA (*j9thread_create)(j9thread_t *handle, UDATA stacksize, UDATA priority, UDATA suspend, j9thread_entrypoint_t entrypoint, void *entryarg, U_32 category);
    IDATA (*j9thread_monitor_init_with_name)(j9thread_monitor_t *handle, UDATA flags, const char *name);
    IDATA (*j9thread_monitor_destroy)(j9thread_monitor_t monitor);
    IDATA (*j9thread_monitor_enter)(j9thread_monitor_t monitor);
    IDATA (*j9thread_monitor_exit)(j9thread_monitor_t monitor);
    IDATA (*j9thread_monitor_wait)(j9thread_monitor_t monitor);
    IDATA (*j9thread_monitor_notify_all)(j9thread_monitor_t monitor);
} OMR_ThreadAPI;

#ifdef __cplusplus
}
#endif

#endif /* OMRAGENT_INTERNAL_H */
