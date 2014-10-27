 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_plugins_os_windows_h
#define ibmras_monitoring_plugins_os_windows_h

#include "ibmras/monitoring/plugins/os/Plugin.h"
#include <pdh.h>

namespace osplugin {

class WindowsCPUPullSource : public CPUPullSource {
public:
	WindowsCPUPullSource();
	PULL_CALLBACK getCallback();
	PULL_CALLBACK_COMPLETE getCallbackComplete();
	~WindowsCPUPullSource();
	monitordata* pullCallback();
private:
	HQUERY handle;
};

class WindowsCPUStats : public CPUStats {
public:
	WindowsCPUStats(char* procstat);
};

} /* end namespace osplugin */


#endif /* ibmras_monitoring_plugins_os_windows_h */
