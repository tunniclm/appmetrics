/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#if !defined(OMR_RUNTIME_HPP_)
#define OMR_RUNTIME_HPP_

#include "omr.h"
#include "OMR_BaseNonVirtual.hpp"
#include "OMR_RuntimeConfiguration.hpp"

extern "C" {

/**
 * Internal: Attach a VM to the runtime.
 *
 * @param[in] *runtime the runtime
 * @param[in] *vm the VM to attach
 *
 * @return an OMR error code
 */
omr_error_t attachVM(OMR_Runtime *runtime, OMR_VM *vm);

/**
 * Internal: Detach a VM from the runtime.
 *
 * @param[in] *runtime the runtime
 * @param[in] *vm the VM to detach
 *
 * @return an OMR error code
 */
omr_error_t detachVM(OMR_Runtime *runtime, OMR_VM *vm);

}

#endif /* OMR_RUNTIME_HPP_ */
