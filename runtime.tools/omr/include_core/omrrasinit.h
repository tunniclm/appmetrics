/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef omrrasinit_h
#define omrrasinit_h

#include "omr.h"
#include "rastrace_external.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct OMR_Agent OMR_Agent;

/**
 * @brief Initialize port library memory categories.
 * 
 * This should be done before any significant memory allocations are made.
 * This function cannot be called more than once in the lifetime of the port library.
 * Memory category definitions cannot be changed after they are initialized.
 *
 * @param[in] portLibrary The port library.
 *
 * @return an OMR error code
 */
omr_error_t omr_ras_initMemCategories(J9PortLibrary *portLibrary);

omr_error_t omr_ras_initHealthCenter(OMR_VMThread *omrVMThread, OMR_Agent **hc, const char *healthCenterOpt);
omr_error_t omr_ras_cleanupHealthCenter(OMR_VMThread *omrVMThread, OMR_Agent **hc);

/**
 * @internal
 * @brief Initialize data structures that support the OMR_TI API.
 *
 * This function is called by omr_ras_initHealthCenter().
 * It is public for the benefit of the FVT test framework.
 *
 * @param[in] vm the current OMR VM
 * @return OMR error code
 */
omr_error_t omr_ras_initTI(OMR_VM *vm);

/**
 * @internal
 * @brief Cleanup data structures that support the OMR_TI API.
 *
 * This function is called by omr_ras_cleanupHealthCenter().
 * It is public for the benefit of the FVT test framework.
 *
 * @param[in] vm the current OMR VM
 * @return an OMR error code. There is no error if TI was uninitialized.
 */
omr_error_t omr_ras_cleanupTI(OMR_VM *vm);

omr_error_t omr_ras_initTraceEngine(OMR_VMThread *omrVMThread, OMRTraceEngine **trcEngine, const char *traceOptString, const char *datDir);
omr_error_t omr_ras_cleanupTraceEngine(OMR_VMThread *currentThread, OMRTraceEngine **trcEngine);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* omrrasinit_h */
