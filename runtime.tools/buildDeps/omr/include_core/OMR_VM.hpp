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
 * @param[in] vm The VM
 * @param[in] vmthread The vmthread to attach. NOTE: Need NOT be the current thread!
 *                     vmthread->_os_thread must be initialized.
 *
 * @return an OMR error code
 */
omr_error_t attachThread(OMR_VM *vm, OMR_VMThread *vmthread);

/**
 * Internal: Detach a vmthread from the VM.
 *
 * @param[in] vm The VM
 * @param[in] vmthread The vmthread to detach. NOTE: Need NOT be the current thread!
 *
 * @return an OMR error code
 */
omr_error_t detachThread(OMR_VM *vm, OMR_VMThread *vmthread);

}

#endif /* OMR_VM_HPP_ */
