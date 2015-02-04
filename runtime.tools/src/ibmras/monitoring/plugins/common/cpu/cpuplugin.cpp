 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "ibmras/monitoring/AgentExtensions.h"
#include "ibmras/monitoring/plugins/common/cpu/cputime.h"
#include <cstring>
#include <string>
#include <sstream>

#if defined(_WINDOWS)
#define CPUPLUGIN_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define CPUPLUGIN_DECL
#endif

#define CPUSOURCE_PULL_INTERVAL 2
#define DEFAULT_CAPACITY 1024*10

namespace plugin {
	uint32 provid = 0;
	struct CPUTime* last;
	struct CPUTime* current;
	bool noFailures = true;
}

namespace cpuplugin {
	agentCoreFunctions aCF;
}

using namespace ibmras::common::logging;

static char* NewCString(const std::string& s) {
	char *result = new char[s.length() + 1];
	std::strcpy(result, s.c_str());
	return result;
}

static double clamp(double value, double min, double max) {
	if (value > max) return max;
	if (value < min) return min;
	return value;
}

static double CalculateTotalCPU(struct CPUTime* start, struct CPUTime* finish) {
	double cpu = (double)(finish->total - start->total) / (double)(finish->time - start->time);
	if (cpu < 0.0 || cpu > 1.0) {
		std::stringstream cpuss;
		cpuss <<  "[cpu] Total CPU reported is out of range 0.0 to 1.0 ("<<cpu<<")";
		cpuplugin::aCF.logMessage(debug, cpuss.str().c_str());
	}
	cpu = clamp(cpu, 0.0, 1.0);
	return cpu;
}

static double CalculateProcessCPU(struct CPUTime* start, struct CPUTime* finish) {
	double cpu = (double)(finish->process - start->process) / (double)(finish->time - start->time);
	if (cpu < 0.0 || cpu > 1.0) {
		std::stringstream cpuss;
		cpuss <<  "[cpu] Process CPU reported is out of range 0.0 to 1.0 ("<<cpu<<")";
		cpuplugin::aCF.logMessage(debug, cpuss.str().c_str());
	}
	cpu = clamp(cpu, 0.0, 1.0);
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

static bool TimesAreDifferent(struct CPUTime* start, struct CPUTime* finish) {
	return finish->time != start->time;
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
	
	if (IsValidData(plugin::last) && IsValidData(plugin::current)
		&& TimesAreDifferent(plugin::last, plugin::current)) {
		
		std::stringstream contentss;
		contentss << "#CPUSource\n";
		AppendCPUTime(contentss);
		
		std::string content = contentss.str();
		data->size = static_cast<uint32>(content.length()); // should data->size be a size_t?
		data->data = NewCString(content);
	} else {
		if (!IsValidData(plugin::current)) {
			cpuplugin::aCF.logMessage(debug, "[cpu] Skipped sending data (reason: invalid data)");
			if (plugin::noFailures) {
				cpuplugin::aCF.logMessage(warning, "[cpu] At least one data gathering failure occurred");
				plugin::noFailures = false;
			}
		} else if (IsValidData(plugin::last) && !TimesAreDifferent(plugin::last, plugin::current)) {
			cpuplugin::aCF.logMessage(debug, "[cpu] Skipped sending data (reason: time did not advance)");
			if (plugin::noFailures) {
				cpuplugin::aCF.logMessage(warning, "[cpu] At least one data gathering failure occurred");
				plugin::noFailures = false;
			}
		}
	}
	
	return data;
}

void OnComplete(monitordata* data) {
	if (data != NULL) {
		if (data->data != NULL) {
			delete[] data->data;
		}
		delete data;
	}
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
CPUPLUGIN_DECL pullsource* ibmras_monitoring_registerPullSource(agentCoreFunctions aCF, uint32 provID) {
	cpuplugin::aCF = aCF;
	cpuplugin::aCF.logMessage(debug, "[cpu] Registering pull source");
	pullsource *head = createPullSource(0, "cpu_os");
	plugin::provid = provID;
	return head;
}

CPUPLUGIN_DECL int ibmras_monitoring_plugin_init(const char* properties) {
	// NOTE(tunniclm): We don't have the agentCoreFunctions yet, so we can't do any init that requires
	//                 calling into the API (eg getting properties.)	
	return 0;
}

CPUPLUGIN_DECL int ibmras_monitoring_plugin_start() {
	cpuplugin::aCF.logMessage(fine, "[cpu] Starting");
	plugin::noFailures = true;
	return 0;
}

CPUPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
	cpuplugin::aCF.logMessage(fine, "[cpu] Stopping");
	return 0;
}

CPUPLUGIN_DECL const char* ibmras_monitoring_getVersion() {
	return "1.0";
}
}
