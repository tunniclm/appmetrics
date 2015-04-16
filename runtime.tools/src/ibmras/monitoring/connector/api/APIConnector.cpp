 /** 
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/connector/api/APIConnector.h"

#define DEFAULT_CAPACITY 10240  /* default bucket capacity = 1MB */

#if defined(_WINDOWS)
#define APICONNECTORPLUGIN_DECL __declspec(dllexport)   /* required for DLLs to export the plugin functions */
#else
#define APICONNECTORPLUGIN_DECL
#endif

namespace APIConnector {

const char* apiConnVersion = "1.0";

APIConnector::APIConnector() {
}

void (*listener)(const std::string&, unsigned int, void*);

int APIConnector::sendMessage(const std::string &sourceId, uint32 size, void *data) {
	if (listener != NULL) listener(sourceId, size, data);
    return size;
}

extern "C" {
APICONNECTORPLUGIN_DECL void registerListener(void(*func)(const std::string&, unsigned int, void*)){
	listener = func;
}

APICONNECTORPLUGIN_DECL void deregisterListener(){
    listener = NULL;
}

APICONNECTORPLUGIN_DECL void pushData(const std::string& sendData) {
    monitordata data;
    data.persistent = false;
    data.provID = plugin::provid;
    data.sourceID = 0;
    data.size = static_cast<uint32>(sendData.length()); // should data->size be a size_t?
    data.data = sendData.c_str();
    plugin::api.agentPushData(&data);
}

APICONNECTORPLUGIN_DECL void sendControl(const std::string topic, unsigned int length, void* message) {
	plugin::receiver->receiveMessage(topic, length, message);
}

} // end extern C

void APIConnector::registerReceiver(ibmras::monitoring::connector::Receiver *receiver) {
	plugin::receiver = receiver;
}

int APIConnector::start() {
    return 0;
}

int APIConnector::stop() {
    return 0;
}

APIConnector::~APIConnector() {
}

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


extern "C" {
APICONNECTORPLUGIN_DECL pushsource* ibmras_monitoring_registerPushSource(agentCoreFunctions api, uint32 provID) {
    plugin::api = api;
    plugin::api.logMessage(ibmras::common::logging::debug, "[api_push] Registering push sources");
    pushsource *head = createPushSource(0, "api");
    plugin::provid = provID;
    return head;
}

APICONNECTORPLUGIN_DECL void* ibmras_monitoring_getConnector(const char* properties) {
	listener = NULL;
    return new APIConnector();
}

APICONNECTORPLUGIN_DECL int ibmras_monitoring_plugin_start() {
    return 0;
}

APICONNECTORPLUGIN_DECL int ibmras_monitoring_plugin_stop() {
    return 0;
}

APICONNECTORPLUGIN_DECL const char* ibmras_monitoring_getVersion() {
	return apiConnVersion;
}

}   // extern "C"

}   // Listener namespace
