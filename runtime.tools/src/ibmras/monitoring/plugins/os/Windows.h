/*
 * osWindows.h
 *
 *  Created on: 3 Apr 2014
 *      Author: adam
 */

#ifndef OSWindows_H_
#define OSWindows_H_

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


#endif /* OSWindows_H_ */
