 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


/*
 * Linux specific OS routines
 */

#include "ibmras/monitoring/plugins/os/Linux.h"
#include <cstring>
#include "ibmras/common/logging.h"

namespace osplugin {

extern SourceManager* mgr;
extern ibmras::common::Logger* logger;

LinuxCPUPullSource src;

monitordata* callback() {
	return src.pullCallback();
}

void complete(monitordata* data) {
	delete[] data->data;		/* free the internal buffer */
	delete data;			/* free the data structure */
}

PullSource* getCPUPullSource() {
	return &src;
}

FILE* LinuxCPUPullSource::fp = NULL;

LinuxCPUPullSource::LinuxCPUPullSource() {
	/* do nothing */
}

LinuxCPUPullSource::~LinuxCPUPullSource() {
	if(fp) {
		fclose(fp);
	}
}

PULL_CALLBACK LinuxCPUPullSource::getCallback() {
	return callback;
}

PULL_CALLBACK_COMPLETE LinuxCPUPullSource::getCallbackComplete() {
	return complete;
}

monitordata* LinuxCPUPullSource::pullCallback() {
	monitordata* data = new monitordata;
	data->persistent = false;
	data->provID = mgr->getProvID();
	data->sourceID = CPU;
	data->size = 0;
	data->data = NULL;
	fp = fopen("/proc/stat" , "r");
	if(fp) {
		char* buffer = new char[512];
		if(fgets(buffer, 512, fp)) {
			CPUStats* stats = new LinuxCPUStats(buffer);
			const char* json = stats->JSON();
			delete stats;

			data->size = strlen(json);
			data->data = reinterpret_cast<char*>(reinterpret_cast<long>(json));
			IBMRAS_DEBUG_1(info, "RAW Data : %s", buffer);
			IBMRAS_DEBUG_1(info, "JSON Data : %s", json);
			fseek(fp, 0, SEEK_SET);
		}
		fclose(fp);
		fp = NULL;
		delete[] buffer;
	}
	return data;
}

LinuxCPUStats::LinuxCPUStats(char* procstat) {
	stats[0].name = "cpuid";
	stats[1].name = "user"; /* normal processes executing in user mode */
    stats[2].name = "nice"; /* niced processes executing in user mode */
    stats[3].name = "system"; /* processes executing in kernel mode */
    stats[4].name = "idle";
    stats[5].name = "iowait"; /* waiting for I/O to complete */
    stats[6].name = "irq";	/*  servicing interrupts */
    stats[7].name = "softirq"; 	/* servicing softirqs */
	data = new std::string(procstat);
	uint32 marker = 0;
	uint32 start = 0;
	uint32 length = data->size();
	CPUStat* stat = stats;
	while(stat < stats+CPUStats::STAT_MAX && marker < length) {
		if(*(procstat + marker++) != ' ') {
			continue;
		}
		stat->start = start;
		stat->length = (marker - 1) - stat->start;
		if(marker < length) {
			while((marker < length) && (*(procstat + marker++) <= ' ')); /* skip multiple spaces */
			if(marker < length) {
				start = marker;
				stat++;
			}
		}
	}
	json = NULL;
}



} /* end namespace osplugin */
