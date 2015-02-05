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


#if defined(_WINDOWS)
#define APICONNECTORPLUGIN_DECL __declspec(dllexport)   /* required for DLLs to export the plugin functions */
#else
#define APICONNECTORPLUGIN_DECL
#endif

namespace APIConnector {

APIConnector::APIConnector() {
}

void (*listener)(const std::string&, unsigned int, void*);

int APIConnector::sendMessage(const std::string &sourceId, uint32 size, void *data) {
	if (listener != NULL) listener(sourceId, size, data);
    return size;
}

APICONNECTORPLUGIN_DECL void registerListener(void(*func)(const std::string&, unsigned int, void*)){
	listener = func;
}

APICONNECTORPLUGIN_DECL void deregisterListener(){
    listener = NULL;
}


void APIConnector::registerReceiver(ibmras::monitoring::connector::Receiver *receiver) {
}

int APIConnector::start() {
    return 0;
}

int APIConnector::stop() {
    return 0;
}

APIConnector::~APIConnector() {
}

extern "C" {
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

}   // extern "C"

}   // Listener namespace
