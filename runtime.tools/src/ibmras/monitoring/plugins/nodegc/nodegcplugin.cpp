/*
 * plugin.cpp
 *
 *  Created on: 10 May 2014
 *      Author: Mike Tunnicliffe
 */

#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/util/sysUtils.h"
#include <cstring>
#include <string>
#include <cstdio>
#if defined(_LINUX)
#include <time.h>
#else if defined(_WINDOWS)
#include "windows.h"
#endif
#include "v8.h"

#define DEFAULT_CAPACITY 10240  /* default bucket capacity = 1MB */

#if defined(_WINDOWS)
#define NODEGCPLUGIN_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define NODEGCPLUGIN_DECL
#endif

IBMRAS_DEFINE_LOGGER("NodeGCPlugin");

namespace plugin {
	void (*callback)(monitordata*);
	uint32 provid = 0;
	bool timingOK;
	
#ifdef _WINDOWS
	LARGE_INTEGER gcSteadyStart, gcSteadyEnd;
#endif
#ifdef _LINUX
	struct timespec gcSteadyStart, gcSteadyEnd;
#endif
}

using namespace v8;

/*
 * Linux
 */
#ifdef _LINUX
static bool GetSteadyTime(struct timespec* tv) {
	int rc = clock_gettime(CLOCK_MONOTONIC, tv);
	return rc == 0;
}
static unsigned long CalculateDuration(struct timespec start, struct timespec finish) {
	return (unsigned long)((finish.tv_sec - start.tv_sec) * 1000L + (finish.tv_nsec - start.tv_nsec) / 1000000L);
}
#endif

/*
 * Windows
 */
#ifdef _WINDOWS
static LARGE_INTEGER freq;
static bool freqInitialized = FALSE;
static bool GetSteadyTime(LARGE_INTEGER* pcount) {
	if (!freqInitialized) {
		if (QueryPerformanceFrequency(&freq) == 0) {
			return FALSE;
		}
		freqInitialized = TRUE;
	}
	BOOL rc = QueryPerformanceCounter(pcount);
	return rc != 0;
}
static unsigned long CalculateDuration(LARGE_INTEGER start, LARGE_INTEGER finish) {
	if (!freqInitialized) return 0L;
	LARGE_INTEGER elapsedMilliseconds;
	elapsedMilliseconds.QuadPart = finish.QuadPart - start.QuadPart;
	elapsedMilliseconds.QuadPart *= 1000;
	elapsedMilliseconds.QuadPart /= freq.QuadPart;
	return (unsigned long)(elapsedMilliseconds.QuadPart);
}
#endif

static char* FormatRecord(const unsigned long long gcRealEnd, const char* gcType, const long heapSize, const long heapUsed, const long gcDuration) {
	char* buffer = new char[1024];
	// TODO: use snprintf on Linux? or do other bounds checks?
	std::sprintf(buffer, "NodeGCData,%llu,%s,%ld,%ld,%ld\n", gcRealEnd, gcType, heapSize, heapUsed, gcDuration); // FIXME: should bounds check, no snprintf on windows :(
	return buffer;
}

void beforeGC(GCType type, GCCallbackFlags flags) {
	plugin::timingOK = GetSteadyTime(&plugin::gcSteadyStart);
}
void afterGC(GCType type, GCCallbackFlags flags) {
	unsigned long long gcRealEnd;
	
	// GC pause time
	if (plugin::timingOK) {
		plugin::timingOK = GetSteadyTime(&plugin::gcSteadyEnd);	
	}
	const long gcDuration = plugin::timingOK ? CalculateDuration(plugin::gcSteadyStart, plugin::gcSteadyEnd) : 0L; 

	// Get "real" time
	gcRealEnd = ibmras::common::util::getMilliseconds();

	// GC type
	const char *gcType = (type == kGCTypeMarkSweepCompact) ? "M" : "S";

	// GC heap stats
	HeapStatistics hs;
	V8::GetHeapStatistics(&hs);

	// Encode data (TODO: extract this bit into an API?)
	char *buffer = FormatRecord(gcRealEnd, gcType, (long)hs.total_heap_size(), (long)hs.used_heap_size(), gcDuration); 

	// Send data
	monitordata data;
	data.persistent = false;
	data.provID = plugin::provid;
	data.sourceID = 0;
	data.data = buffer;
	data.size = strlen(buffer);
	plugin::callback(&data);
}

pushsource* createPushSource(uint32 srcid, const char* name) {
        pushsource *src = new pushsource();
        src->header.name = name;
        std::string desc("Description for ");
        desc.append(name);
        src->header.description = desc.c_str();
        src->header.sourceID = srcid;
        src->header.config = "";
        src->next = NULL;
        src->header.capacity = (DEFAULT_CAPACITY / (srcid+1));
        return src;
}

extern "C" {
NODEGCPLUGIN_DECL pushsource* ibmras_monitoring_registerPushSource(void (*callback)(monitordata*), uint32 provID) {
        IBMRAS_DEBUG(info,  "Registering push sources");
        pushsource *head = createPushSource(0, "gc_node");
        plugin::callback = callback;
        plugin::provid = provID;
        return head;
}

NODEGCPLUGIN_DECL int ibmras_monitoring_plugin_start() {
	IBMRAS_DEBUG(info,  "Starting");
	V8::AddGCPrologueCallback(*beforeGC);
	V8::AddGCEpilogueCallback(*afterGC);
	return 0;
}

NODEGCPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
	IBMRAS_DEBUG(info,  "Stopping");

	//TODO: Implement stop method
	return 0;
}
}
