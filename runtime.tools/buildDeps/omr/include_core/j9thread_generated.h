/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef J9THREADGENERATED_H
#define J9THREADGENERATED_H

#include "j9comp.h"
/* Reducing the TLS slots from 127 to 123 on ZOS and from 128 to 124 on the rest
 * to compensate for adding thread category related functionality
 */
#if defined(J9ZOS390)
#define J9THREAD_MAX_TLS_KEYS 123   /* One of the TLS slots is replaced by os_errno2 on ZOS */
#else /* !J9ZOS390 */  
#define J9THREAD_MAX_TLS_KEYS 124
#endif /* !J9ZOS390 */

#define J9THREAD_CATEGORY_SYSTEM_THREAD				0x1
/* GC and JIT are also SYSTEM threads, so they have the SYSTEM bit set */
#define J9THREAD_CATEGORY_SYSTEM_GC_THREAD			(0x2 | J9THREAD_CATEGORY_SYSTEM_THREAD)
#define J9THREAD_CATEGORY_SYSTEM_JIT_THREAD			(0x4 | J9THREAD_CATEGORY_SYSTEM_THREAD)

#define J9THREAD_CATEGORY_RESOURCE_MONITOR_THREAD	0x10

#define J9THREAD_CATEGORY_APPLICATION_THREAD		0x100
#define J9THREAD_USER_DEFINED_THREAD_CATEGORY_1		(0x1000 | J9THREAD_CATEGORY_APPLICATION_THREAD)
#define J9THREAD_USER_DEFINED_THREAD_CATEGORY_2		(0x2000 | J9THREAD_CATEGORY_APPLICATION_THREAD)
#define J9THREAD_USER_DEFINED_THREAD_CATEGORY_3		(0x3000 | J9THREAD_CATEGORY_APPLICATION_THREAD)
#define J9THREAD_USER_DEFINED_THREAD_CATEGORY_4		(0x4000 | J9THREAD_CATEGORY_APPLICATION_THREAD)
#define J9THREAD_USER_DEFINED_THREAD_CATEGORY_5		(0x5000 | J9THREAD_CATEGORY_APPLICATION_THREAD)

#define USER_DEFINED_THREAD_CATEGORY_MASK			0xF000
#define USER_DEFINED_THREAD_CATEGORY_BIT_SHIFT		12

#define TYPE_SET_CREATE		1
#define TYPE_SET_ATTACH		2
#define TYPE_SET_MODIFY		3
#define TYPE_SET_GC			4

#define NUM_USER_DEFINED_THREAD_CATEGORY	5

typedef struct J9ThreadsCpuUsage {
	int64_t timestamp;
	int64_t applicationCpuTime;
	int64_t resourceMonitorCpuTime;
	int64_t systemJvmCpuTime;
	int64_t gcCpuTime;
	int64_t jitCpuTime;
	int64_t applicationUserCpuTime[NUM_USER_DEFINED_THREAD_CATEGORY];
} J9ThreadsCpuUsage;

typedef struct J9ThreadTracing {
#if defined(J9VM_THR_JLM_HOLD_TIMES)
    uintptr_t pause_count;
#endif /* J9VM_THR_JLM_HOLD_TIMES */
} J9ThreadTracing;

#define J9_ABSTRACT_THREAD_FIELDS_1 \
    struct J9ThreadLibrary* library; \
    uintptr_t attachcount; \
    uintptr_t priority; \
    struct J9ThreadMonitor* monitor; \
    struct J9Thread* next; \
    struct J9Thread* prev; \
    void* tls[J9THREAD_MAX_TLS_KEYS]; \
    j9thread_entrypoint_t entrypoint; \
    void* entryarg; \
    uintptr_t flags; \
    uintptr_t tid; \
    struct J9Thread* interrupter; \
    uint32_t category; \
    uint32_t effective_category; \
    int64_t lastCategorySwitchTime;

#if defined(J9VM_THR_JLM_HOLD_TIMES)
#define J9_ABSTRACT_THREAD_FIELDS_2 \
    struct J9ThreadTracing* tracing;
#else /* J9VM_THR_JLM_HOLD_TIMES */
#define J9_ABSTRACT_THREAD_FIELDS_2
#endif /* J9VM_THR_JLM_HOLD_TIMES */

#define J9_ABSTRACT_THREAD_FIELDS_3 \
    uintptr_t waitNumber; \
    uintptr_t lockedmonitorcount; \
    j9thread_os_errno_t os_errno;

#define J9_ABSTRACT_THREAD_FIELDS \
	J9_ABSTRACT_THREAD_FIELDS_1 \
	J9_ABSTRACT_THREAD_FIELDS_2 \
	J9_ABSTRACT_THREAD_FIELDS_3

typedef struct J9ThreadMonitorTracing {
    char* monitor_name;
    uintptr_t enter_count;
    uintptr_t slow_count;
    uintptr_t recursive_count;
    uintptr_t spin2_count;
    uintptr_t yield_count;
#if defined(J9VM_THR_JLM_HOLD_TIMES)
    uint64_t enter_time;
    uint64_t holdtime_sum;
    uint64_t holdtime_avg;
    uint64_t spin_time;
	uintptr_t volatile holdtime_count;
    uintptr_t enter_pause_count;
#endif /* J9VM_THR_JLM_HOLD_TIMES */
} J9ThreadMonitorTracing;

#define J9_ABSTRACT_MONITOR_FIELDS_1 \
    uintptr_t count; \
    struct J9Thread * volatile owner; \
    struct J9Thread* waiting; \
    uintptr_t flags; \
    uintptr_t userData;

#if defined(J9VM_THR_JLM)
#define J9_ABSTRACT_MONITOR_FIELDS_2 \
	struct J9ThreadMonitorTracing* tracing;
#else /* J9VM_THR_JLM */
#define J9_ABSTRACT_MONITOR_FIELDS_2
#endif /* J9VM_THR_JLM */

#define J9_ABSTRACT_MONITOR_FIELDS_3 \
    char* name; \
    uintptr_t pinCount;

#if defined(J9VM_THR_THREE_TIER_LOCKING)
#define J9_ABSTRACT_MONITOR_FIELDS_4 \
    uintptr_t spinlockState; \
    uintptr_t lockingWord; \
    uintptr_t spinCount1; \
    uintptr_t spinCount2; \
    uintptr_t spinCount3; \
    struct J9Thread* blocking;
#else /* J9VM_THR_THREE_TIER_LOCKING */
#define J9_ABSTRACT_MONITOR_FIELDS_4
#endif /* J9VM_THR_THREE_TIER_LOCKING */

#if defined(J9VM_THR_ADAPTIVE_SPIN)
#define J9_ABSTRACT_MONITOR_FIELDS_5 \
    uintptr_t sampleCounter;
#else /* J9VM_THR_ADAPTIVE_SPIN */
#define J9_ABSTRACT_MONITOR_FIELDS_5
#endif /* J9VM_THR_ADAPTIVE_SPIN */

#define J9_ABSTRACT_MONITOR_FIELDS_6

#define J9_ABSTRACT_MONITOR_FIELDS \
	J9_ABSTRACT_MONITOR_FIELDS_1 \
	J9_ABSTRACT_MONITOR_FIELDS_2 \
	J9_ABSTRACT_MONITOR_FIELDS_3 \
	J9_ABSTRACT_MONITOR_FIELDS_4 \
	J9_ABSTRACT_MONITOR_FIELDS_5 \
	J9_ABSTRACT_MONITOR_FIELDS_6

typedef struct J9ThreadAbstractMonitor {
	J9_ABSTRACT_MONITOR_FIELDS
} J9ThreadAbstractMonitor;

#define J9THREAD_MONITOR_SYSTEM  0
#define J9THREAD_MONITOR_INFLATED  0x10000
#define J9THREAD_MONITOR_OBJECT  0x60000
#define J9THREAD_MONITOR_MUTEX_UNINITIALIZED  0x80000
#define J9THREAD_MONITOR_SUPPRESS_CONTENDED_EXIT  0x100000
#define J9THREAD_MONITOR_STOP_SAMPLING  0x200000
#define J9THREAD_MONITOR_JLM_TIME_STAMP_INVALIDATOR  0x400000
#define J9THREAD_MONITOR_NAME_COPY  0x800000
#define J9THREAD_MONITOR_DISABLE_SPINNING  0x1000000
#define J9THREAD_MONITOR_ADAPT_HOLDTIMES_ENABLED  0x2000000
#define J9THREAD_MONITOR_IGNORE_ENTER  0x4000000
#define J9THREAD_MONITOR_SLOW_ENTER  0x8000000
#define J9THREAD_MONITOR_TRY_ENTER_SPIN  0x10000000
#define J9THREAD_MONITOR_SPINLOCK_UNOWNED  0
#define J9THREAD_MONITOR_SPINLOCK_OWNED  1
#define J9THREAD_MONITOR_SPINLOCK_EXCEEDED  2

typedef struct J9AbstractThread {
	J9_ABSTRACT_THREAD_FIELDS
} J9AbstractThread;

#define J9THREAD_FLAG_BLOCKED  1
#define J9THREAD_FLAG_WAITING  2
#define J9THREAD_FLAG_INTERRUPTED  4
#define J9THREAD_FLAG_SUSPENDED  8
#define J9THREAD_FLAG_NOTIFIED  0x10
#define J9THREAD_FLAG_DEAD  0x20
#define J9THREAD_FLAG_SLEEPING  0x40
#define J9THREAD_FLAG_DETACHED  0x80
#define J9THREAD_FLAG_PRIORITY_INTERRUPTED  0x100
#define J9THREAD_FLAG_ATTACHED  0x200
#define J9THREAD_FLAG_CANCELED  0x400
#define J9THREAD_FLAG_STARTED  0x800
#define J9THREAD_FLAG_JOINABLE  0x1000
#define J9THREAD_FLAG_INTERRUPTABLE  0x2000
#define J9THREAD_FLAG_JLM_ENABLED  0x4000
#define J9THREAD_FLAG_JLM_TIME_STAMPS_ENABLED  0x8000
#define J9THREAD_FLAG_JLMHST_ENABLED  0x10000
#define J9THREAD_FLAG_JLM_ENABLED_ALL  0x1C000
#define J9THREAD_FLAG_JLM_HAS_BEEN_ENABLED  0x20000
#define J9THREAD_FLAG_PARKED  0x40000
#define J9THREAD_FLAG_UNPARKED  0x80000
#define J9THREAD_FLAG_TIMER_SET  0x100000
#define J9THREAD_FLAG_ABORTED  0x400000
#define J9THREAD_FLAG_ABORTABLE  0x800000

#define J9THREAD_SUCCESS  0
#define J9THREAD_ERR  1 /* Generic thread errors. */

/* return values from j9thread_create() */
#define J9THREAD_ERR_INVALID_PRIORITY  2
#define J9THREAD_ERR_CANT_ALLOCATE_J9THREAD_T  3
#define J9THREAD_ERR_CANT_INIT_CONDITION  4
#define J9THREAD_ERR_CANT_INIT_MUTEX  5
#define J9THREAD_ERR_THREAD_CREATE_FAILED  6
#define J9THREAD_ERR_INVALID_CREATE_ATTR  7
#define J9THREAD_ERR_CANT_ALLOC_CREATE_ATTR  8
#define J9THREAD_ERR_CANT_ALLOC_STACK  9

/* return values from j9thread_attr functions */
#define J9THREAD_ERR_NOMEMORY  10 /* memory allocation failed */
#define J9THREAD_ERR_UNSUPPORTED_ATTR  11 /* unsupported attribute */
#define J9THREAD_ERR_UNSUPPORTED_VALUE  12 /* unsupported attribute value */
#define J9THREAD_ERR_INVALID_ATTR  13 /* invalid attribute structure */
#define J9THREAD_ERR_INVALID_VALUE  14 /* invalid attribute value */

/* return values from j9thread_get_stack_range() */
#define J9THREAD_ERR_INVALID_THREAD  15 /* Invalid thread argument */
#define J9THREAD_ERR_GETATTR_NP  16 /* Error retrieving attribute from thread */
#define J9THREAD_ERR_GETSTACK  17 /* pthread_attr_getstack() failed */
#define J9THREAD_ERR_UNSUPPORTED_PLAT  18 /* unsupported platform */

/* return values from j9thread_get_jvm_cpu_usage_info() */
#define J9THREAD_ERR_USAGE_RETRIEVAL_ERROR			19	/* error retrieving thread usage data. */
#define J9THREAD_ERR_USAGE_RETRIEVAL_UNSUPPORTED	20	/* -XX:-EnableCPUmonitor has been set, usage retrieval unsupported. */
#define J9THREAD_ERR_INVALID_TIMESTAMP				21	/* Invalid timestamp retrieved. */

/* return values from j9thread_get_cpu_time() */
#define J9THREAD_ERR_NO_SUCH_THREAD					22  /* Underlying thread is no longer available. */

/* Bit flag indicating that os_errno is set. This flag must not interfere with the sign bit. */
#define J9THREAD_ERR_OS_ERRNO_SET  0x40000000
#define J9THREAD_INVALID_OS_ERRNO  -1

#define J9THREAD_ILLEGAL_MONITOR_STATE  1
#define J9THREAD_INTERRUPTED  2
#define J9THREAD_TIMED_OUT  3
#define J9THREAD_PRIORITY_INTERRUPTED  5
#define J9THREAD_ALREADY_ATTACHED  6
#define J9THREAD_INVALID_ARGUMENT  7
#define J9THREAD_WOULD_BLOCK  8
#define J9THREAD_INTERRUPTED_MONITOR_ENTER  9

#endif
