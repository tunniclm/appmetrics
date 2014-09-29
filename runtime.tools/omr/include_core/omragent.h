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

typedef struct OMR_TI_MemoryCategory OMR_TI_MemoryCategory;

typedef struct OMR_TI {
	I_32 version;
	void *internalData; /* AGENTS MUST NOT ACCESS THIS FIELD */

	/**
	 * Bind the current thread to an OMR VM.
	 * A thread can bind itself multiple times.
	 * Binds must be paired with an equal number of Unbinds.
	 *
	 * @param[in] vm the OMR vm
	 * @param[in] threadName The thread name. May be NULL. The caller is responsible for managing this memory.
	 * @param[out] vmThread the current OMR VMThread
	 * @return an OMR error code
	 *	 OMR_ERROR_NONE - BindCurrentThread complete
	 *	 OMR_ERROR_ILLEGAL_ARGUMENT - if vm is NULL
	 *	 OMR_ERROR_ILLEGAL_ARGUMENT - if vmThread is NULL
	 */
	omr_error_t (*BindCurrentThread)(OMR_VM *vm, const char *threadName, OMR_VMThread **vmThread);

	/**
	 * Unbind the current thread from its OMR VM.
	 * Unbinds must be paired with an equal number of binds.
	 * When the bind count of a thread reaches 0, the OMR VMThread
	 * is freed, and can no longer be used.
	 *
	 * @param[in,out] vmThread the current OMR VMThread
	 * @return an OMR error code
	 */
	omr_error_t (*UnbindCurrentThread)(OMR_VMThread *vmThread);

	/**
	 * Create a trace record subscriber. The returned subscriptionID uniquely identifies
	 * the new subscriber. The record subscriber must be shut down with a call to
	 * DeregisterRecordSubscriber using the same subscriptionID.
	 * DeregisterRecordSubscriber will cause the subscription thread to call the registered alarmFunc.
	 * 
	 * @param[in] vmThread the current VM thread
	 * @param[in] description description of the subscriber
	 * @param[in] subscriber Trace subscriber callback function. This function will be passed records containing trace data
	 * 			  as they are processed.
	 * @param[in] alarmFunc Alarm callback function that's executed if the subscriber shuts down, returns an error or misbehaves.
	 * @param[in] userData Data passed to subscribed and alarm callbacks.
	 * @param[out] subscriptionID subscription info
	 * @return an OMR error code
	 */
	omr_error_t	(*RegisterRecordSubscriber)(OMR_VMThread *vmThread, char const *description,
		utsSubscriberCallback subscriberFunc, utsSubscriberAlarmCallback alarmFunc, void *userData,	UtSubscription **subscriptionID);

	/**
	 * Shuts down the registered trace record subscriber.
	 * Removes the specified subscriber callback, preventing it from being passed any more data.
	 * This function may block indefinitely if the agent registers a subscription callback that does not terminate.
	 * A subscriptionID that was returned from a RegisterRecordSubscriber call must be used.
	 * The subscriber identified by the subscriptionID should not be deregistered more than once.
	 * DeregisterRecordSubscriber will cause the subscription thread to call the registered alarmFunc.
	 * @param[in] vmThread the current VM thread
	 * @param[in] subscriberID the subscription to deregister
	 * @return an OMR error code
	 *	 OMR_ERROR_NONE - deregistration complete
	 *	 OMR_THREAD_NOT_ATTACHED - no current VM thread
	 *	 OMR_ERROR_NOT_AVAILABLE - deregistration is unsuccessful
	 *	 OMR_ERROR_ILLEGAL_ARGUMENT - unknown subscriber
	 */
	omr_error_t (*DeregisterRecordSubscriber)(OMR_VMThread *vmThread, UtSubscription *subscriptionID);

	/**
	 * This function supplies the trace metadata for use with the trace formatter.
	 * @param[in] vmThread the current VM thread
	 * @param[out] data metadata in a form usable by the trace formatter
	 * @param[out] length the length of the metadata returned
	 * @return an OMR error code
	 *   OMR_ERROR_NONE - success
	 *   OMR_THREAD_NOT_ATTACHED - no current VM thread
	 *   OMR_ERROR_NOT_AVAILABLE - No trace engine
	 *   OMR_ERROR_INTERNAL - other error
	 */
	omr_error_t (*GetTraceMetadata)(OMR_VMThread *vmThread, void **data, I_32 *length);

	/**
	 * Set trace options at runtime.
	 * @param[in] vmThread the current VM thread
	 * @param[in] opts NULL-terminated array of option strings. The options must be provided in name / value pairs.
	 * 					If a name has no value, a NULL array entry must be provided.
	 * 					e.g. { "print", "none", NULL } is valid
	 * 					e.g. { "print", NULL } is invalid
	 * @return an OMR error code
	 */
	omr_error_t (*SetTraceOptions)(OMR_VMThread *vmThread, char const *opts[]);

	/**
	 * This function gets the system CPU load
	 * @param[in] vmThread the current OMR VM thread
	 * @param[out] systemCpuLoad the system CPU load between the observed time period, it cannot be NULL
	 * @return an OMR error code, 	return OMR_ERROR_NONE on successful call.
	 * 								return OMR_ERROR_NOT_AVAILABLE if not supported or insufficient user privilege or vm->sysInfo is not initialized.
	 * 								return OMR_ERROR_RETRY on the first call or if the interval between two calls is too small.
	 * 								return OMR_ERROR_INTERNAL if the calculated CPU load is invalid.
	 * 								return OMR_ERROR_ILLEGAL_ARGUMENT if systemCpuLoad is NULL.
	 * 								return OMR_THREAD_NOT_ATTACHED if vmThread is NULL.
	 */
	omr_error_t (*GetSystemCpuLoad)(OMR_VMThread *vmThread, double *systemCpuLoad);

	/**
	 * This function gets the process CPU load
	 * @param[in] vmThread the current OMR VM thread
	 * @param[out] processCpuLoad the process CPU load between the observed time period, it cannot be NULL.
	 * @return an OMR error code, 	return OMR_ERROR_NONE on successful call.
	 * 								return OMR_ERROR_NOT_AVAILABLE if not supported or insufficient user privilege or vm->sysInfo is not initialized.
	 * 								return OMR_ERROR_RETRY on the first call or if the interval between two calls is too small.
	 * 							  	return OMR_ERROR_INTERNAL if the calculated CPU load is invalid.
	 * 							  	return OMR_ERROR_ILLEGAL_ARGUMENT if processCpuLoad is NULL.
	 * 							  	return OMR_THREAD_NOT_ATTACHED if vmThread is NULL.
	 */
	omr_error_t (*GetProcessCpuLoad)(OMR_VMThread *vmThread, double *processCpuLoad);

	/**
	  * Samples the values of the JVM memory categories and writes them into a buffer allocated by the user.
	  *
	  * @param[in] vmThread the current OMR VM thread
	  * @param[in] max_categories Maximum number of categories to read into category_buffer
	  * @param[out] categories_buffer Block of memory to write result into. 0th entry is the first root category. All other nodes can be walked from the root.
	  * @param[out] written_count_ptr If not NULL, the number of categories written to the buffer is written to this address
	  * @param[out] total_categories_ptr If not NULL, the total number of categories available is written to this address
	  *
	  * @return omr_error_t error code:
	  * OMR_ERROR_NONE - success
	  * OMR_ERROR_ILLEGAL_ARGUMENT - Illegal argument (categories_buffer, count_ptr & total_categories_ptr are all NULL)
	  * OMR_ERROR_OUT_OF_MEMORY - Memory category data was truncated becasue category_buffer/max_categories was not large enough
	  * OMR_ERROR_INTERNAL - GetMemoryCategories was unable to allocate memory for it's own use.
	  * OMR_THREAD_NOT_ATTACHED - The vmThread parameter was null
	  *
	  */
	omr_error_t (*GetMemoryCategories)(OMR_VMThread *vmThread, I_32 max_categories, OMR_TI_MemoryCategory *categories_buffer,
			I_32 *written_count_ptr, I_32 *total_categories_ptr);

	/**
	 * Adds all trace buffers containing data to the write queue, then prompts processing of the write
	 * queue via the standard mechanism.
	 *
	 * @param[in] vmThread the current OMR VM thread
	 * @return Return values with meaning specific to this function:
	 * @retval OMR_ERROR_NONE - success
	 * @retval OMR_ERROR_ILLEGAL_ARGUMENT - If another flush operation was in progress or no record subscribers are registered
	 * @retval OMR_THREAD_NOT_ATTACHED - The vmThread parameter was null
	 * @retval OMR_ERROR_NOT_AVAILABLE - No trace engine
	 */
	omr_error_t (*FlushTraceData)(OMR_VMThread *vmThread);

	/**
	 * This function gets the free physical memory size on the system in bytes
	 * @param[in] vmThread the current OMR VM thread
	 * @param[out] freePhysicalMemorySize the amount of available memory size in bytes
	 * @return an OMR error code
	 */
	omr_error_t (*GetFreePhysicalMemorySize)(OMR_VMThread *vmThread, U_64 *freePhysicalMemorySize);

	/**
	 * This function gets the process virtual memory size in bytes
	 * @param[in] vmThread the current OMR VM thread
	 * @param[out] processVirtualMemorySize the amount of process virtual memory in bytes
	 * @return an OMR error code
	 */
	omr_error_t (*GetProcessVirtualMemorySize)(OMR_VMThread *vmThread, U_64 *processVirtualMemorySize);

	/**
	 * This function gets the process private memory size in bytes
	 * @param[in] vmThread the current OMR VM thread
	 * @param[out] processPrivateMemorySize the amount of process private memory in bytes
	 * @return an OMR error code
	 */
	omr_error_t (*GetProcessPrivateMemorySize)(OMR_VMThread *vmThread, U_64 *processPrivateMemorySize);

	/**
	 * This function gets the process physical memory size in bytes
	 * @param[in] vmThread the current OMR VM thread
	 * @param[out] processPhysicalMemorySize the amount of process physical memory in bytes
	 * @return an OMR error code
	 */
	omr_error_t (*GetProcessPhysicalMemorySize)(OMR_VMThread *vmThread, U_64 *processPhysicalMemorySize);
} OMR_TI;

/*
 * Return data for the GetMemoryCategories API
 */
struct OMR_TI_MemoryCategory {
	/* Category name */
	const char * name;

	/* Bytes allocated under this category */
	I_64 liveBytesShallow;

	/* Bytes allocated under this category and all child categories */
	I_64 liveBytesDeep;

	/* Number of allocations under this category */
	I_64 liveAllocationsShallow;

	/* Number of allocations under this category and all child categories */
	I_64 liveAllocationsDeep;

	/* Pointer to the first child category (NULL if this node has no children) */
	struct OMR_TI_MemoryCategory * firstChild;

	/* Pointer to the next sibling category (NULL if this node has no next sibling)*/
	struct OMR_TI_MemoryCategory * nextSibling;

	/* Pointer to the parent category. (NULL if this node is a root) */
	struct OMR_TI_MemoryCategory * parent;
};

/*
 * Required agent entry points:
 */
#if defined(_MSC_VER)
omr_error_t __cdecl OMRAgent_OnLoad(OMR_TI const *ti, OMR_VM *vm, char const *options);
omr_error_t __cdecl OMRAgent_OnUnload(OMR_TI const *ti, OMR_VM *vm);
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
 * The agent must deregister its subscribers before returning from this function.
 *
 * @param[in] ti The OMR tooling interface.
 * @param[in] vm The OMR vm.
 * @return omr_error_t error code:
 *   OMR_ERROR_NONE - Success: it is safe to unload the agent lib
 *   Otherwise, any other return value indicates the VM cannot unload the agent lib
 */
omr_error_t OMRAgent_OnUnload(OMR_TI const *ti, OMR_VM *vm);
#endif

#ifdef __cplusplus
}
#endif

#endif /* OMRAGENT_H_INCLUDED */
