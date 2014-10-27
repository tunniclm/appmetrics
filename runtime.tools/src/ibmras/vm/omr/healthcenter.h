 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_vm_omr_healthcenter_h
#define ibmras_vm_omr_healthcenter_h

#include <omr.h>
#include <omragent.h>

struct omrRunTimeProviderParameters {
	OMR_VM *theVm;
	OMR_TI const *omrti;
};
#endif // ibmras_vm_omr_healthcenter_h
