 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/logging.h"
#include <iostream>
#include <cstring>
#include <string>
#include "v8.h"
#include "v8-profiler.h"
#include "uv.h"


#define DEFAULT_CAPACITY 10240  /* default bucket capacity = 1MB */

IBMRAS_DEFINE_LOGGER("NodeProfPlugin");

namespace plugin {
uint32 provid = 0;
int profileIdx = -1;
monitordata *latest = new monitordata();
}

using namespace v8;

static void monitordata_ResetToBlank(monitordata *data) {
	if (data->data != NULL) { delete[] data->data; }
	data->persistent = false;
	data->provID = plugin::provid;
	data->sourceID = 0;
	data->size = 0;
	data->data = NULL;
}

static void monitordata_Copy(const monitordata *src, monitordata *dest) {
	dest->persistent = src->persistent;
	dest->provID = src->provID;
	dest->sourceID = src->sourceID;
	dest->size = src->size;
	dest->data = src->data == NULL ? NULL : strdup(src->data);
}

monitordata* OnRequestData() {
	std::cerr << "--- OnRequestData ---" << std::endl;
	monitordata *data = new monitordata;
	monitordata_Copy(plugin::latest, data);
	return data;
}

static const CpuProfile * GetProfile() {
	// Get profile
	Handle<String> s = String::New("NodeProfPlugin"); 
	const CpuProfile *p = CpuProfiler::StopProfiling(s);
	CpuProfiler::StartProfiling(s);
	return p;
//	if (plugin::profileIdx == -1) { // Should I be using the UID? (unique id?) CpuProfile;:FindProfile(unsigned uid)
//		std::cerr << CpuProfiler::GetProfilesCount() << " profiles" << std::endl;
//		int i;
//		for (i = 0; i < CpuProfiler::GetProfilesCount(); i++) {
//			const CpuProfile *p = CpuProfiler::GetProfile(i);
//			Handle<String> title = p->GetTitle();
//			char *titleS = new char[title->Length() + 1];
//			p->GetTitle()->WriteAscii(titleS, 0, p->GetTitle()->Length());
//			if (strcmp(titleS, "NodeProfPlugin") == 0) {
//				profile = p;
//				plugin::profileIdx = i;
//				break;
//			}
//			delete[] titleS;
//		}
//	} else {
//		profile = CpuProfiler::GetProfile(plugin::profileIdx);
//	}
//	return profile;
}

static bool ExtractV8String(const Handle<String> v8string, char **cstring) {
	*cstring = new char[v8string->Length() + 1];
	if (*cstring == NULL) return false;
	v8string->WriteAscii(*cstring);
	return true;
}

static char * ConstructNodeData(const int level, const CpuProfileNode *node) {
	int line = node->GetLineNumber();
	double selfSamples = node->GetSelfSamplesCount();
	double totalSamples = node->GetTotalSamplesCount();
	// ? line == CpuProfileNode::kNoLineNumberInfo ?

	char *function, *script;
	if (!ExtractV8String(node->GetFunctionName(), &function)) {
		return NULL;
	}
	if (!ExtractV8String(node->GetScriptResourceName(), &script)) {
		delete[] function;
		return NULL;
	}

	char *buffer = new char[1024];
	int written = snprintf(buffer, 1024, "%d,%s,%s,%d,%lf,%lf",
		level, script, function, line, selfSamples, totalSamples
	);
	if (written + 1 > 1024) {
		delete[] buffer;
		buffer = NULL;
	}

	// clean up
	delete[] script;
	delete[] function;
	return buffer;
}

static char * ConstructData(const CpuProfile *profile) {
	//assert(profile != NULL);
	char *buffer = NULL;
	const CpuProfileNode *topRoot = profile->GetTopDownRoot();
	const CpuProfileNode *current = topRoot;

	char *nodeData = ConstructNodeData(0, current);
	if (nodeData == NULL) return NULL;

	int length = strlen(nodeData) + 1;

	const int children = current->GetChildrenCount();
	int childrenAllocated = 0;
	char **childrenNodeData = new char*[children];
	if (childrenNodeData == NULL) goto cleanup1;

	const CpuProfileNode *child;
	int i;
	for (i = 0; i < children; i++) {
		child = current->GetChild(i);
		childrenNodeData[i] = ConstructNodeData(1, child);
		if (childrenNodeData[i] == NULL) {
			goto cleanup2;
		}
		childrenAllocated++;
		length += strlen(childrenNodeData[i]) + 1;
	}

	buffer = new char[length + 1];
	if (buffer == NULL) goto cleanup2;

	strcpy(buffer, nodeData);
	for (i = 0; i < children; i++) {
		strcat(buffer, "\n");
		strcat(buffer, childrenNodeData[i]); 
	}
	strcat(buffer, "\n");

cleanup2:
	for (i = 0; i < childrenAllocated; i++) { delete[] childrenNodeData[i]; }
	delete[] childrenNodeData;
cleanup1:
	delete[] nodeData;
	return buffer;
}

void OnGatherDataOnV8Thread(uv_timer_s *data, int status) {
	monitordata_ResetToBlank(plugin::latest);
	if (Isolate::GetCurrent() == NULL) { std::cerr << "*** NO ISOLATE ***" << std::endl; return; }
	std::cerr << "-- ISOLATE VALID ---" << std::endl;
	HandleScope scope;
	const CpuProfile *profile = GetProfile();
	if (profile != NULL) {
		std::cerr << "--- PROFILE FOUND ---" << std::endl;
		plugin::latest->data = ConstructData(profile);
		if (plugin::latest->data != NULL) {
			plugin::latest->size = strlen(plugin::latest->data);
		}
	}
	scope.Close(Undefined());
}

void OnComplete(monitordata* data) {
	delete[] data->data;
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
    src->header.capacity = (DEFAULT_CAPACITY / (srcid+1));
	src->callback = OnRequestData;
	src->complete = OnComplete;
    return src;
}

pullsource* ibmras_monitoring_registerPullSource(uint32 provID) {
        IBMRAS_DEBUG(info,  "Registering pull sources");
        pullsource *head = createPullSource(0, "profiling_node");
        plugin::provid = provID;
        return head;
}

int ibmras_monitoring_plugin_start() {
	IBMRAS_DEBUG(info,  "Starting");
	monitordata_ResetToBlank(plugin::latest);
	CpuProfiler::StartProfiling(String::New("NodeProfPlugin"));
	uv_timer_t *timer = new uv_timer_t;
	uv_timer_init(uv_default_loop(), timer);
	uv_unref((uv_handle_t*) timer); // don't prevent event loop exit
	uv_timer_start(timer, OnGatherDataOnV8Thread, 5000, 5000);
	return 0;
}


int ibmras_monitoring_plugin_stop() {
	IBMRAS_DEBUG(info,  "Stopping");

	//TODO: Implement stop method
	return 0;
}
