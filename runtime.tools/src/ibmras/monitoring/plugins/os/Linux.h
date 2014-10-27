 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_plugins_os_linux_h
#define ibmras_monitoring_plugins_os_linux_h

#include "ibmras/monitoring/plugins/os/Plugin.h"

namespace osplugin {

class LinuxCPUPullSource : public CPUPullSource {
public:
	LinuxCPUPullSource();
	PULL_CALLBACK getCallback();
	PULL_CALLBACK_COMPLETE getCallbackComplete();
	~LinuxCPUPullSource();
	monitordata* pullCallback();
private:
	static FILE* fp;				/* file pointer */
};

class LinuxCPUStats : public CPUStats {
public:
	LinuxCPUStats(char* procstat);
};

} /* end namespace osplugin */


#endif /* ibmras_monitoring_plugins_os_linux_h */
