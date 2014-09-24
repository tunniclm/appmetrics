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
