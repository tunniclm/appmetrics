 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/common/Logger.h"
#include "ibmras/monitoring/plugins/os/Windows.h"
#include "ibmras/monitoring/plugins/os/Plugin.h"
#include <windows.h>
#include <pdh.h>


namespace osplugin {

extern ibmras::common::Logger logger;
extern SourceManager* mgr;

const char* QUERY = "\\Processor(0)\\% Processor Time";
WindowsCPUPullSource src;

monitordata* callback() {
	return src.pullCallback();
}

void complete(monitordata* data) {
	/* do something */
}

PullSource* getCPUPullSource() {
	return &src;
}


WindowsCPUPullSource::WindowsCPUPullSource() {
/*	PDH_STATUS result = PdhOpenQuery(NULL, NULL, &handle);
	if (ERROR_SUCCESS != result) {
		logger.warning("Failed to query performance counters");
	    handle = NULL;
	}
*/
}

WindowsCPUPullSource::~WindowsCPUPullSource() {

}

PULL_CALLBACK WindowsCPUPullSource::getCallback() {
	return callback;
}

PULL_CALLBACK_COMPLETE WindowsCPUPullSource::getCallbackComplete() {
	return complete;
}

monitordata* WindowsCPUPullSource::pullCallback() {
	return NULL;
}

WindowsCPUStats::WindowsCPUStats(char* procstat) {
}


} /* end namespace osplugin */
