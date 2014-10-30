/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef omrprofiler_h
#define omrprofiler_h

#include "omr.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OMR_METHOD_DICTIONARY_INVALID_LINE_NUMBER ((U_32)0)

#if defined(_MSC_VER)
#pragma warning(disable : 4200)
#endif /* defined(_MSC_VER) */
typedef struct OMR_MethodDictionaryEntry {
	const void *key;
	const char *propertyValues[];
} OMR_MethodDictionaryEntry;

/* ---------------- OMR_MethodDictionary.cpp ---------------- */

/**
 * @brief Allocate and initialize the VM's method dictionary.
 *
 * Does nothing if the VM's method dictionary is already allocated.
 *
 * @param[in] vm The OMR VM.
 * @param[in] numProperties Number of properties per method.
 * @param[in] propertyNames Names of method properties.
 * @return An OMR error code.
 * @retval OMR_ERROR_NONE Success.
 * @retval OMR_ERROR_OUT_OF_NATIVE_MEMORY Unable to allocate native memory for the method dictionary.
 * @retval OMR_ERROR_FAILED_TO_ALLOCATE_MONITOR Unable to allocate the method dictionary's lock.
 * @retval OMR_ERROR_FAILED_TO_ATTACH_NATIVE_THREAD Unable to attach to the j9thread library.
 */
omr_error_t omr_ras_initMethodDictionary(OMR_VM *vm, size_t numProperties, const char **propertyNames);

/**
 * @brief Deallocate the VM's method dictionary.
 *
 * @param[in] vm The OMR VM.
 */
void omr_ras_cleanupMethodDictionary(OMR_VM *vm);

/**
 * @brief Insert a method into the method dictionary.
 *
 * If the method's key is already in the dictionary, then the old entry is overwritten by the new entry.
 *
 * There is no error if the method dictionary is not enabled.
 *
 * @param[in] vm The OMR VM.
 * @param[in] entry The method entry to insert.
 * @return An OMR error code.
 * @retval OMR_ERROR NONE Success.
 * @retval OMR_ERROR_OUT_OF_NATIVE_MEMORY Unable to allocate native memory for the new entry.
 * @retval OMR_ERROR_FAILED_TO_ATTACH_NATIVE_THREAD Unable to attach to the j9thread library,
 * which is needed to lock the method dictionary.
 * @retval OMR_ERROR_INTERNAL An unexpected internal error occurred.
 */
omr_error_t omr_ras_insertMethodDictionary(OMR_VM *vm, OMR_MethodDictionaryEntry *entry);

/* ---------------- OMR_Profiler.cpp ---------------- */
/**
 * @brief Trace the current thread's top-most stack frame.
 *
 * Wrapper for tracepoint macro.
 * This allows omrglue code to be compiled without access to tracegen-generated
 * header files.
 *
 * @pre The current thread must be attached to the OMR VM.
 *
 * @param[in] omrVMThread The current OMR VM thread. Must not be NULL.
 * @param[in] methodKey The stack frame's method's key. It corresponds to the key of
 *                      the method's entry in the method dictionary.
 */
void omr_ras_sampleStackTraceStart(OMR_VMThread *omrVMThread, const void *methodKey);

/**
 * @brief Trace a frame of the current thread's stack, other than the top-most frame.
 *
 * Wrapper for tracepoint macro.
 * This allows omrglue code to be compiled without access to tracegen-generated
 * header files.
 *
 * @pre The current thread must be attached to the OMR VM.
 *
 * @param[in] omrVMThread The current OMR VM thread. Must not be NULL.
 * @param[in] methodKey The stack frame's method's key. It corresponds to the key of
 *                      the method's entry in the method dictionary.
 */
void omr_ras_sampleStackTraceContinue(OMR_VMThread *omrVMThread, const void *methodKey);

/**
 * @brief Test whether stack sampling is enabled.
 *
 * Wrapper for tracepoint macro.
 * This allows omrglue code to be compiled without access to tracegen-generated
 * header files.
 *
 * Stack stampling is enabled if either of the sampling tracepoints is enabled,
 * regardless of whether the method dictionary is enabled.
 *
 * @return TRUE if stack sampling is enabled. FALSE if stack sampling is disabled.
 */
BOOLEAN omr_ras_sampleStackEnabled(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* omrprofiler_h */