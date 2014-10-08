/*
 * plugin.cpp
 *
 *  Created on: 16 Jul 2014
 *      Author: Mike Tunnicliffe
 */

#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/Logger.h"
#include "ibmras/monitoring/plugins/cpu/cputime.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <string>

#if defined(_WINDOWS)
#define CPUPLUGIN_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define CPUPLUGIN_DECL
#endif

#define CPUSOURCE_PULL_INTERVAL 1
#define DEFAULT_CAPACITY 1024*10

IBMRAS_DEFINE_LOGGER("CPUPlugin");

namespace plugin {
	uint32 provid = 0;
	struct CPUTime* last;
	struct CPUTime* current;
}

// Do something akin to C++11 std::to_string()
// use VPRINT macro from Logger for these?
std::string ToStringLLU(unsigned long long value) {
	char buf[32]; // bounds...
	std::sprintf(buf, "%llu", value);
	return std::string(buf);
}

std::string ToStringD(int value) {
	char buf[32]; // bounds...
	std::sprintf(buf, "%d", value);
	return std::string(buf);
}

std::string ToStringU4(unsigned value) {
	char buf[32]; // only 4 digits required
	std::sprintf(buf, "%04u", value);
	return std::string(buf);
}

static unsigned CalculateTotalCPUPercentage(struct CPUTime* start, struct CPUTime* finish) {
	// percentage * 100 (gives 2 decimal places) -- eg 44.55% would be 4455
	return (unsigned)(100ULL * 100ULL * (finish->total - start->total) / (finish->time - start->time));
}

static unsigned CalculateProcessCPUPercentage(struct CPUTime* start, struct CPUTime* finish) {
	// percentage * 100 (gives 2 decimal places) -- eg 44.55% would be 4455
	return (unsigned)(100ULL * 100ULL * (finish->process - start->process) / (finish->time - start->time));
}

static void AppendCPUTime(std::string &content) {
	unsigned total = CalculateTotalCPUPercentage(plugin::last, plugin::current);
	unsigned process = CalculateProcessCPUPercentage(plugin::last, plugin::current);
	content += "startCPU";
	content += "@#" + ToStringLLU(plugin::current->time / 1000000); // time in ms
	content += "@#0." + ToStringU4(process);
	content += "@#0." + ToStringU4(total);
	//content += "@#" + ToStringD(plugin::current->nprocs);
}

static bool IsValidData(struct CPUTime* cputime) {
	return cputime != NULL;
}

monitordata* OnRequestData() {
	monitordata *data = new monitordata;
	data->provID = plugin::provid;
	data->sourceID = 0;
	data->size = 0;
	data->data = NULL;
	data->persistent = false;

	plugin::last = plugin::current;
	plugin::current = getCPUTime();
	
	if (IsValidData(plugin::last) && IsValidData(plugin::current)) {
		std::string content("#CPUSource\n");
		AppendCPUTime(content);
		content += "\n";
		data->size = content.length() + 1;
		data->data = strdup(content.c_str());
	}
	
	return data;
}

void OnComplete(monitordata* data) {
	if (data->data != NULL) {
		delete[] data->data;
	}
	delete data;
}

pullsource* createPullSource(uint32 srcid, const char* name) {
	pullsource *src = new pullsource();
	src->header.name = name;
	std::string desc("Description for ");
	desc.append(name);
	src->header.description = desc.c_str();
	src->header.sourceID = srcid;
	src->next = NULL;
	src->header.capacity = DEFAULT_CAPACITY;
	src->callback = OnRequestData;
	src->complete = OnComplete;
	src->pullInterval = CPUSOURCE_PULL_INTERVAL; // seconds
	return src;
}

extern "C" {
CPUPLUGIN_DECL pullsource* ibmras_monitoring_registerPullSource(uint32 provID) {
	IBMRAS_DEBUG(info,  "Registering pull sources");
	pullsource *head = createPullSource(0, "cpu_os");
	plugin::provid = provID;
	return head;
}

CPUPLUGIN_DECL int ibmras_monitoring_plugin_start() {
	IBMRAS_DEBUG(info,  "Starting");
	return 0;
}

CPUPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
	return 0;
}
}
