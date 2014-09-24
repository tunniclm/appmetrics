/*
 * plugin.cpp
 *
 *  Created on: 02 Jul 2014
 *      Author: Mike Tunnicliffe
 */

#include "ibmras/monitoring/Monitoring.h"
#include "ibmras/common/logging.h"
#include <iostream>
#include <cstring>
#include <string>
#include "uv.h"
#include "v8.h"

#define DEFAULT_CAPACITY 1024

#if defined(_WINDOWS)
#define NODEENVPLUGIN_DECL __declspec(dllexport)	/* required for DLLs to export the plugin functions */
#else
#define NODEENVPLUGIN_DECL
#endif

IBMRAS_DEFINE_LOGGER("NodeEnvPlugin");

namespace plugin {
uint32 provid = 0;
const char *nodeVersion = NULL;
const char *nodeTag = NULL;
const char *nodeVendor = NULL;
}

using namespace v8;

monitordata* OnRequestData() {
	monitordata *data = new monitordata;
	data->persistent = false;
	data->provID = plugin::provid;
	data->sourceID = 0;
	data->size = 0;
	data->data = NULL;
	
	if (plugin::nodeVersion != NULL) {
		std::string content("#EnvironmentSource\n");
		content += "system.java.specification.version=";
		content += plugin::nodeVersion;
		content += "\nsdk.version=";
		content += plugin::nodeVersion;
		if (plugin::nodeTag != NULL) {
			content += plugin::nodeTag;
		}
		if (plugin::nodeVendor != NULL) {
			content += "\nsdk.vendor=";
			content += plugin::nodeVendor;
		}
		data->size = content.length();
		data->data = strdup(content.c_str());
	}		
	
	return data;
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
	src->header.capacity = DEFAULT_CAPACITY;
	src->callback = OnRequestData;
	src->complete = OnComplete;
	src->pullInterval = 20*60;
	return src;
}

char *NewCString(Local<String> s) {
	char *buf = new char[s->Length() + 1];
	s->WriteAscii(buf);
	return buf;
}
	
char * GetNodeVersion() {
	Local<String> version = Context::GetCurrent()->Global()->Get(String::New("process"))->ToObject()->Get(String::New("version"))->ToString();
	return NewCString(version);
}

char * GetNodeTag() {
	Local<String> tag = Context::GetCurrent()->Global()->Get(String::New("process"))->ToObject()->Get(String::New("config"))->ToObject()->Get(String::New("variables"))->ToObject()->Get(String::New("node_tag"))->ToString();
	return NewCString(tag);
}

void PrintComponentVersions() {
	Local<Object> versions = Context::GetCurrent()->Global()->Get(String::New("process"))->ToObject()->Get(String::New("versions"))->ToObject();
	Local<Array> componentNames = versions->GetOwnPropertyNames();
	unsigned i;
	for (i=0; i<componentNames->Length(); i++) {
		char *componentName_cstr = NewCString(componentNames->Get(i)->ToString());
		char *componentVersion_cstr = NewCString(versions->Get(componentNames->Get(i))->ToString()); 
		std::cout << componentName_cstr << " = " << componentVersion_cstr << std::endl;
		delete[] componentName_cstr;
		delete[] componentVersion_cstr;
	}
}

uv_async_t async;

void GetNodeInformation(uv_async_t *handle, int status) {
	HandleScope scope;
	plugin::nodeVersion = GetNodeVersion();
	plugin::nodeTag = GetNodeTag();
	if (strstr(plugin::nodeTag, "IBMBuild")) {
		plugin::nodeVendor = "IBM";
	}
	PrintComponentVersions();
	uv_close((uv_handle_t*) &async, NULL);
}

extern "C" {
NODEENVPLUGIN_DECL pullsource* ibmras_monitoring_registerPullSource(uint32 provID) {
	IBMRAS_DEBUG(info,  "Registering pull sources");
	pullsource *head = createPullSource(0, "environment_node");
	plugin::provid = provID;
	return head;
}

NODEENVPLUGIN_DECL int ibmras_monitoring_plugin_start() {
	IBMRAS_DEBUG(info,  "Starting");
	
	// Run GetNodeInformation() on the Node event loop
	uv_async_init(uv_default_loop(), &async, GetNodeInformation);
	uv_async_send(&async);
	return 0;
}

NODEENVPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
	IBMRAS_DEBUG(info,  "Stopping");

	//TODO: Implement stop method
	return 0;
}
}