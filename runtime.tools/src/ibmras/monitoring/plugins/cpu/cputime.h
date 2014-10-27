 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_monitoring_plugins_cpu_cputime_h
#define ibmras_monitoring_plugins_cpu_cputime_h

struct CPUTime {
	unsigned long long time; /* ns since fixed point */
	unsigned long long total; /* cumulative total cpu time in ns */
	unsigned long long process; /* cumulative process cpu time in ns */
	int nprocs;
};

extern "C" struct CPUTime* getCPUTime();

#endif /* ibmras_monitoring_plugins_cpu_cputime_h */
