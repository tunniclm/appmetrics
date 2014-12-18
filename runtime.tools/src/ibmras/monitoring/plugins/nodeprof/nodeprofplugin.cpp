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
#include "ibmras/common/Properties.h"
#include "ibmras/common/util/sysUtils.h"
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include "v8.h"
#include "v8-profiler.h"
#include "uv.h"


#define DEFAULT_CAPACITY 10240  /* CHECK(tunniclm): Is this a good default size? */

#if defined(_WINDOWS)
#define NODEPROFPLUGIN_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define NODEPROFPLUGIN_DECL
#endif

IBMRAS_DEFINE_LOGGER("NodeProfPlugin");

namespace plugin {
	void (*callback)(monitordata*);
	uint32 provid = 0;
	int profileIdx = -1;
}

using namespace v8;

static char* NewCString(const std::string& s) {
	char *result = new char[s.length() + 1];
	std::strcpy(result, s.c_str());
	return result;
}

static bool ExtractV8String(const Handle<String> v8string, char **cstring) {
	*cstring = new char[v8string->Length() + 1];
	if (*cstring == NULL) return false;
	v8string->WriteAscii(*cstring);
	return true;
}

static void ConstructNodeData(const CpuProfileNode *node, int id, int parentId, std::stringstream &result) {
	int line = node->GetLineNumber(); // CHECK(tunniclm): Is this deprecated, if so in which version/what is replacement?
	double selfSamples = node->GetSelfSamplesCount(); // CHECK(tunniclm): Is this deprecated, if so in which version/what is replacement?
	double totalSamples = node->GetTotalSamplesCount(); // CHECK(tunniclm): Is this deprecated, if so in which version/what is replacement?
	// ? line == CpuProfileNode::kNoLineNumberInfo ?

	char *function, *script;
	if (!ExtractV8String(node->GetFunctionName(), &function)) {
		return;
	}
	if (!ExtractV8String(node->GetScriptResourceName(), &script)) {
		delete[] function;
		return;
	}

	result << "NodeProfData,Node," << id << ',' << parentId << ',';
	result << script << ',' << function << ',' << line << ',' << selfSamples << ',' << totalSamples << '\n';

	// clean up
	delete[] script;
	delete[] function;
}

typedef void visit_callback(const CpuProfileNode *, int, int, std::stringstream &result);
static void visit(const CpuProfileNode *current, visit_callback *cb, int parentId, std::stringstream &result) {
	static int nextid = 1;
	int id = nextid++;
	cb(current, id, parentId, result);

	int children = current->GetChildrenCount();
	for (int i=0; i<children; i++) {
		visit(current->GetChild(i), cb, id, result);
	}
}

static char * ConstructData(const CpuProfile *profile) {
	//assert(profile != NULL);
	//char *buffer = NULL;
	const CpuProfileNode *topRoot = profile->GetTopDownRoot();
	//const CpuProfileNode *current = topRoot;

	std::stringstream result;
	result << "NodeProfData,Start," << ibmras::common::util::getMilliseconds() << '\n';
	visit(topRoot, ConstructNodeData, 0, result);
	result << "NodeProfData,End" << '\n';
	return NewCString(result.str());
}

void OnGatherDataOnV8Thread(uv_timer_s *data, int status) {
	if (Isolate::GetCurrent() == NULL) { 
		IBMRAS_DEBUG(debug, "No V8 Isolate found when gathering method profile"); // CHECK(tunniclm): Should this be a warning?
		return; 
	}
	
	HandleScope scope;
	
	// Get profile
	Handle<String> s = String::New("NodeProfPlugin"); 
	const CpuProfile *profile = CpuProfiler::StopProfiling(s); // CHECK(tunniclm): do we need to release the CpuProfile here? Check the V8 API
	CpuProfiler::StartProfiling(s);

	if (profile != NULL) {
		char *serialisedProfile = ConstructData(profile);
		if (serialisedProfile != NULL) {
			// Send data to agent
			monitordata data;
			data.persistent = false;
			data.provID = plugin::provid;
			data.sourceID = 0;
			data.size = static_cast<uint32>(strlen(serialisedProfile)); // should data->size be a size_t?
			data.data = serialisedProfile;
			plugin::callback(&data);
			
			delete[] serialisedProfile;
		} else {
			IBMRAS_DEBUG(debug, "Failed to serialise method profile"); // CHECK(tunniclm): Should this be a warning?
		}
	} else {
		IBMRAS_DEBUG(debug, "No method profile found"); // CHECK(tunniclm): Should this be a warning?
	}
	
	scope.Close(Undefined());
}

pushsource* createPushSource(uint32 srcid, const char* name) {
	pushsource *src = new pushsource();
	src->header.name = name;
	std::string desc("Description for ");
	desc.append(name);
	src->header.description = NewCString(desc);
	src->header.sourceID = srcid;
	src->next = NULL;
	src->header.capacity = DEFAULT_CAPACITY;
	return src;
}

extern "C" {
NODEPROFPLUGIN_DECL pushsource* ibmras_monitoring_registerPushSource(void (*callback)(monitordata*), uint32 provID) {
	IBMRAS_DEBUG(info,  "Registering push sources");
	pushsource *head = createPushSource(0, "profiling_node");
	plugin::callback = callback;
	plugin::provid = provID;
	return head;
}

NODEPROFPLUGIN_DECL int ibmras_monitoring_plugin_init(const char* properties) {
	ibmras::common::Properties props;
	props.add(properties);

	std::string loggingProp = props.get("com.ibm.diagnostics.healthcenter.logging.level");
	ibmras::common::LogManager::getInstance()->setLevel("level", loggingProp);
	loggingProp = props.get("com.ibm.diagnostics.healthcenter.logging.NodeProfPlugin");
	ibmras::common::LogManager::getInstance()->setLevel("NodeProfPlugin", loggingProp);
	
	return 0;
}

NODEPROFPLUGIN_DECL int ibmras_monitoring_plugin_start() {
	IBMRAS_DEBUG(info,  "Starting");
	
	HandleScope scope;
	CpuProfiler::StartProfiling(String::New("NodeProfPlugin"));
	uv_timer_t *timer = new uv_timer_t;
	uv_timer_init(uv_default_loop(), timer);
	uv_unref((uv_handle_t*) timer); // don't prevent event loop exit
	uv_timer_start(timer, OnGatherDataOnV8Thread, 5000, 5000);
	scope.Close(Undefined());
	
	return 0;
}

NODEPROFPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
	IBMRAS_DEBUG(info,  "Stopping");

	HandleScope scope;
	const CpuProfile *profile = CpuProfiler::StopProfiling(String::New("NodeProfPlugin")); // CHECK(tunniclm): do we need to release the CpuProfile here? Check the V8 API
	scope.Close(Undefined());
	return 0;
}
}
