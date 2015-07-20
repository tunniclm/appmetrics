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
#include "ibmras/monitoring/Typesdef.h"
#include <cstring>
#include <string>
#include <sstream>
#include "uv.h"
#include "v8.h"
#include "nan.h"

#define DEFAULT_CAPACITY 1024

#if defined(_WINDOWS)
#define NODEENVPLUGIN_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define NODEENVPLUGIN_DECL
#endif

namespace plugin {
	agentCoreFunctions api;
	uint32 provid = 0;
	
	std::string nodeVersion;
	std::string nodeTag;
	std::string nodeVendor;
	std::string nodeName;
	std::string commandLineArguments;
}

using namespace v8;
using namespace ibmras::common::logging;

static char* NewCString(const std::string& s) {
	char *result = new char[s.length() + 1];
	std::strcpy(result, s.c_str());
	return result;
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

static std::string ToStdString(Local<String> s) {
	char *buf = new char[s->Length() + 1];
	s->WriteUtf8(buf); // TODO (florincr): not sure if this is correct or not.
	std::string result(buf);
	delete[] buf;
	return result;
}

static Local<Object> GetProcessObject() {
	return NanGetCurrentContext()->Global()->Get(NanNew<String>("process"))->ToObject();
}

static Local<Object> GetProcessConfigObject() {
	return NanGetCurrentContext()->Global()->Get(NanNew<String>("process"))->ToObject()->Get(NanNew<String>("config"))->ToObject();

}
	
static std::string GetNodeVersion() {
	Local<String> version = GetProcessObject()->Get(NanNew<String>("version"))->ToString();
	return ToStdString(version);
}

static std::string GetNodeTag() {
	Local<String> tag = GetProcessConfigObject()->Get(NanNew<String>("variables"))->ToObject()->Get(NanNew<String>("node_tag"))->ToString();
	return ToStdString(tag);
}

// Notes:
// process.argv -- array containing strings: 
//                 [0] contains path to node (as passed)
//                 [1] contains absolute path to the main module (usually a js file, the entry point to the user app)
//                 [2+] remaining entries are the args to the main module 
// process.execPath -- absolute path to node executable
// process.execArgv -- array containing strings for each argument to node (not including the main module and its args)

static std::string GetNodeArguments(const std::string separator="@@@") {
	std::stringstream ss;
	Local<Object> process = GetProcessObject();
	Local<Object> nodeArgv = process->Get(NanNew<String>("execArgv"))->ToObject();
	int64 nodeArgc = nodeArgv->Get(NanNew<String>("length"))->ToInteger()->Value();

	int written = 0;
	if (nodeArgc > 0) {
		for (int i = 0; i < nodeArgc; i++) {
			if (written++ > 0) ss << separator;
			ss << ToStdString(nodeArgv->Get(i)->ToString());
		}
	}
	
	return ss.str();
}

//static void PrintComponentVersions() {
//	Local<Object> versions = Context::GetCurrent()->Global()->Get(String::New("process"))->ToObject()->Get(String::New("versions"))->ToObject();
//	Local<Array> componentNames = versions->GetOwnPropertyNames();
//	unsigned i;
//	for (i=0; i<componentNames->Length(); i++) {
//		std::string componentName = ToStdString(componentNames->Get(i)->ToString());
//		std::string componentVersion = ToStdString(versions->Get(componentNames->Get(i))->ToString());
//		std::cout << componentName << " = " << componentVersion << std::endl;
//	}
//}

static void cleanupHandle(uv_handle_t *handle) {
	delete handle;
}

#if NODE_VERSION_AT_LEAST(0, 11, 0) // > v0.11+
static void GetNodeInformation(uv_async_t *async) {
#else
static void GetNodeInformation(uv_async_t *async, int status) {
#endif

	NanScope();
	plugin::nodeVersion = GetNodeVersion();
	plugin::nodeTag = GetNodeTag();
	if (plugin::nodeTag.find("IBMBuild") != std::string::npos) {
		plugin::nodeVendor = std::string("IBM");
		plugin::nodeName = std::string("IBM SDK for Node.js");
	} else {
		plugin::nodeName = std::string("Node.js");
	}
	plugin::commandLineArguments = GetNodeArguments();
	//PrintComponentVersions();
	uv_close((uv_handle_t*) async, cleanupHandle);
	
	if (plugin::nodeVersion != "") {
		std::stringstream contentss;
		contentss << "#EnvironmentSource\n";
		
		contentss << "runtime.version=" << plugin::nodeVersion;
		if (plugin::nodeTag != "") {
			contentss << plugin::nodeTag;
		}
		contentss << '\n';
		
		if (plugin::nodeVendor != "") {
			contentss << "runtime.vendor=" << plugin::nodeVendor << '\n';
		}
		if (plugin::nodeName != "") {
			contentss << "runtime.name=" << plugin::nodeName << '\n';
		}
		contentss << "command.line.arguments=" << plugin::commandLineArguments << '\n';
		
		std::string content = contentss.str();
		monitordata data;
		data.persistent = false;
		data.provID = plugin::provid;
		data.sourceID = 0;
		data.size = static_cast<uint32>(content.length()); // should data->size be a size_t?
		data.data = content.c_str();
		plugin::api.agentPushData(&data);
	} else {
		plugin::api.logMessage(debug, "[environment_node] Unable to get Node.js environment information");
	}

}

extern "C" {
NODEENVPLUGIN_DECL pushsource* ibmras_monitoring_registerPushSource(agentCoreFunctions api, uint32 provID) {
	plugin::api = api;
	plugin::api.logMessage(debug, "[environment_node] Registering push sources");

	pushsource *head = createPushSource(0, "environment_node");
	plugin::provid = provID;
	return head;
}

NODEENVPLUGIN_DECL int ibmras_monitoring_plugin_init(const char* properties) {
	return 0;
}

NODEENVPLUGIN_DECL int ibmras_monitoring_plugin_start() {
	plugin::api.logMessage(fine, "[environment_node] Starting");
	
	// Run GetNodeInformation() on the Node event loop
	uv_async_t *async = new uv_async_t;
	uv_async_init(uv_default_loop(), async, GetNodeInformation);
	uv_async_send(async); // close and cleanup in call back
	
	return 0;
}

NODEENVPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
	plugin::api.logMessage(fine, "[environment_node] Stopping");
	return 0;
}

NODEENVPLUGIN_DECL const char* ibmras_monitoring_getVersion() {
	return "1.0";
}
}
