 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_plugins_common_cpu_cputime_h
#define ibmras_monitoring_plugins_common_cpu_cputime_h

#include "ibmras/common/types.h"

// TODO Should rewrite this as a class and not expose timestamp
//      use the platform specific time values in an opaque way
//      as converting them to uint64s is problematic
struct CPUTime {
	uint64 time; /* ns since fixed point */
	uint64 total; /* cumulative total cpu time in ns */
	uint64 process; /* cumulative process cpu time in ns */
	uint32 nprocs;
};

extern "C" struct CPUTime* getCPUTime();

#endif /* ibmras_monitoring_plugins_cpu_common_cputime_h */
