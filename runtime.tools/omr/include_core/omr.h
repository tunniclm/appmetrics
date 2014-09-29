/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef omr_h
#define omr_h

#include "j9port.h"

#define PORT_ACCESS_FROM_OMRRUNTIME(omrRuntime) J9PortLibrary *privatePortLibrary = (omrRuntime)->_portLibrary
#define PORT_ACCESS_FROM_OMRVM(omrVM) PORT_ACCESS_FROM_OMRRUNTIME((omrVM)->_runtime)
#define PORT_ACCESS_FROM_OMRVMTHREAD(omrVMThread) PORT_ACCESS_FROM_OMRVM((omrVMThread)->_vm)

#if defined(J9ZOS390)
#include "edcwccwi.h"
/* Convert function pointer to XPLINK calling convention */
#define OMR_COMPATIBLE_FUNCTION_POINTER(fp) ((void*)__bldxfd(fp))
#else /* J9ZOS390 */
#define OMR_COMPATIBLE_FUNCTION_POINTER(fp) ((void*)(fp))
#endif /* J9ZOS390 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	OMR_ERROR_NONE = 0,
	OMR_ERROR_OUT_OF_NATIVE_MEMORY,
	OMR_ERROR_FAILED_TO_ATTACH_NATIVE_THREAD,
	OMR_ERROR_MAXIMUM_VM_COUNT_EXCEEDED,
	OMR_ERROR_MAXIMUM_THREAD_COUNT_EXCEEDED,
	OMR_THREAD_STILL_ATTACHED,
	OMR_VM_STILL_ATTACHED,
	OMR_ERROR_FAILED_TO_ALLOCATE_MONITOR,
	OMR_ERROR_INTERNAL,
	OMR_ERROR_ILLEGAL_ARGUMENT,
	OMR_ERROR_NOT_AVAILABLE,
	OMR_THREAD_NOT_ATTACHED,
	OMR_ERROR_FILE_UNAVAILABLE,
	OMR_ERROR_RETRY
} omr_error_t;

struct OMR_Agent;
struct OMR_RuntimeConfiguration;
struct OMR_Runtime;
struct OMR_SysInfo;
struct OMR_TI;
struct OMRTraceEngine;
struct OMR_VMConfiguration;
struct OMR_VM;
struct OMR_VMThread;
struct UtInterface;
struct UtThreadData;

typedef struct OMR_RuntimeConfiguration {
	UDATA _maximum_vm_count;		/* 0 for unlimited */
} OMR_RuntimeConfiguration;

typedef struct OMR_Runtime {
	UDATA _initialized;
	J9PortLibrary *_portLibrary;
	struct OMR_VM *_vmList;
	j9thread_monitor_t _vmListMutex;
	struct OMR_VM *_rootVM;
	struct OMR_RuntimeConfiguration _configuration;
	UDATA _vmCount;
} OMR_Runtime;

typedef struct OMR_VMConfiguration {
	UDATA _maximum_thread_count;		/* 0 for unlimited */
} OMR_VMConfiguration;

typedef struct movedObjectHashCode {
	U_32 originalHashCode;
	BOOLEAN hasBeenMoved;
	BOOLEAN hasBeenHashed;
} movedObjectHashCode;

typedef struct OMR_VM {
	struct OMR_Runtime *_runtime;
	void *_language_vm;
	struct OMR_VM *_linkNext;
	struct OMR_VM *_linkPrevious;
	struct OMR_VMThread *_vmThreadList;
	j9thread_monitor_t _vmThreadListMutex;
	UDATA _arrayletLeafSize;
	UDATA _arrayletLeafLogSize;
	UDATA _compressedPointersShift;
	void * _gcOmrVMExtensions;
	struct OMR_VMConfiguration _configuration;
	UDATA _languageThreadCount;
	UDATA _internalThreadCount;
	UDATA gcPolicy;
	struct OMR_SysInfo *sysInfo;

#if defined(J9VM_RAS_TDF_TRACE)
	struct UtInterface *utIntf;
	struct OMR_Agent *_hcAgent;
	j9thread_monitor_t _omrTIAccessMutex;
	struct OMRTraceEngine *_trcEngine;
#endif /* J9VM_RAS_TDF_TRACE */
} OMR_VM;

typedef struct OMR_VMThread {
	struct OMR_VM *_vm;
	void * _language_vmthread;
	j9thread_t _os_thread;
	struct OMR_VMThread *_linkNext;
	struct OMR_VMThread *_linkPrevious;
	UDATA _internal;
    void *_gcOmrVMThreadExtensions;
    
    UDATA vmState;
	UDATA exclusiveCount;

	U_8 *threadName;
    BOOLEAN threadNameIsStatic; /**< threadName is managed externally; Don't free it. */
    j9thread_monitor_t threadNameMutex; /**< Hold this mutex to read or modify threadName. */

#if defined(J9VM_RAS_TDF_TRACE)
	struct UtThreadData *uteThread;
#endif /* J9VM_RAS_TDF_TRACE */

	/* todo: dagar these are temporarily duplicated and should be removed from J9VMThread */
	void *lowTenureAddress;
	void *highTenureAddress;

	void *heapBaseForBarrierRange0;
	UDATA heapSizeForBarrierRange0;
	
	void* memorySpace;
#if defined(J9VM_JIT_RUNTIME_INSTRUMENTATION)
	J9RIParameters riParameters;
#endif /* J9VM_JIT_RUNTIME_INSTRUMENTATION */

	struct movedObjectHashCode movedObjectHashCodeCache;

	I_32 _attachCount;
} OMR_VMThread;

/**
 * Perform basic structural initialization of the OMR runtime
 * (allocating monitors, etc).
 *
 * @param[in] *runtime the runtime to initialize
 *
 * @return an OMR error code
 */
omr_error_t omr_initialize_runtime(OMR_Runtime *runtime);

/**
 * Perform final destruction of the OMR runtime.
 * All VMs be detached before calling.
 *
 * @param[in] *runtime the runtime to destroy
 *
 * @return an OMR error code
 */
omr_error_t omr_destroy_runtime(OMR_Runtime *runtime);

/**
 * Attach an OMR VM to the runtime.
 *
 * @param[in] *vm the VM to attach
 *
 * @return an OMR error code
 */
omr_error_t omr_attach_vm_to_runtime(OMR_VM *vm);

/**
 * Detach an OMR VM from the runtime.
 * All language threads must be detached before calling.
 *
 * @param[in] *vm the VM to detach
 *
 * @return an OMR error code
 */
omr_error_t omr_detach_vm_from_runtime(OMR_VM *vm);

/**
 * Attach an OMR VMThread to the VM.
 *
 * @param[in] *vmthread the vmthread to attach
 *
 * @return an OMR error code
 */
omr_error_t omr_attach_vmthread_to_vm(OMR_VMThread *vmthread);

/**
 * Detach a OMR VMThread from the VM.
 *
 * @param[in] *vmthread the vmthread to detach
 *
 * @return an OMR error code
 */
omr_error_t omr_detach_vmthread_from_vm(OMR_VMThread *vmthread);

/**
 * Initialize an OMR VMThread.
 * This should be done before attaching the thread to the OMR VM.
 * The caller thread must be attached to j9thread.
 *
 * @param[in] vmthread a new vmthread
 * @return an OMR error code
 */
omr_error_t omr_vmthread_init(OMR_VMThread *vmthread);

/**
 * Destroy an OMR VMThread. Free associated data structures.
 * This should be done after detaching the thread from the OMR VM.
 * The caller thread must be attached to j9thread.
 *
 * @param[in] vmthread the vmthread to cleanup
 */
void omr_vmthread_destroy(OMR_VMThread *vmthread);

/**
 * Attach the current thread to an OMR VM.
 * A thread can attach itself multiple times.
 * Attaches must be paired with an equal number of detaches.
 *
 * @param[in]  vm the OMR vm
 * @param[out] vmThread the current OMR VMThread
 * @return an OMR error code
 */
omr_error_t omr_vmthread_alloc_and_attach(OMR_VM *vm, OMR_VMThread **vmThread);

/**
 * Detach the current thread from its OMR VM.
 * Detaches must be paired with an equal number of attaches.
 * When the attach count of a thread reaches 0, the OMR VMThread
 * is freed, and can no longer be used.
 *
 * @param[in,out] vmThread the current OMR VMThread
 * @return an OMR error code
 */
omr_error_t omr_vmthread_detach_and_free(OMR_VMThread *vmThread);

/**
 * Get the current OMR_VMThread, if the current thread is attached.
 *
 * @param[in] vm the VM
 * @param[out] vmthread on success, the non-NULL current OMR_VMThread
 * @return an OMR error code.
 *         OMR_ERROR_NONE indicates the current thread is attached.
 *         OMR_THREAD_NOT_ATTACHED indicates the current thread is not attached.
 *         Other error codes indicate that a problem occurred.
 */
omr_error_t omr_vmthread_get_current(OMR_VM *vm, OMR_VMThread **vmthread);

/*
 * C wrappers for OMR_Agent API
 */
/**
 * @see OMR_Agent::createAgent
 */
struct OMR_Agent *omr_agent_create(OMR_VM *vm, char const *arg);

/**
 * @see OMR_Agent::destroyAgent
 */
void omr_agent_destroy(struct OMR_Agent *agent);

/**
 * @see OMR_Agent::openLibrary
 */
omr_error_t omr_agent_openLibrary(struct OMR_Agent *agent);

/**
 * @see OMR_Agent::callOnLoad
 */
omr_error_t omr_agent_callOnLoad(struct OMR_Agent *agent);

/**
 * @see OMR_Agent::callOnUnload
 */
omr_error_t omr_agent_callOnUnload(struct OMR_Agent *agent);

/**
 * Access the TI function table.
 *
 * @return the TI function table
 */
struct OMR_TI const *omr_agent_getTI(void);


/*
 * LANGUAGE VM GLUE
 * The following functions must be implemented by the language VM.
 */
/**
 * Bind the current thread to a language VM.
 * As a side-effect, the current thread will also be bound to the OMR VM.
 * A thread can bind itself multiple times.
 * Binds must be paired with an equal number of Unbinds.
 *
 * @param[in] vm the OMR vm
 * @param[in] threadName An optional name for the thread. May be NULL.
 * 	?? It is the responsibility of the caller to ensure this string remains valid for the lifetime of the thread.
 * @param[out] vmThread the current OMR VMThread
 * @return an OMR error code
 */
omr_error_t OMR_LangVMGlue_BindCurrentThread(OMR_VM *vm, const char *threadName, OMR_VMThread **vmThread);

/**
 * Unbind the current thread from its language VM.
 * As a side-effect, the current thread will also be unbound from the OMR VM.
 * Unbinds must be paired with an equal number of binds.
 * When the bind count of a thread reaches 0, the OMR VMThread
 * is freed, and can no longer be used.
 *
 * @param[in,out] vmThread the current OMR VMThread
 * @return an OMR error code
 */
omr_error_t OMR_LangVMGlue_UnbindCurrentThread(OMR_VMThread *vmThread);

#ifdef __cplusplus
}
#endif

#endif /* omr_h */
