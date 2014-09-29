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
#if defined(J9ZOS390)
#define J9THREAD_MAX_TLS_KEYS 127   /* One of the TLS slots is replaced by os_errno2 on ZOS */
#else /* !J9ZOS390 */  
#define J9THREAD_MAX_TLS_KEYS 128
#endif /* !J9ZOS390 */

typedef struct J9ThreadTracing {
#if defined(J9VM_THR_JLM_HOLD_TIMES)
    UDATA pause_count;
#endif /* J9VM_THR_JLM_HOLD_TIMES */
} J9ThreadTracing;

#define J9_ABSTRACT_THREAD_FIELDS_1 \
    struct J9ThreadLibrary* library; \
    UDATA attachcount; \
    UDATA priority; \
    struct J9ThreadMonitor* monitor; \
    struct J9Thread* next; \
    struct J9Thread* prev; \
    void* tls[J9THREAD_MAX_TLS_KEYS]; \
    j9thread_entrypoint_t entrypoint; \
    void* entryarg; \
    UDATA flags; \
    UDATA tid; \
    struct J9Thread* interrupter;

#if defined(J9VM_THR_JLM_HOLD_TIMES)
#define J9_ABSTRACT_THREAD_FIELDS_2 \
    struct J9ThreadTracing* tracing;
#else /* J9VM_THR_JLM_HOLD_TIMES */
#define J9_ABSTRACT_THREAD_FIELDS_2
#endif /* J9VM_THR_JLM_HOLD_TIMES */

#define J9_ABSTRACT_THREAD_FIELDS_3 \
    UDATA waitNumber; \
    UDATA lockedmonitorcount; \
    j9thread_os_errno_t os_errno;

#define J9_ABSTRACT_THREAD_FIELDS \
	J9_ABSTRACT_THREAD_FIELDS_1 \
	J9_ABSTRACT_THREAD_FIELDS_2 \
	J9_ABSTRACT_THREAD_FIELDS_3

typedef struct J9ThreadMonitorTracing {
    char* monitor_name;
    UDATA enter_count;
    UDATA slow_count;
    UDATA recursive_count;
    UDATA spin2_count;
    UDATA yield_count;
#if defined(J9VM_THR_JLM_HOLD_TIMES)
    U_64 enter_time;
    U_64 holdtime_sum;
    U_64 holdtime_avg;
    U_64 spin_time;
	UDATA volatile holdtime_count;
    UDATA enter_pause_count;
#endif /* J9VM_THR_JLM_HOLD_TIMES */
} J9ThreadMonitorTracing;

#define J9_ABSTRACT_MONITOR_FIELDS_1 \
    UDATA count; \
    struct J9Thread * volatile owner; \
    struct J9Thread* waiting; \
    UDATA flags; \
    UDATA userData;

#if defined(J9VM_THR_JLM)
#define J9_ABSTRACT_MONITOR_FIELDS_2 \
	struct J9ThreadMonitorTracing* tracing;
#else /* J9VM_THR_JLM */
#define J9_ABSTRACT_MONITOR_FIELDS_2
#endif /* J9VM_THR_JLM */

#define J9_ABSTRACT_MONITOR_FIELDS_3 \
    char* name; \
    UDATA pinCount;

#if defined(J9VM_THR_THREE_TIER_LOCKING)
#define J9_ABSTRACT_MONITOR_FIELDS_4 \
    UDATA spinlockState; \
    UDATA lockingWord; \
    UDATA spinCount1; \
    UDATA spinCount2; \
    UDATA spinCount3; \
    struct J9Thread* blocking;
#else /* J9VM_THR_THREE_TIER_LOCKING */
#define J9_ABSTRACT_MONITOR_FIELDS_4
#endif /* J9VM_THR_THREE_TIER_LOCKING */

#if defined(J9VM_THR_ADAPTIVE_SPIN)
#define J9_ABSTRACT_MONITOR_FIELDS_5 \
    UDATA sampleCounter;
#else /* J9VM_THR_ADAPTIVE_SPIN */
#define J9_ABSTRACT_MONITOR_FIELDS_5
#endif /* J9VM_THR_ADAPTIVE_SPIN */

#if defined(J9VM_OPT_TENANT)
#define J9_ABSTRACT_MONITOR_FIELDS_6 \
    UDATA userData2;
#else
#define J9_ABSTRACT_MONITOR_FIELDS_6
#endif /* J9VM_OPT_TENANT */

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
#define J9THREAD_ERR  1

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

#define J9THREAD_CATEGORY_APPLICATION_THREAD	0x1
#define J9THREAD_CATEGORY_MONITOR_THREAD		0x2
#define J9THREAD_CATEGORY_SYSTEM_THREAD			0x4
/* GC and JIT are also SYSTEM threads, so they have the SYSTEM bit set */
#define J9THREAD_CATEGORY_SYSTEM_GC_THREAD		(0x8 | J9THREAD_CATEGORY_SYSTEM_THREAD)
#define J9THREAD_CATEGORY_SYSTEM_JIT_THREAD		(0x10 | J9THREAD_CATEGORY_SYSTEM_THREAD)

typedef struct J9ThreadsCpuUsage {
	I_64 timestamp;
	I_64 applicationCpuTime;
	I_64 monitorCpuTime;
	I_64 systemCpuTime;
	I_64 gcCpuTime;
	I_64 jitCpuTime;
} J9ThreadsCpuUsage;

#endif
