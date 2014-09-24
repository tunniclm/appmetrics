/*
 * plugin.cpp
 *
 *  Created on: 31 Jan 2014
 *      Author: adam
 */

#include "ibmras/monitoring/plugins/os/Plugin.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/common.h"
#include <cstring>


namespace osplugin {

SourceManager* mgr = new SourceManager;

extern PullSource* getCPUPullSource();

IBMRAS_DEFINE_LOGGER("OSPlugin");


pullsource* CPUPullSource::getDescriptor() {
	pullsource *src = new pullsource();
	src->header.name = "CPU";
	src->header.description = "CPU usage";
	src->header.sourceID = CPU;
	src->header.capacity = SourceManager::DEFAULT_CAPACITY;
	src->next = NULL;
	src->callback = getCallback();
	src->complete = getCallbackComplete();
	src->pullInterval = 1;
	return src;
}

SourceManager::SourceManager() {
	running = false;
	provid = -1;
	pullCount = 1;
	pullsources = new PullSource[pullCount];
}

pullsource* SourceManager::registerPullSource(uint32 provID) {
	IBMRAS_DEBUG(fine,  "Registering pull sources");
	provid = provID;
	pullsources = getCPUPullSource();
	pullsource* src = pullsources[CPU].getDescriptor();
	return src;
}

int SourceManager::start() {
	/* do nothing, only have pull sources at the moment */
	IBMRAS_DEBUG(info,  "Starting");
	return 0;
}

int SourceManager::stop() {
	IBMRAS_DEBUG(info,  "Stopping");
	for(uint32 i = 0; i < pullCount; i++) {
		PullSource* p = &pullsources[i];
		delete p;
	}
	return 0;
}

uint32 SourceManager::getProvID() {
	return provid;
}

uint32 CPUStats::count = 0;

const char* CPUStats::JSON() {
	CPUStat* stat = stats;
	json = new std::string;
	json->append("{ \n\"count\" : ");
	json->append(ibmras::common::itoa(count++));
	json->append(",\n");
	for(uint32 i = 0, j = STAT_MAX - 1; i < STAT_MAX; stat++, i++, j--) {
		json->append("\"");
		json->append(stat->name);
		json->append("\" : ");
		if(!i) json->append("\"");
		if(stat->length) {
			json->append(data->substr(stat->start, stat->length));
		} else {
			json->append("0");		/* no data available for that stat */
		}
		if(!i) json->append("\"");
		if(j) {
			json->append(",\n");
		} else {
			json->append("\n");
		}
	}
	json->append(" }\n");
	char* result = new char[json->length() + 1];
	*(result + json->length()) = 0;		/* null terminate the string */
	json->copy(result, json->length());
	delete json;
	json = NULL;
	return result;
}

CPUStats::~CPUStats() {
	if(data) {
		delete data;
	}
	if(json) {
		delete json;
	}
}

} /* end osplugin namespace */


extern "C" {

	pullsource* ibmras_monitoring_registerPullSource(uint32 provID) {

		return osplugin::mgr->registerPullSource(provID);
	}

	int ibmras_monitoring_plugin_start() {
		return osplugin::mgr->start();
	}

	int ibmras_monitoring_plugin_stop() {
		return osplugin::mgr->stop();
	}
}
