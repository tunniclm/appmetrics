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
#include <sstream>

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

static char* NewCString(const std::string& s) {
	char *result = new char[s.length() + 1];
	std::strcpy(result, s.c_str());
	return result;
}

static double CalculateTotalCPU(struct CPUTime* start, struct CPUTime* finish) {
	double cpu = (double)(finish->total - start->total) / (double)(finish->time - start->time);
	if (cpu > 1.0) {
		IBMRAS_DEBUG_1(debug, "Total CPU reported > 1.0 (%lf)", cpu);
		cpu = 1.0;
	}
	return cpu;
}

static double CalculateProcessCPU(struct CPUTime* start, struct CPUTime* finish) {
	double cpu = (double)(finish->process - start->process) / (double)(finish->time - start->time);
	if (cpu > 1.0) {
		IBMRAS_DEBUG_1(debug, "Process CPU reported > 1.0 (%lf)", cpu);
		cpu = 1.0;
	}
	return cpu;
}

static void AppendCPUTime(std::stringstream& contentss) {
	contentss << "startCPU";
	contentss << "@#" << (plugin::current->time / 1000000); // time in ms
	contentss << "@#" << CalculateProcessCPU(plugin::last, plugin::current);
	contentss << "@#" << CalculateTotalCPU(plugin::last, plugin::current);
	contentss << '\n';
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

	if (plugin::last != NULL) {
		delete plugin::last;
	}
	plugin::last = plugin::current;
	plugin::current = getCPUTime();
	
	if (IsValidData(plugin::last) && IsValidData(plugin::current)) {
		std::stringstream contentss;
		contentss << "#CPUSource\n";
		AppendCPUTime(contentss);
		
		std::string content = contentss.str();
		data->size = content.length();
		data->data = NewCString(content);
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
	src->header.description = NewCString(desc);
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
