 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_monitoring_plugins_os_plugin_h
#define ibmras_monitoring_plugins_os_plugin_h

/* declarations for the plugin that will provide OS metrics */

#include "ibmras/common/port/ThreadData.h"
#include "ibmras/monitoring/Monitoring.h"
#include <cstdio>
#include <string>

namespace osplugin {

/* enums for the the different pull sources */
enum PullSourceIDs {
	CPU
};


class PullSource {
public:
	virtual pullsource* getDescriptor() {return NULL;};
	virtual ~PullSource() {};
};

class CPUPullSource : public PullSource {
public:
	virtual PULL_CALLBACK getCallback() = 0;					/* call to get the data */
	virtual PULL_CALLBACK_COMPLETE getCallbackComplete() = 0;	/* indicates that the data can be released / re-used */
	CPUPullSource(){};
	pullsource* getDescriptor();
	~CPUPullSource(){};
};

class SourceManager {
public:
	SourceManager();
	int start();
	int stop();
	pullsource* registerPullSource(uint32 provID);
	static const uint32 DEFAULT_CAPACITY = 1024;
	uint32 getProvID();
private:
	uint32 provid;
	bool running;
	uint32 pullCount;
	PullSource* pullsources;
};

class CPUStat {
public:
	CPUStat(const char* name = NULL, uint32 start = 0, uint32 length = 0) { this->name = name; this->start = start; this->length = length; }
	const char* name;
	uint32 start;
	uint32 length;
};

class CPUStats {
public:
	const char* JSON();
	~CPUStats();
protected:
	static const uint32 STAT_MAX = 8;
	CPUStat stats[CPUStats::STAT_MAX];
    std::string* data;
    std::string* json;
    static uint32 count;
};

}	/* end of namespace osplugin */


#endif /* ibmras_monitoring_plugins_os_plugin_h */
