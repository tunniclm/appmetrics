/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#if !defined(OMR_VM_HPP_)
#define OMR_VM_HPP_

#include "omr.h"
#include "OMR_BaseNonVirtual.hpp"
#include "OMR_VMConfiguration.hpp"

extern "C" {

/**
 * Internal: Attach a vmthread to the VM.
 *
 * @param[in] vm the VM
 * @param[in] vmthread the vmthread to attach
 *
 * @return an OMR error code
 */
omr_error_t attachThread(OMR_VM *vm, OMR_VMThread *vmthread);

/**
 * Internal: Detach a vmthread from the VM.
 *
 * @param[in] vm the VM
 * @param[in] vmthread the vmthread to detach
 *
 * @return an OMR error code
 */
omr_error_t detachThread(OMR_VM *vm, OMR_VMThread *vmthread);

}

#endif /* OMR_VM_HPP_ */
