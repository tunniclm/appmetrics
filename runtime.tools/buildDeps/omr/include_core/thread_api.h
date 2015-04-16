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
	/* For consistency sake, times are stored as int64_t's */
	int64_t _systemTime;
	int64_t _userTime;
} j9thread_process_time_t;

typedef struct j9thread_state_t {
	uintptr_t flags;
	j9thread_monitor_t blocker;
	j9thread_t owner;
	uintptr_t count;
} j9thread_state_t;

typedef struct j9thread_attr *j9thread_attr_t;
#define J9THREAD_ATTR_DEFAULT ((j9thread_attr_t *)NULL)

typedef enum j9thread_detachstate_t {
	J9THREAD_CREATE_DETACHED,
	J9THREAD_CREATE_JOINABLE,
	/* ensure 4-byte enum */
	j9thread_detachstate_EnsureWideEnum = 0x1000000
} j9thread_detachstate_t;

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

typedef uintptr_t j9thread_prio_t;

typedef struct j9thread_monitor_walk_state_t {
	struct J9ThreadMonitorPool* pool;
	uintptr_t monitorIndex;
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
intptr_t
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
* @return uintptr_t
*/
uintptr_t 
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
* @return uintptr_t
*/
uintptr_t 
j9thread_get_priority(j9thread_t thread);

/**
* @brief
* @param thread
* @return uintptr_t
*/
uintptr_t 
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
 * @return uintptr_t
 */
uintptr_t
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
 * @return uintptr_t
 */
uintptr_t
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
* @return intptr_t
*/
intptr_t
j9thread_rwmutex_destroy(j9thread_rwmutex_t mutex);


/**
* @brief
* @param mutex
* @return intptr_t
*/
intptr_t
j9thread_rwmutex_enter_read(j9thread_rwmutex_t mutex);


/**
* @brief
* @param mutex
* @return intptr_t
*/
intptr_t
j9thread_rwmutex_enter_write(j9thread_rwmutex_t mutex);

/**
 * @brief
 * @param mutex
 * @returns intptr_t
 */
intptr_t
j9thread_rwmutex_try_enter_write(j9thread_rwmutex_t mutex);

/**
* @brief
* @param mutex
* @return intptr_t
*/
intptr_t
j9thread_rwmutex_exit_read(j9thread_rwmutex_t mutex);


/**
* @brief
* @param mutex
* @return intptr_t
*/
intptr_t
j9thread_rwmutex_exit_write(j9thread_rwmutex_t mutex);


/**
* @brief
* @param handle
* @param flags
* @param name
* @return intptr_t
*/
intptr_t
j9thread_rwmutex_init(j9thread_rwmutex_t* handle, uintptr_t flags, const char* name);

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
* @return uintptr_t
*/
uintptr_t
j9thread_map_native_priority(int nativePriority);


/* ---------------- j9thread.c ---------------- */

/**
* @brief
* @param s
* @return intptr_t
*/
intptr_t
j9sem_destroy(j9sem_t s);


/**
* @brief
* @param sp
* @param initValue
* @return intptr_t
*/
intptr_t
j9sem_init(j9sem_t* sp, int32_t initValue);


/**
* @brief
* @param s
* @return intptr_t
*/
intptr_t 
j9sem_post(j9sem_t s);


/**
* @brief
* @param s
* @return intptr_t
*/
intptr_t 
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
* @return intptr_t
*/
intptr_t 
j9thread_attach(j9thread_t* handle, uint32_t category);

/**
 * Wait for a thread to terminate
 *
 * This function follows the conventions of pthread_join().
 *
 * - A thread can't join itself.
 * - A thread can only be joined once.
 * - The target thread must have been created with detachstate=J9THREAD_CREATE_JOINABLE.
 *
 * If you pass in an invalid thread argument, the behaviour is undefined
 * because the j9thread_t is in an unknown state. The function may segv.
 *
 * A joinable thread MUST be joined to free its resources.
 *
 * @param[in] thread Wait for this given thread to terminate.
 * @return a j9thread error code
 * @retval J9THREAD_SUCCESS Success
 * @retval J9THREAD_INVALID_ARGUMENT Target thread is the current thread,
 *         or the target thread could be determined to be non-joinable.
 * @retval J9THREAD_TIMED_OUT The O/S join function returned a timeout status.
 * @retval J9THREAD_ERR The O/S join function returned an error code.
 * @retval J9THREAD_ERR|J9THREAD_ERR_OS_ERRNO_SET The O/S join function returned an error code,
 *         and an os_errno is also available.
 */
intptr_t
j9thread_join(j9thread_t thread);

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
* @return uintptr_t
*/
uintptr_t
j9thread_clear_interrupted(void);


/**
* @brief
* @param void
* @return uintptr_t
*/
uintptr_t
j9thread_clear_priority_interrupted(void);


/**
* @brief
* @param handle
* @param stacksize
* @param priority
* @param suspend
* @param entrypoint
* @param entryarg
* @return intptr_t
*/
intptr_t
j9thread_create(j9thread_t *handle, uintptr_t stacksize, uintptr_t priority, uintptr_t suspend, j9thread_entrypoint_t entrypoint, void *entryarg, uint32_t category);

/**
* @brief
* @param handle
* @param attr
* @param suspend
* @param entrypoint
* @param entryarg
* @return intptr_t
*/
intptr_t
j9thread_create_ex(j9thread_t *handle, const j9thread_attr_t *attr, uintptr_t suspend, j9thread_entrypoint_t entrypoint, void *entryarg, uint32_t category);

/**
* @brief
* @param void
* @return uintptr_t
*/
uintptr_t
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
* @return uintptr_t*
*/
uintptr_t* 
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
* @return uintptr_t
*/
uintptr_t
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
* @return intptr_t
*/
intptr_t 
j9thread_jlm_init(uintptr_t flags);
#endif /* J9VM_THR_JLM */

#if defined(J9VM_THR_ADAPTIVE_SPIN)
/**
 * @brief initializes jlm for capturing data needed by the adaptive spin options
 * @param thread
 * @param adaptiveFlags flags indicating the adaptive modes that have been enabled
 * @return intptr_t
 */
intptr_t
jlm_adaptive_spin_init(void);
#endif

/**
* @brief
* @param void
* @return uintptr_t
*/
uintptr_t 
j9thread_lib_get_flags(void);

/**
* @brief
* @param flags
* @return uintptr_t
*/
uintptr_t
j9thread_lib_set_flags(uintptr_t flags);

/**
* @brief
* @param flags
* @return uintptr_t
*/
uintptr_t
j9thread_lib_clear_flags(uintptr_t flags);


#define J9THREAD_LIB_CONTROL_TRACE_START "trace_start"
#define J9THREAD_LIB_CONTROL_TRACE_STOP "trace_stop"

#define J9THREAD_LIB_CONTROL_GET_MEM_CATEGORIES "get_mem_categories"

#if defined(LINUX) && defined(J9VM_GC_REALTIME) 
#define J9THREAD_LIB_CONTROL_USE_REALTIME_SCHEDULING "use_realtime_scheduling"
#define J9THREAD_LIB_CONTROL_USE_REALTIME_SCHEDULING_ENABLED ((uintptr_t) 1)
#define J9THREAD_LIB_CONTROL_USE_REALTIME_SCHEDULING_DISABLED ((uintptr_t) 0)
#endif /* defined(LINUX) && defined(J9VM_GC_REALTIME) */

/**
* @brief Control the thread library.
* @param key 
* @param value
* @return intptr_t 0 on success, -1 on failure.
*/
intptr_t
j9thread_lib_control(const char *key, uintptr_t value);

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
* @return intptr_t
*/
intptr_t
j9thread_lib_try_lock(j9thread_t self);


/**
* @brief
* @param self
* @return void
*/
void 
j9thread_lib_unlock(j9thread_t self);

#if defined(J9VM_THR_FORK_SUPPORT)
/**
 * Post-fork reset function used to clean up and reset old j9thread and j9thread_monitor structures
 * associated with non-existent threads in the child process. This function is to be called directly
 * before fork and requires an attached thread.
 *
 * Since WIN32 has no fork, this code is currently not included. For WIN32, some of the reset code
 * is incomplete: in postForkResetThreads, the j9thread_t->handle must be reset differently.
 *
 * @note This function must only be called in the child process immediately following a fork().
 */
void
j9thread_lib_postForkChild(void);

/**
 * Perform post-fork unlocking of mutexes that must be held over a fork and reset. This function is
 * to be called directly after a fork, before any other j9thread functions, and requires an attached
 * thread.
 */
void
j9thread_lib_postForkParent(void);

/**
 * Perform pre-fork locking of mutexes that must be held over a fork and reset. This function is
 * to be called directly after a fork, before any other j9thread functions, and requires an attached
 * thread.
 */
void
j9thread_lib_preFork(void);
#endif /* !defined(J9VM_THR_FORK_SUPPORT) */

/**
* @brief
* @param monitor
* @return intptr_t
*/
intptr_t 
j9thread_monitor_destroy(j9thread_monitor_t monitor);

/**
* @brief
* @param monitor
* @return intptr_t
*/
intptr_t
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
* @return intptr_t
*/
intptr_t 
j9thread_monitor_enter(j9thread_monitor_t monitor);

/**
* @brief
* @param monitor
* @param threadId
* @return intptr_t
*/
intptr_t 
j9thread_monitor_enter_abortable_using_threadId(j9thread_monitor_t monitor, j9thread_t threadId);

/**
* @brief
* @param monitor
* @param threadId
* @return intptr_t
*/
intptr_t 
j9thread_monitor_enter_using_threadId(j9thread_monitor_t monitor, j9thread_t threadId);


/**
* @brief
* @param monitor
* @return intptr_t
*/
intptr_t 
j9thread_monitor_exit(j9thread_monitor_t monitor);

/**
* @brief
* @param monitor
* @param threadId
* @return intptr_t
*/
intptr_t 
j9thread_monitor_exit_using_threadId(j9thread_monitor_t monitor, j9thread_t threadId);


/**
* @brief
* @param handle
* @param flags
* @param name
* @return intptr_t
*/
intptr_t 
j9thread_monitor_init_with_name(j9thread_monitor_t* handle, uintptr_t flags, const char* name);


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
* @return intptr_t
*/
intptr_t 
j9thread_monitor_notify(j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @return intptr_t
*/
intptr_t
j9thread_monitor_notify_all(j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @return uintptr_t
*/
uintptr_t 
j9thread_monitor_num_waiting(j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @return intptr_t
*/
intptr_t 
j9thread_monitor_try_enter(j9thread_monitor_t monitor);


/**
* @brief
* @param monitor
* @param threadId
* @return intptr_t
*/
intptr_t 
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
* @return intptr_t
*/
intptr_t 
j9thread_monitor_wait(j9thread_monitor_t monitor);

/**
* @brief
* @param monitor
* @param millis
* @param nanos
* @return intptr_t
*/
intptr_t 
j9thread_monitor_wait_abortable(j9thread_monitor_t monitor, int64_t millis, intptr_t nanos);

/**
* @brief
* @param monitor
* @param millis
* @param nanos
* @return intptr_t
*/
intptr_t 
j9thread_monitor_wait_interruptable(j9thread_monitor_t monitor, int64_t millis, intptr_t nanos);


/**
* @brief
* @param monitor
* @param millis
* @param nanos
* @return intptr_t
*/
intptr_t 
j9thread_monitor_wait_timed(j9thread_monitor_t monitor, int64_t millis, intptr_t nanos);

/**
* @brief
* @param nanos
* @return intptr_t
*/
intptr_t 
j9thread_nanosleep(int64_t nanos);


/**
* @brief
* @param void
* @return intptr_t
*/
intptr_t 
j9thread_nanosleep_supported(void);


/**
* @brief
* @param wakeTime
* @return intptr_t
*/
intptr_t 
j9thread_nanosleep_to(int64_t wakeTime);


/**
* @brief
* @param millis
* @param nanos
* @return intptr_t
*/
intptr_t 
j9thread_park(int64_t millis, intptr_t nanos);


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
* @return uintptr_t
*/
uintptr_t 
j9thread_priority_interrupted(j9thread_t thread);


#if (defined(J9VM_THR_STACK_PROBES)) 
/**
* @brief
* @param void
* @return intptr_t
*/
intptr_t
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
* @return intptr_t
*/
intptr_t
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
* @return intptr_t
*/
intptr_t
j9thread_set_name(j9thread_t thread, const char *name);


/**
* @brief
* @param thread
* @param priority
* @return intptr_t
*/
intptr_t 
j9thread_set_priority(j9thread_t thread, uintptr_t priority);



#if defined(J9VM_GC_REALTIME)
/**
* @brief
* @return intptr_t
*/
intptr_t
j9thread_set_priority_spread(void);

#endif /* defined(J9VM_GC_REALTIME) */

/**
* @brief
* @param millis
* @return intptr_t
*/
intptr_t 
j9thread_sleep(int64_t millis);


/**
* @brief
* @param millis
* @param nanos
* @return intptr_t
*/
intptr_t 
j9thread_sleep_interruptable(int64_t millis, intptr_t nanos);


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
* @return intptr_t
*/
intptr_t 
j9thread_tls_alloc(j9thread_tls_key_t* handle);


/**
* @brief
* @param handle
* @param finalizer
* @return intptr_t
*/
intptr_t 
j9thread_tls_alloc_with_finalizer(j9thread_tls_key_t* handle, j9thread_tls_finalizer_t finalizer);


/**
* @brief
* @param key
* @return intptr_t
*/
intptr_t 
j9thread_tls_free(j9thread_tls_key_t key);


/**
* @brief
* @param thread
* @param key
* @param value
* @return intptr_t
*/
intptr_t 
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
j9thread_yield_new(uintptr_t sequentialYields);


/**
 * Returns whether or not realtime thread scheduling is being used
 * @return 1 if realtime scheduling is in use, 0 otherwise
 */
uintptr_t
j9thread_lib_use_realtime_scheduling(void);

/**
 * @brief Set the category of the thread.
 * @param thread the threadid whose category needs to be set.
 * @param category the category to be set to.
 * @param type type of category creation
 * @return 0 on success, -1 on failure.
 */
intptr_t
j9thread_set_category(j9thread_t thread, uintptr_t category, uintptr_t type);

/**
 * @brief Get the category of the thread.
 * @param thread the threadid whose category is needed.
 * @return thread category.
 */
uintptr_t
j9thread_get_category(j9thread_t thread);

/* ---------------- thrprof.c ---------------- */

/**
* @brief
* @param enable
* @return void
*/
void 
j9thread_enable_stack_usage(uintptr_t enable);


/**
 * @brief
 * @param thread
 * @return int64_t
 */
int64_t 
j9thread_get_cpu_time(j9thread_t thread);


/**
 * @brief
 * @param thread
 * @return int64_t
 */
int64_t
j9thread_get_cpu_time_ex(j9thread_t thread);


/**
 * Return the amount of CPU time used by the entire process in nanoseconds.
 * @return time in nanoseconds
 * @retval -1 not supported on this platform
 * @see j9thread_get_self_cpu_time, j9thread_get_user_time
 */
int64_t
j9thread_get_process_cpu_time(void);

/**
 * @brief
 * @param self
 * @return int64_t
 */
int64_t
j9thread_get_self_cpu_time(j9thread_t self);

/**
* @brief
* @param thread
* @return int64_t
*/
int64_t 
j9thread_get_user_time(j9thread_t thread);

/**
 * @brief
 * @param self
 * @return int64_t
 */
int64_t 
j9thread_get_self_user_time(j9thread_t self);

/**
* @brief
* @param Pointer to processTime struct
* @return int64_t
*/
int64_t
j9thread_get_process_times(j9thread_process_time_t* processTime);

/**
* @brief
* @param thread
* @return uintptr_t
*/
uintptr_t 
j9thread_get_handle(j9thread_t thread);

/**
* @brief
* @param thread
* @param policy
* @param *priority
* @return intptr_t
*/
intptr_t j9thread_get_os_priority(j9thread_t thread, intptr_t* policy, intptr_t *priority);

/**
* @brief
* @param thread
* @return uintptr_t
*/
uintptr_t 
j9thread_get_stack_size(j9thread_t thread);

/**
* @brief
* @param thread
* @return uintptr_t
*/
uintptr_t 
j9thread_get_stack_usage(j9thread_t thread);

/**
 * @brief Return the CPU usage for the various thread categories
 * @param cpuUsage CPU usage details to be filled in.
 * @return 0 on success and -1 on failure.
 */
intptr_t
j9thread_get_jvm_cpu_usage_info(J9ThreadsCpuUsage *cpuUsage);

/**
 * Called by the signal handler in javadump.cpp to release any mutexes
 * held by j9thread_get_jvm_cpu_usage_info if the thread walk fails.
 * Can only be used to be called in a signal handler running in the
 * context of the same thread that locked the mutexes in the first place
 * for the locks to be successfully released.
 */
void
j9thread_get_jvm_cpu_usage_info_error_recovery(void);

/* ---------------- j9threadattr.c ---------------- */

/**
 * @brief
 * @param attr
 * @return intptr_t
 */
intptr_t
j9thread_attr_init(j9thread_attr_t *attr);

/**
 * @brief
 * @param attr
 * @return intptr_t
 */
intptr_t
j9thread_attr_destroy(j9thread_attr_t *attr);

/**
 * @brief
 * @param attr
 * @param name
 * @return intptr_t
 */
intptr_t
j9thread_attr_set_name(j9thread_attr_t *attr, const char *name);

/**
 * @brief
 * @param attr
 * @param policy
 * @return intptr_t
 */
intptr_t
j9thread_attr_set_schedpolicy(j9thread_attr_t *attr, j9thread_schedpolicy_t policy);

/**
 * @brief
 * @param attr
 * @param priority
 * @return intptr_t
 */
intptr_t
j9thread_attr_set_priority(j9thread_attr_t *attr, j9thread_prio_t priority);

/**
 * Set detach state
 *
 * @param[in] attr
 * @param[in] detachstate the desired state (i.e., detached or joinable)
 * @return success or failure
 * @retval J9THREAD_SUCCESS success
 * @retval J9THREAD_ERR_INVALID_ATTR attr is an invalid attribute struct.
 * @retval J9THREAD_ERR_INVALID_VALUE Failed to set the specified detachstate.
 */
intptr_t
j9thread_attr_set_detachstate(j9thread_attr_t *attr, j9thread_detachstate_t detachstate);

/**
 * @brief
 * @param attr
 * @param stacksize
 * @return intptr_t
 */
intptr_t
j9thread_attr_set_stacksize(j9thread_attr_t *attr, uintptr_t stacksize);

/* ---------------- j9threaderror.c ---------------- */
/**
 * @brief
 * @param err
 * @return const char *
 */
const char * 
j9thread_get_errordesc(intptr_t err);

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
 * @return uintptr_t
 */
uintptr_t
j9thread_numa_get_max_node(void);

intptr_t
j9thread_numa_set_node_affinity(j9thread_t thread, const uintptr_t *numaNodes, uintptr_t nodeCount, uint32_t flags);

/**
 * @brief
 * @param thread
 * @param numaNodes
 * @param nodeCount
 * @return intptr_t
 */
intptr_t
j9thread_numa_get_node_affinity(j9thread_t thread, uintptr_t *numaNodes, uintptr_t *nodeCount);

/* -------------- rasthrsup.c ------------------- */
/**
 * @brief
 * @return uintptr_t
 */
uintptr_t
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

/* forward struct definition */
struct J9ThreadLibrary;

/* Thread library API accessible via GetEnv() */
#define J9THREAD_VERSION_1_1 0x7C010001

typedef struct J9ThreadEnv {
	uintptr_t 		(* get_priority)(j9thread_t thread);
	intptr_t 		(* set_priority)(j9thread_t thread, uintptr_t priority);

	j9thread_t 	(* self)(void);
	uintptr_t* 		(* global)(char* name);
	intptr_t 		(* attach)(j9thread_t* handle, uint32_t category);
	intptr_t 		(* sleep)(int64_t millis);
	intptr_t 		(* create)(j9thread_t *handle, uintptr_t stacksize, uintptr_t priority, uintptr_t suspend, j9thread_entrypoint_t entrypoint, void *entryarg, uint32_t category);
	void 		(* exit)(j9thread_monitor_t monitor);
	void 		(* abort)(j9thread_t handle);

	void 		(* priority_interrupt)(j9thread_t thread);
	uintptr_t		(* clear_interrupted)(void);

	intptr_t 		(* monitor_enter)(j9thread_monitor_t monitor);
	intptr_t 		(* monitor_exit)(j9thread_monitor_t monitor);
	intptr_t 		(* monitor_init_with_name)(j9thread_monitor_t* handle, uintptr_t flags, const char* name);
	intptr_t 		(* monitor_destroy)(j9thread_monitor_t monitor);
	intptr_t 		(* monitor_wait)(j9thread_monitor_t monitor);
	intptr_t 		(* monitor_notify_all)(j9thread_monitor_t monitor);

	void* 		(* tls_get)(j9thread_t thread, j9thread_tls_key_t key);
	intptr_t 		(* tls_set)(j9thread_t thread, j9thread_tls_key_t key, void* value);
	intptr_t 		(* tls_alloc)(j9thread_tls_key_t* handle);
	intptr_t 		(* tls_free)(j9thread_tls_key_t handle);

} J9ThreadEnv;

#ifdef __cplusplus
}
#endif

#endif /* thread_api_h */

