/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef J9THREAD_H
#define J9THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "j9comp.h"

typedef UDATA j9thread_tls_key_t;

#define J9THREAD_PROC

typedef int(J9THREAD_PROC* j9thread_entrypoint_t)(void*);
typedef void(J9THREAD_PROC* j9thread_tls_finalizer_t)(void*);

/* Must be wide enough to support error codes from any platform */
typedef IDATA j9thread_os_errno_t;

typedef struct J9Thread *j9thread_t;
typedef struct J9ThreadMonitor *j9thread_monitor_t;
typedef struct J9Semaphore *j9sem_t;

#include "j9thread_generated.h"

#define J9THREAD_PRIORITY_MIN  0
#define J9THREAD_PRIORITY_USER_MIN  1
#define J9THREAD_PRIORITY_NORMAL  5
#define J9THREAD_PRIORITY_USER_MAX  10
#define J9THREAD_PRIORITY_MAX 11

/*
A thread priority calue can be one of three possible types: a java thread priority, a j9thread thread priority or a native operating system priority.
A single integer can be used to store both the priority value and type, and these macros allow for this storage and conversion to the desired java
or j9thread priority value.
*/
#define PRIORITY_INDICATOR_VALUE(priority) ((priority) & 0x00ffffff)
#define PRIORITY_INDICATOR_TYPE(priority) ((priority) >> 24)
#define PRIORITY_INDICATOR_ADJUSTED_TYPE(type) ((type) << 24)
#define PRIORITY_INDICATOR_J9THREAD_PRIORITY 0
#define PRIORITY_INDICATOR_JAVA_PRIORITY 1
#define PRIORITY_INDICATOR_NATIVE_PRIORITY 2

#define getJavaPriorityFromIndicator(vm, value) ((PRIORITY_INDICATOR_TYPE(value) == PRIORITY_INDICATOR_JAVA_PRIORITY) ? PRIORITY_INDICATOR_VALUE(value) : \
	((PRIORITY_INDICATOR_TYPE(value) == PRIORITY_INDICATOR_NATIVE_PRIORITY) ? (vm)->j9Thread2JavaPriorityMap[j9thread_map_native_priority(PRIORITY_INDICATOR_VALUE(value))] : \
	(vm)->j9Thread2JavaPriorityMap[PRIORITY_INDICATOR_VALUE(value)]))

#define getJ9ThreadPriorityFromIndicator(vm, value) ((PRIORITY_INDICATOR_TYPE(value) == PRIORITY_INDICATOR_JAVA_PRIORITY) ? (vm)->java2J9ThreadPriorityMap[PRIORITY_INDICATOR_VALUE(value)] : \
	((PRIORITY_INDICATOR_TYPE(value) == PRIORITY_INDICATOR_NATIVE_PRIORITY) ? j9thread_map_native_priority(PRIORITY_INDICATOR_VALUE(value)) : PRIORITY_INDICATOR_VALUE(value)))




#define J9THREAD_LOCKING_DEFAULT	0 /* default locking policy for platform */
#define J9THREAD_LOCKING_NO_DATA	(-1)	/* if no policy data is provided */


#define J9THREAD_LIB_FLAG_JLM_HAS_BEEN_ENABLED  0x20000
#define J9THREAD_LIB_FLAG_JLM_SLOW_SAMPLING_ENABLED  0x200000
#define J9THREAD_LIB_FLAG_FAST_NOTIFY  1
#define J9THREAD_LIB_FLAG_JLMHST_ENABLED  0x10000
#define J9THREAD_LIB_FLAG_JLM_ENABLED  0x4000
#define J9THREAD_LIB_FLAG_JLM_ENABLED_ALL  0x1C000
#define J9THREAD_LIB_FLAG_NO_SCHEDULING  4
#define J9THREAD_LIB_YIELD_ALGORITHM_SCHED_YIELD  0
#define J9THREAD_LIB_FLAG_TRACING_ENABLED  8
#define J9THREAD_LIB_FLAG_JLM_TIME_STAMPS_ENABLED  0x8000
#define J9THREAD_LIB_FLAG_JLM_INFO_SAMPLING_ENABLED  0x300000
#define J9THREAD_LIB_FLAG_DESTROY_MUTEX_ON_MONITOR_FREE  0x400000
#define J9THREAD_LIB_FLAG_JLM_HOLDTIME_SAMPLING_ENABLED  0x100000
#define J9THREAD_LIB_FLAG_JLM_TRACING_REQUIRED  0x304000
#define J9THREAD_LIB_YIELD_ALGORITHM_CONSTANT_USLEEP  2
#define J9THREAD_LIB_YIELD_ALGORITHM_INCREASING_USLEEP  3
#define J9THREAD_LIB_FLAG_ATTACHED_THREAD_EXITED  2

#include "thread_api.h"


#define j9thread_monitor_init(pMon,flags)  j9thread_monitor_init_with_name(pMon,flags, #pMon)

#define j9thread_monitor_set_name(pMon,pName) /* fn on death row */

/* Eye-catcher to identify j9threads that did not survive a fork and should no longer be used. */
#define J9THREAD_POST_FORK_POISONING_EYECATCHER 0xDEADBEEF 

#ifdef __cplusplus
}
#endif

#endif /* J9THREAD_H */
