/*
 * oslinux.h
 *
 *  Created on: 3 Apr 2014
 *      Author: adam
 */

#ifndef OSLINUX_H_
#define OSLINUX_H_

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


#endif /* OSLINUX_H_ */
