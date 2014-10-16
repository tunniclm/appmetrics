/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef thread_api_h
#define thread_api_h

/**
* @file thread_api.h
* @brief Public API for the THREAD module.
*
* This file contains public function prototypes and
* type definitions for the THREAD module.
*
*/

#include "j9thread.h"
#include "j9cfg.h"
#include "j9comp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define J9THREAD_RWMUTEX_OK		 	 0
#define J9THREAD_RWMUTEX_FAIL	 	 1
#define J9THREAD_RWMUTEX_WOULDBLOCK -1

/* Define conversions for units of time used in thrprof.c */
#define SEC_TO_NANO_CONVERSION_CONSTANT		1000 * 1000 * 1000
#define MICRO_TO_NANO_CONVERSION_CONSTANT	1000
#define GET_PROCESS_TIMES_IN_NANO			100

typedef struct j9thread_process_time_t {
	/* For consistency sake, times are stored as I_64's */
	I_64 _systemTime;
	I_64 _userTime;
} j9thread_process_time_t;

typedef struct j9thread_state_t {
	UDATA flags;
	j9thread_monitor_t blocker;
	j9thread_t owner;
	UDATA count;
} j9thread_state_t;

typedef struct j9thread_attr *j9thread_attr_t;
#define J9THREAD_ATTR_DEFAULT ((j9thread_attr_t *)NULL)

typedef enum j9thread_schedpolicy_t {
	J9THREAD_SCHEDPOLICY_INHERIT,
	J9THREAD_SCHEDPOLICY_OTHER,
	J9THREAD_SCHEDPOLICY_RR,
	J9THREAD_SCHEDPOLICY_FIFO,
	/* dummy value marking end of list */
	j9thread_schedpolicy_LastEnum,
	/* ensure 4-byte enum */
	j9thread_schedpolicy_EnsureWideEnum = 0x1000000
} j9thread_schedpolicy_t;

typedef UDATA j9thread_prio_t;

typedef struct j9thread_monitor_walk_state_t {
	struct J9ThreadMonitorPool* pool;
	UDATA monitorIndex;
	BOOLEAN lockTaken;
} j9thread_monitor_walk_state_t;

/* ---------------- j9threadinspect.c ---------------- */
#if defined (J9VM_OUT_OF_PROCESS)
/* redefine thread functions */
#define j9thread_monitor_walk dbg_j9thread_monitor_walk
#define j9thread_monitor_walk_no_locking dbg_j9thread_monitor_walk_no_locking
#define j9thread_tls_get dbg_j9thread_tls_get
#define j9thread_get_priority dbg_j9thread_get_priority
#define j9thread_get_flags dbg_j9thread_get_flags
#define j9thread_get_state dbg_j9thread_get_state
#define j9thread_get_osId dbg_j9thread_get_osId
#define j9thread_monitor_get_name dbg_j9thread_monitor_get_name
#define j9thread_get_stack_range dbg_j9thread_get_stack_range
#define j9thread_monitor_get_tracing dbg_j9thread_monitor_get_tracing
#define getVMThreadRawState dbgGetVMThreadRawState
#endif


/**
* @brief
* @param void
* @return void
*/
IDATA
j9thread_init_library(void);

/**
* @brief
* @param void
* @return void
*/
void
j9thread_shutdown_library(void);

/**
* @brief
* @param thread
* @param blocker
* @return UDATA
*/
UDATA 
j9thread_get_flags(j9thread_t thread, j9thread_monitor_t* blocker);

/**
 * @brief
 * @param thread
 * @param state
 * @return void
 */
void
j9thread_get_state(j9thread_t thread, j9thread_state_t *const state);

/**
* @brief
* @param thread
* @return UDATA
*/
UDATA 
j9thread_get_priority(j9thread_t thread);

/**
* @brief
* @param thread
* @return UDATA
*/
UDATA 
j9thread_get_osId(j9thread_t thread);

/**
* @brief
* @param monitor
* @return char*
*/
char* 
j9thread_monitor_get_name(j9thread_monitor_t monitor);

/**
 * @brief
 * @param thread
 * @param stackStart
 * @param stackEnd
 * @return UDATA
 */
UDATA
j9thread_get_stack_range(j9thread_t thread, void **stackStart, void **stackEnd);


#if (defined(J9VM_THR_JLM)) 
/**
* @brief
* @param monitor
* @return J9ThreadMonitorTracing*
*/
J9ThreadMonitorTracing* 
j9thread_monitor_get_tracing(j9thread_monitor_t monitor);
#endif /* J9VM_THR_JLM */


/**
* @brief
* @param walkState
* @return j9thread_monitor_t
*/
j9thread_monitor_t 
j9thread_monitor_walk(j9thread_monitor_walk_state_t* walkState);

/**
* @brief
* @param walkState
* @return j9thread_monitor_t
*/
j9thread_monitor_t 
j9thread_monitor_walk_no_locking(j9thread_monitor_walk_state_t* walkState);

/**
* @brief
* @param walkState
* @return void
*/
void 
j9thread_monitor_init_walk(j9thread_monitor_walk_state_t* walkState);

/**
* @brief
* @param thread
* @param key
* @return void*
*/
void*
j9thread_tls_get(j9thread_t thread, j9thread_tls_key_t key);

/**
 * @brief
 * @param monitor
 * @return UDATA
 */
UDATA
j9thread_monitor_owned_by_self(j9thread_monitor_t monitor);

/* ---------------- rwmutex.c ---------------- */

/**
* @struct
*/
struct RWMutex;

/**
*@typedef 
*/
typedef struct RWMutex* j9thread_rwmutex_t;

/**
* @brief
* @param mutex
* @return IDATA
*/
IDATA
j9thread_rwmutex_destroy(j9thread_rwmutex_t mutex);


/**
* @brief
* @param mutex
* @return IDATA
*/
IDATA
j9thread_rwmutex_enter_read(j9thread_rwmutex_t mutex);


/**
* @brief
* @param mutex
* @return IDATA
*/
IDATA
j9thread_rwmutex_enter_write(j9thread_rwmutex_t mutex);

/**
 * @brief
 * @param mutex
 * @returns IDATA
 */
IDATA
j9thread_rwmutex_try_enter_write(j9thread_rwmutex_t mutex);

/**
* @brief
* @param mutex
* @return IDATA
*/
IDATA
j9thread_rwmutex_exit_read(j9thread_rwmutex_t mutex);


/**
* @brief
* @param mutex
* @return IDATA
*/
IDATA
j9thread_rwmutex_exit_write(j9thread_rwmutex_t mutex);


/**
* @brief
* @param handle
* @param flags
* @param name
* @return IDATA
*/
IDATA
j9thread_rwmutex_init(j9thread_rwmutex_t* handle, UDATA flags, const char* name);

/**
* @brief
* @param mutex
* @return BOOLEAN
*/
BOOLEAN
j9thread_rwmutex_is_writelocked(j9thread_rwmutex_t mutex);

/* ---------------- j9threadpriority.c ---------------- */

/**
* @brief
* @param nativePriority
* @return UDATA
*/
UDATA
j9thread_map_native_priority(int nativePriority);


/* ---------------- j9thread.c ---------------- */

/**
* @brief
* @param s
* @return IDATA
*/
IDATA
j9sem_destroy(j9sem_t s);


/**
* @brief
* @param sp
* @param initValue
* @return IDATA
*/
IDATA
j9sem_init(j9sem_t* sp, I_32 initValue);


/**
* @brief
* @param s
* @return IDATA
*/
IDATA 
j9sem_post(j9sem_t s);


/**
* @brief
* @param s
* @return IDATA
*/
IDATA 
j9sem_wait(j9sem_t s);

/**
* @brief
* @param handle
* @return void
*/
void 
j9thread_abort(j9thread_t handle);


/**
* @brief
* @param handle
* @param category
* @return IDATA
*/
IDATA 
j9thread_attach(j9thread_t* handle, U_32 category);


/**
* @brief
* @param thread
* @return void
*/
void  
j9thread_cancel(j9thread_t thread);


/**
* @brief
* @param void
* @return UDATA
*/
UDATA
j9thread_clear_interrupted(void);


/**
* @brief
* @param void
* @return UDATA
*/
UDATA
j9thread_clear_priority_interrupted(void);


/**
* @brief
* @param handle
* @param stacksize
* @param priority
* @param suspend
* @param entrypoint
* @param entryarg
* @return IDATA
*/
IDATA
j9thread_create(j9thread_t *handle, UDATA stacksize, UDATA priority, UDATA suspend, j9thread_entrypoint_t entrypoint, void *entryarg, U_32 category);

/**
* @brief
* @param handle
* @param attr
* @param suspend
* @param entrypoint
* @param entryarg
* @return IDATA
*/
IDATA
j9thread_create_ex(j9thread_t *handle, const j9thread_attr_t *attr, UDATA suspend, j9thread_entrypoint_t entrypoint, void *entryarg, U_32 category);

/**
* @brief
* @param void
* @return UDATA
*/
UDATA
j9thread_current_stack_free(void);


/**
* @brief
* @param thread
* @return void
*/
void
j9thread_detach(j9thread_t thread);


#if (defined(J9VM_THR_TRACING)) 
/**
* @brief
* @param thread
* @return void
*/
void 
j9thread_dump_trace(j9thread_t thread);
#endif /* J9VM_THR_TRACING */

/**
* @brief
* @param monitor
* @return void
*/
void OMRNORETURN
j9thread_exit(j9thread_monitor_t monitor);

/**
* @brief
* @param name
* @return UDATA*
*/
UDATA* 
j9thread_global(char* name);


/**
* @brief
* @return j9thread_monitor_t
*/
j9thread_monitor_t
j9thread_global_monitor(void);


/**
* @brief
* @param thread
* @return void
*/
void 
j9thread_interrupt(j9thread_t thread);


/**
* @brief
* @param thread
* @return UDATA
*/
UDATA
j9thread_interrupted(j9thread_t thread);


#if (defined(J9VM_THR_JLM)) 
/**
* @brief
* @param void
* @return J9ThreadMonitorTracing*
*/
J9ThreadMonitorTracing* 
j9thread_jlm_get_gc_lock_tracing(void);
#endif /* J9VM_THR_JLM */


#if (defined(J9VM_THR_JLM)) 
/**
* @brief
* @param flags
* @return IDATA
*/
IDATA 
j9thread_jlm_init(UDATA flags);
#endif /* J9VM_THR_JLM */

#if defined(J9VM_THR_ADAPTIVE_SPIN)
/**
 * @brief initializes jlm for capturing data needed by the adaptive spin options
 * @param thread
 * @param adaptiveFlags flags indicating the adaptive modes that have been enabled
 * @return IDATA
 */
IDATA
jlm_adaptive_spin_init(void);
#endif

/**
* @brief
* @param void
* @return UDATA
*/
UDATA 
j9thread_lib_get_flags(void);

/**
* @brief
* @param flags
* @return UDATA
*/
UDATA
j9thread_lib_set_flags(UDATA flags);

/**
* @brief
* @param flags
* @return UDATA
*/
UDATA
j9thread_lib_clear_flags(UDATA flags);


#define J9THREAD_LIB_CONTROL_TRACE_START "trace_start"
#define J9THREAD_LIB_CONTROL_TRACE_STOP "trace_stop"

#define J9THREAD_LIB_CONTROL_GET_MEM_CATEGORIES "get_mem_categories"

#if defined(LINUX) && defined(J9VM_GC_REALTIME) 
#define J9THREAD_LIB_CONTROL_USE_REALTIME_SCHEDULING "use_realtime_scheduling"
#define J9THREAD_LIB_CONTROL_USE_REALTIME_SCHEDULING_ENABLED ((UDATA) 1)
#define J9THREAD_LIB_CONTROL_USE_REALTIME_SCHEDULING_DISABLED ((UDATA) 0)
#endif /* defined(LINUX) && defined(J9VM_GC_REALTIME) */

/**
* @brief Control the thread library.
* @param key 
* @param value
* @return IDATA 0 on success, -1 on failure.
*/
IDATA
j9thread_lib_control(const char *key, UDATA value);

/**
* @brief
* @param self
* @return void
*/
void 
j9thread_lib_lock(j9thread_t self);


/**
* @brief
* @param self
* @return IDATA
*/
IDATA
j9thread_lib_try_lock(j9thread_t self);


/**
* @brief
* @param self
* @return void
*/
void 
j9thread_lib_unlock(j9thread_t self);

#if !defined(WIN32)
/**
* @brief
* @return IDATA
*/
IDATA
j9thread_lib_post_fork_reset(void);
#endif /* !defined(WIN32) */

/**
* @brief
* @param monitor
* @return IDATA
*/
IDATA 
j9thread_monitor_destroy(j9thread_monitor_t monitor);

/**
* @brief
* @param monitor
* @return IDATA
*/
IDATA
j9thread_monitor_destroy_nolock(j9thread_t self, j9thread_monitor_t monitor);

/**
* @brief
* @param void
* @return void
*/
void
j9thread_monitor_flush_destroyed_monitor_list(j9thread_t self);


#if (defined(J9VM_THR_TRACING)) 
/**
* @brief
* @param void
* @return void
*/
void 
j9thread_monitor_dump_all(void);
#endif /* J9VM_THR_TRACING */


#if (defined(J9VM_THR_TRACING)) 
/**
* @brief
* @param monitor
* @return void
*/
void 
j9thread_monitor_dump_trace(j9thread_monitor_t monitor);
#endif /* J9VM_THR_TRACING */


/**
* @brief
* @param monitor
* @return IDATA
*/
IDATA 
j9thread_monitor_enter(j9thread_monitor_t monitor);

/**
* @brief
* @param monitor
* @param threadId
* @return IDATA
*/
IDATA 
j9thread_monitor_enter_abortable_using_threadId(j9thread_monitor_t monitor, j9thread_t threadId);

/**
* @brief
* @param monitor
* @param threadId
* @return IDATA
*/
IDATA 
j9thread_monitor_enter_using_threadId(j9thread_monitor_t monitor, j9thread_t threadId);


/**
* @brief
* @param monitor
* @return IDATA
*/
IDATA 
j9thread_monitor_exit(j9thread_monitor_t monitor);

/**
* @brief
* @param monitor
* @param threadId
* @return IDATA
*/
IDATA 
j9thread_monitor_exit_using_threadId(j9thread_monitor_t monitor, j9thread_t threadId);


/**
* @brief
* @param handle
* @param flags
* @param name
* @return IDATA
*/
IDATA 
j9thread_monitor_init_with_name(j9thread_monitor_t* handle, UDATA flags, const char* name);


/**
* @brief
* @param self
* @param monitor
* @return void
*/
void 
j9thread_monitor_lock(j9thread_t self, j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @return IDATA
*/
IDATA 
j9thread_monitor_notify(j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @return IDATA
*/
IDATA
j9thread_monitor_notify_all(j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @return UDATA
*/
UDATA 
j9thread_monitor_num_waiting(j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @return IDATA
*/
IDATA 
j9thread_monitor_try_enter(j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @param threadId
* @return IDATA
*/
IDATA 
j9thread_monitor_try_enter_using_threadId(j9thread_monitor_t monitor, j9thread_t threadId);


/**
* @brief
* @param self
* @param monitor
* @return void
*/
void 
j9thread_monitor_unlock(j9thread_t self, j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @return IDATA
*/
IDATA 
j9thread_monitor_wait(j9thread_monitor_t monitor);

/**
* @brief
* @param monitor
* @param millis
* @param nanos
* @return IDATA
*/
IDATA 
j9thread_monitor_wait_abortable(j9thread_monitor_t monitor, I_64 millis, IDATA nanos);

/**
* @brief
* @param monitor
* @param millis
* @param nanos
* @return IDATA
*/
IDATA 
j9thread_monitor_wait_interruptable(j9thread_monitor_t monitor, I_64 millis, IDATA nanos);


/**
* @brief
* @param monitor
* @param millis
* @param nanos
* @return IDATA
*/
IDATA 
j9thread_monitor_wait_timed(j9thread_monitor_t monitor, I_64 millis, IDATA nanos);

/**
* @brief
* @param nanos
* @return IDATA
*/
IDATA 
j9thread_nanosleep(I_64 nanos);


/**
* @brief
* @param void
* @return IDATA
*/
IDATA 
j9thread_nanosleep_supported(void);


/**
* @brief
* @param wakeTime
* @return IDATA
*/
IDATA 
j9thread_nanosleep_to(I_64 wakeTime);


/**
* @brief
* @param millis
* @param nanos
* @return IDATA
*/
IDATA 
j9thread_park(I_64 millis, IDATA nanos);


/**
* @brief
* @param thread
* @return void
*/
void 
j9thread_priority_interrupt(j9thread_t thread);


/**
* @brief
* @param thread
* @return UDATA
*/
UDATA 
j9thread_priority_interrupted(j9thread_t thread);


#if (defined(J9VM_THR_STACK_PROBES)) 
/**
* @brief
* @param void
* @return IDATA
*/
IDATA
j9thread_probe(void);
#endif /* J9VM_THR_STACK_PROBES */


#if (defined(J9VM_THR_TRACING)) 
/**
* @brief
* @param void
* @return void
*/
void 
j9thread_reset_tracing(void);
#endif /* J9VM_THR_TRACING */


/**
* @brief
* @param thread
* @return IDATA
*/
IDATA
j9thread_resume(j9thread_t thread);


/**
* @brief
* @param void
* @return j9thread_t
*/
j9thread_t 
j9thread_self(void);


/**
* @brief
* @param thread
* @param name
* @return IDATA
*/
IDATA
j9thread_set_name(j9thread_t thread, const char *name);


/**
* @brief
* @param thread
* @param priority
* @return IDATA
*/
IDATA 
j9thread_set_priority(j9thread_t thread, UDATA priority);



#if defined(J9VM_GC_REALTIME)
/**
* @brief
* @return IDATA
*/
IDATA
j9thread_set_priority_spread(void);

#endif /* defined(J9VM_GC_REALTIME) */

/**
* @brief
* @param millis
* @return IDATA
*/
IDATA 
j9thread_sleep(I_64 millis);


/**
* @brief
* @param millis
* @param nanos
* @return IDATA
*/
IDATA 
j9thread_sleep_interruptable(I_64 millis, IDATA nanos);


/**
* @brief
* @param void
* @return void
*/
void 
j9thread_suspend(void);


/**
* @brief
* @param handle
* @return IDATA
*/
IDATA 
j9thread_tls_alloc(j9thread_tls_key_t* handle);


/**
* @brief
* @param handle
* @param finalizer
* @return IDATA
*/
IDATA 
j9thread_tls_alloc_with_finalizer(j9thread_tls_key_t* handle, j9thread_tls_finalizer_t finalizer);


/**
* @brief
* @param key
* @return IDATA
*/
IDATA 
j9thread_tls_free(j9thread_tls_key_t key);


/**
* @brief
* @param thread
* @param key
* @param value
* @return IDATA
*/
IDATA 
j9thread_tls_set(j9thread_t thread, j9thread_tls_key_t key, void* value);


/**
* @brief
* @param thread
* @return void
*/
void 
j9thread_unpark(j9thread_t thread);


/**
* @brief
* @param void
* @return void
*/
void 
j9thread_yield(void);


/**
* @brief Yield the processor
* @param sequencialYields, number of yields in a row that have been made
* @return void
*/
void
j9thread_yield_new(UDATA sequentialYields);


/**
 * Returns whether or not realtime thread scheduling is being used
 * @return 1 if realtime scheduling is in use, 0 otherwise
 */
UDATA
j9thread_lib_use_realtime_scheduling(void);


/* ---------------- thrprof.c ---------------- */

/**
* @brief
* @param enable
* @return void
*/
void 
j9thread_enable_stack_usage(UDATA enable);


/**
 * @brief
 * @param thread
 * @return I_64
 */
I_64 
j9thread_get_cpu_time(j9thread_t thread);


/**
 * Return the amount of CPU time used by the entire process in nanoseconds.
 * @return time in nanoseconds
 * @retval -1 not supported on this platform
 * @see j9thread_get_self_cpu_time, j9thread_get_user_time
 */
I_64
j9thread_get_process_cpu_time(void);

/**
 * @brief
 * @param self
 * @return I_64
 */
I_64
j9thread_get_self_cpu_time(j9thread_t self);

/**
* @brief
* @param thread
* @return I_64
*/
I_64 
j9thread_get_user_time(j9thread_t thread);

/**
 * @brief
 * @param self
 * @return I_64
 */
I_64 
j9thread_get_self_user_time(j9thread_t self);

/**
* @brief
* @param Pointer to processTime struct
* @return I_64
*/
I_64
j9thread_get_process_times(j9thread_process_time_t* processTime);

/**
* @brief
* @param thread
* @return UDATA
*/
UDATA 
j9thread_get_handle(j9thread_t thread);

/**
* @brief
* @param thread
* @param policy
* @param *priority
* @return IDATA
*/
IDATA j9thread_get_os_priority(j9thread_t thread, IDATA* policy, IDATA *priority);

/**
* @brief
* @param thread
* @return UDATA
*/
UDATA 
j9thread_get_stack_size(j9thread_t thread);

/**
* @brief
* @param thread
* @return UDATA
*/
UDATA 
j9thread_get_stack_usage(j9thread_t thread);

/* ---------------- j9threadattr.c ---------------- */

/**
 * @brief
 * @param attr
 * @return IDATA
 */
IDATA
j9thread_attr_init(j9thread_attr_t *attr);

/**
 * @brief
 * @param attr
 * @return IDATA
 */
IDATA
j9thread_attr_destroy(j9thread_attr_t *attr);

/**
 * @brief
 * @param attr
 * @param name
 * @return IDATA
 */
IDATA
j9thread_attr_set_name(j9thread_attr_t *attr, const char *name);

/**
 * @brief
 * @param attr
 * @param policy
 * @return IDATA
 */
IDATA
j9thread_attr_set_schedpolicy(j9thread_attr_t *attr, j9thread_schedpolicy_t policy);

/**
 * @brief
 * @param attr
 * @param priority
 * @return IDATA
 */
IDATA
j9thread_attr_set_priority(j9thread_attr_t *attr, j9thread_prio_t priority);

/**
 * @brief
 * @param attr
 * @param stacksize
 * @return IDATA
 */
IDATA
j9thread_attr_set_stacksize(j9thread_attr_t *attr, UDATA stacksize);

/* ---------------- j9threaderror.c ---------------- */
/**
 * @brief
 * @param err
 * @return const char *
 */
const char * 
j9thread_get_errordesc(IDATA err);

/**
 * @brief
 * @return j9thread_os_errno_t
 */
j9thread_os_errno_t 
j9thread_get_os_errno(void);

#if defined(J9ZOS390)
/**
 * @brief
 * @return j9thread_os_errno_t
 */
j9thread_os_errno_t
j9thread_get_os_errno2(void);
#endif /* J9ZOS390 */  


/* -------------- j9threadnuma.c ------------------- */
/* success code for trheadnuma API */
#define J9THREAD_NUMA_OK 					0
/* generic error code */
#define J9THREAD_NUMA_ERR 					-1
/* no CPUs found for the specified node so affinity is undefined */
#define J9THREAD_NUMA_ERR_NO_CPUS_FOR_NODE 	-2
/* in case affinity is not supported (or at least can't be queried) on the given platform */
#define J9THREAD_NUMA_ERR_AFFINITY_NOT_SUPPORTED 	-3
/*
 * when setting NUMA node affinity, include all specified nodes,
 * including those which are not in the default node set.
 */
#define J9THREAD_NUMA_OVERRIDE_DEFAULT_AFFINITY 0x0001

/**
 * @brief
 * @return UDATA
 */
UDATA
j9thread_numa_get_max_node(void);

IDATA
j9thread_numa_set_node_affinity(j9thread_t thread, const UDATA *numaNodes, UDATA nodeCount, U_32 flags);

/**
 * @brief
 * @param thread
 * @param numaNodes
 * @param nodeCount
 * @return IDATA
 */
IDATA
j9thread_numa_get_node_affinity(j9thread_t thread, UDATA *numaNodes, UDATA *nodeCount);

/* -------------- rasthrsup.c ------------------- */
/**
 * @brief
 * @return UDATA
 */
UDATA
j9thread_get_ras_tid(void);

/* -------------- threadhelp.cpp ------------------- */

/**
* @brief Pin a monitor (prevent its deflation)
* @param monitor the monitor to pin
* @param self the current j9thread_t
* @return void
*/
void
j9thread_monitor_pin(j9thread_monitor_t monitor, j9thread_t self);

/**
* @brief Unpin a monitor (allow its deflation)
* @param monitor the monitor to unpin
* @param self the current j9thread_t
* @return void
*/
void
j9thread_monitor_unpin(j9thread_monitor_t monitor, j9thread_t self);

/**
 * @brief Set the category of the thread.
 * @param thread the threadid whose category needs to be set.
 * @param category the category to be set to.
 * @return 0 on success, -1 on failure.
 */
IDATA
j9thread_set_category(j9thread_t thread, UDATA category);

/**
 * @brief Set the category of the thread.
 * @param thread the threadid whose category needs to be set.
 * @return thread category.
 */
UDATA
j9thread_get_category(j9thread_t thread);

/**
 * @brief Return the cpu usage for the various thread categories
 * @param cpuUsage Cpu usage details to be filled in.
 * @return 0 on success and -1 on failure.
 */
IDATA
j9thread_get_jvm_cpu_usage_info(J9ThreadsCpuUsage *cpuUsage);

/* forward struct definition */
struct J9ThreadLibrary;

/* Thread library API accessible via GetEnv() */
#define J9THREAD_VERSION_1_1 0x7C010001

typedef struct J9ThreadEnv {
	UDATA 		(* get_priority)(j9thread_t thread);
	IDATA 		(* set_priority)(j9thread_t thread, UDATA priority);

	j9thread_t 	(* self)(void);
	UDATA* 		(* global)(char* name);
	IDATA 		(* attach)(j9thread_t* handle, U_32 category);
	IDATA 		(* sleep)(I_64 millis);
	IDATA 		(* create)(j9thread_t *handle, UDATA stacksize, UDATA priority, UDATA suspend, j9thread_entrypoint_t entrypoint, void *entryarg, U_32 category);
	void 		(* exit)(j9thread_monitor_t monitor);
	void 		(* abort)(j9thread_t handle);

	void 		(* priority_interrupt)(j9thread_t thread);
	UDATA		(* clear_interrupted)(void);

	IDATA 		(* monitor_enter)(j9thread_monitor_t monitor);
	IDATA 		(* monitor_exit)(j9thread_monitor_t monitor);
	IDATA 		(* monitor_init_with_name)(j9thread_monitor_t* handle, UDATA flags, const char* name);
	IDATA 		(* monitor_destroy)(j9thread_monitor_t monitor);
	IDATA 		(* monitor_wait)(j9thread_monitor_t monitor);
	IDATA 		(* monitor_notify_all)(j9thread_monitor_t monitor);

	void* 		(* tls_get)(j9thread_t thread, j9thread_tls_key_t key);
	IDATA 		(* tls_set)(j9thread_t thread, j9thread_tls_key_t key, void* value);
	IDATA 		(* tls_alloc)(j9thread_tls_key_t* handle);
	IDATA 		(* tls_free)(j9thread_tls_key_t handle);

} J9ThreadEnv;

#ifdef __cplusplus
}
#endif

#endif /* thread_api_h */

